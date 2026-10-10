// G2MEAB prototype translation unit (GameCube port layer); complete reconstruction of the retained
// natives. .text: 0x80622C90..0x806236F0 (23 native functions).
// The 4.06 PS3 port is a different platform; only the FMOD_OS_* interface is shared. Library_Load,
// Library_GetProcAddress and Debug_OutputStr are not retained natively.

#include "fmod_os_misc.h"
#include "fmod.h"
#include "fmod_memory.h"

#include <dolphin/os.h>
#include <dolphin/dvd.h>
#include <string.h>

// File handle: the DVD file info followed by the read position (0x40 bytes, File_Open 0x80622D7C).
struct FMOD_OS_FILE // Guessed name
{
    DVDFileInfo mInfo; // offset 0x0
    unsigned int mPosition; // offset 0x3C
};

// Sleep alarm: the alarm handler resumes the thread stored after the alarm (Time_Sleep 0x80623228).
struct FMOD_OS_SLEEPALARM // Guessed name
{
    OSAlarm mAlarm; // offset 0x0
    OSThread * mThread; // offset 0x28
};

static OSSemaphore gMemoryCrit; // Guessed name
static unsigned char * gReadBuffer; // Guessed name

void * FMOD_OS_Memory_Alloc(int size)
{
    return OSAllocFromHeap(__OSCurrHeap, (size + 31) & ~31);
}

void * FMOD_OS_Memory_Realloc(void * ptr, int size)
{
    void * newptr = FMOD_OS_Memory_Alloc(size);

    if (newptr && ptr)
    {
        unsigned int oldsize = ((unsigned int *)ptr)[-6] - 32;

        memcpy(newptr, ptr, (unsigned int)size > oldsize ? oldsize : size);
    }

    if (ptr)
    {
        FMOD_OS_Memory_Free(ptr);
    }

    return newptr;
}

void FMOD_OS_Memory_Free(void * ptr)
{
    OSFreeToHeap(__OSCurrHeap, ptr);
}

FMOD_RESULT FMOD_OS_File_Open(const char * name, char * mode, int unicode, unsigned int * filesize, void * * handle)
{
    FMOD_OS_FILE * file;

    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    file = (FMOD_OS_FILE *)FMOD_Memory_Calloc(sizeof(FMOD_OS_FILE));
    if (!file)
    {
        return FMOD_ERR_MEMORY;
    }

    if (!strchr(mode, 'r'))
    {
        FMOD_Memory_Free(file);
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!DVDOpen(name, &file->mInfo))
    {
        FMOD_Memory_Free(file);
        return FMOD_ERR_FILE_NOTFOUND;
    }

    file->mPosition = 0;

    if (filesize)
    {
        *filesize = (file->mInfo.length + 31) & ~31;
    }

    *handle = file;

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_File_Close(void * handle)
{
    FMOD_OS_FILE * file = (FMOD_OS_FILE *)handle;

    if (!file)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    DVDClose(&file->mInfo);

    FMOD_Memory_Free(file);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_File_ReadDVD(FMOD_OS_FILE * file, void * buf, unsigned int count, unsigned int offset, unsigned int * read) // Guessed name
{
    if (DVDGetDriveStatus() == DVD_STATE_FATAL_ERROR)
    {
        return FMOD_ERR_FILE_BAD;
    }

    if (DVDReadAsyncPrio(&file->mInfo, buf, count, offset, 0, 2))
    {
        for (;;)
        {
            bool ready = true;

            switch (DVDGetDriveStatus())
            {
                case DVD_STATE_FATAL_ERROR:
                {
                    return FMOD_ERR_FILE_BAD;
                }
                case DVD_STATE_NO_DISK:
                {
                    ready = false;
                    break;
                }
                case DVD_STATE_COVER_OPEN:
                {
                    ready = false;
                    break;
                }
                case DVD_STATE_WRONG_DISK:
                {
                    ready = false;
                    break;
                }
                case DVD_STATE_RETRY:
                {
                    ready = false;
                    break;
                }
            }

            if (ready)
            {
                switch (DVDGetCommandBlockStatus(&file->mInfo.cb))
                {
                    case DVD_STATE_FATAL_ERROR:
                    {
                        return FMOD_ERR_FILE_BAD;
                    }
                    case DVD_STATE_END:
                    {
                        *read = count;
                        return FMOD_OK;
                    }
                    case DVD_STATE_RETRY:
                    {
                        return FMOD_ERR_FILE_BAD;
                    }
                }
            }

            FMOD_OS_Time_Sleep(1);
        }
    }

    *read = 0;
    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_File_Read(void * handle, void * buf, unsigned int count, unsigned int * read)
{
    FMOD_RESULT result;
    FMOD_OS_FILE * file = (FMOD_OS_FILE *)handle;
    unsigned int toread = count;
    unsigned int remainder = 0;
    unsigned int bytesread;
    unsigned int extraread;

    if (!file)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (file->mPosition >= file->mInfo.length)
    {
        *read = 0;
        return FMOD_ERR_FILE_EOF;
    }

    if ((unsigned int)buf & 31)
    {
        *read = 0;
        return FMOD_ERR_FILE_BAD;
    }

    if (file->mPosition + toread > file->mInfo.length)
    {
        toread = file->mInfo.length - file->mPosition;
        remainder = toread & 31;
        toread &= ~31;
    }

    result = FMOD_OS_File_ReadDVD(file, buf, toread, file->mPosition, &bytesread);
    if (result != FMOD_OK)
    {
        return result;
    }

    file->mPosition += bytesread;

    if (remainder)
    {
        if (!gReadBuffer)
        {
            gReadBuffer = (unsigned char *)FMOD_Memory_Calloc(32);
            if (!gReadBuffer)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        result = FMOD_OS_File_ReadDVD(file, gReadBuffer, 32, file->mPosition, &extraread);
        if (result != FMOD_OK)
        {
            return result;
        }

        memcpy((char *)buf + bytesread, gReadBuffer, remainder);

        file->mPosition += remainder;
        *read = bytesread + remainder;
        return FMOD_OK;
    }

    *read = bytesread;

    if (bytesread != count)
    {
        return FMOD_ERR_FILE_EOF;
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_File_Seek(void * handle, int offset)
{
    FMOD_OS_FILE * file = (FMOD_OS_FILE *)handle;

    if (!file)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (offset >= 0)
    {
        file->mPosition = offset;
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Time_GetNs(unsigned int * ns)
{
    *ns = (unsigned int)((OSTime)OSGetTick() / (OS_TIMER_CLOCK / 1000000));
    return FMOD_OK;
}

void FMOD_OS_Time_SleepAlarm(OSAlarm * alarm, OSContext * context) // Guessed name
{
    OSResumeThread(((FMOD_OS_SLEEPALARM *)alarm)->mThread);
}

FMOD_RESULT FMOD_OS_Time_Sleep(unsigned int ms)
{
    if (!ms)
    {
        OSYieldThread();
    }
    else
    {
        FMOD_OS_SLEEPALARM sleepalarm;

        sleepalarm.mThread = OSGetCurrentThread();
        OSCreateAlarm(&sleepalarm.mAlarm);
        OSSetAlarm(&sleepalarm.mAlarm, OSMillisecondsToTicks(ms), FMOD_OS_Time_SleepAlarm);
        OSSuspendThread(OSGetCurrentThread());
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Thread_GetCurrentID(unsigned int * id)
{
    *id = (unsigned int)OSGetCurrentThread();
    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Thread_Create(const char * name, void * (* func)(void *), void * param, FMOD_THREAD_PRIORITY priority, void * stack, int stacksize, void * * handle)
{
    unsigned char * stackmem;
    OSThread * thread;
    OSPriority threadpriority;

    if (!handle)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    stackmem = (unsigned char *)FMOD_Memory_Calloc(stacksize);
    if (!stackmem)
    {
        return FMOD_ERR_MEMORY;
    }

    thread = (OSThread *)(stackmem + stacksize - sizeof(OSThread));
    ((unsigned int *)thread)[-1] = 1;

    switch (priority)
    {
        case FMOD_THREAD_PRIORITY_VERYLOW:
        {
            threadpriority = 31;
            break;
        }
        case FMOD_THREAD_PRIORITY_LOW:
        {
            threadpriority = 24;
            break;
        }
        case FMOD_THREAD_PRIORITY_NORMAL:
        {
            threadpriority = 16;
            break;
        }
        case FMOD_THREAD_PRIORITY_HIGH:
        {
            threadpriority = 12;
            break;
        }
        case FMOD_THREAD_PRIORITY_VERYHIGH:
        {
            threadpriority = 8;
            break;
        }
        case FMOD_THREAD_PRIORITY_CRITICAL:
        {
            threadpriority = 0;
            break;
        }
    }

    if (OSCreateThread(thread, func, param, stackmem + stacksize - sizeof(OSThread) - 4, stacksize - sizeof(OSThread) - 4, threadpriority, OS_THREAD_ATTR_DETACH))
    {
        *handle = thread;
        OSResumeThread(thread);
    }
    else
    {
        FMOD_Memory_Free(thread->stackEnd);
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Thread_Destroy(void * handle)
{
    OSThread * thread = (OSThread *)handle;

    OSCheckActiveThreads();

    FMOD_Memory_Free(thread->stackEnd);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_CriticalSection_Create(FMOD_OS_CRITICALSECTION * * crit, bool memorycrit)
{
    OSSemaphore * sem;

    if (!crit)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (memorycrit)
    {
        sem = &gMemoryCrit;
    }
    else
    {
        sem = (OSSemaphore *)FMOD_Memory_Calloc(sizeof(OSSemaphore));
        if (!sem)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    OSInitSemaphore(sem, 1);

    *crit = (FMOD_OS_CRITICALSECTION *)sem;

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_CriticalSection_Free(FMOD_OS_CRITICALSECTION * crit)
{
    if (!crit)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if ((OSSemaphore *)crit == &gMemoryCrit)
    {
        return FMOD_OK;
    }

    FMOD_Memory_Free(crit);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_CriticalSection_Enter(FMOD_OS_CRITICALSECTION * crit)
{
    if (!crit)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    OSWaitSemaphore((OSSemaphore *)crit);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_CriticalSection_Leave(FMOD_OS_CRITICALSECTION * crit)
{
    if (!crit)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    OSSignalSemaphore((OSSemaphore *)crit);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Semaphore_Create(FMOD_OS_SEMAPHORE * * sema)
{
    OSSemaphore * sem = (OSSemaphore *)FMOD_Memory_Calloc(sizeof(OSSemaphore));

    if (!sem)
    {
        return FMOD_ERR_MEMORY;
    }

    OSInitSemaphore(sem, 0);

    *sema = (FMOD_OS_SEMAPHORE *)sem;

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Semaphore_Free(FMOD_OS_SEMAPHORE * sema)
{
    if (!sema)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_Memory_Free(sema);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Semaphore_Wait(FMOD_OS_SEMAPHORE * sema)
{
    if (!sema)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    OSWaitSemaphore((OSSemaphore *)sema);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Semaphore_Signal(FMOD_OS_SEMAPHORE * sema, bool interrupt)
{
    if (!sema)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    OSSignalSemaphore((OSSemaphore *)sema);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Library_Free(FMOD_OS_LIBRARY * handle)
{
    return FMOD_ERR_UNSUPPORTED;
}
