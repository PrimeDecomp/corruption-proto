// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OS_MISC_H
#define _FMOD_OS_MISC_H

enum FMOD_THREAD_PRIORITY {
    FMOD_THREAD_PRIORITY_VERYLOW = -2,
    FMOD_THREAD_PRIORITY_LOW = -1,
    FMOD_THREAD_PRIORITY_NORMAL = 0,
    FMOD_THREAD_PRIORITY_HIGH = 1,
    FMOD_THREAD_PRIORITY_VERYHIGH = 2,
    FMOD_THREAD_PRIORITY_CRITICAL = 3
};

typedef struct FMOD_OS_CRITICALSECTION FMOD_OS_CRITICALSECTION;
typedef struct FMOD_OS_SEMAPHORE FMOD_OS_SEMAPHORE;
typedef struct FMOD_OS_LIBRARY FMOD_OS_LIBRARY;
#include "fmod.h"

void * FMOD_OS_Memory_Alloc(int size);
void * FMOD_OS_Memory_Realloc(void * ptr, int size);
void FMOD_OS_Memory_Free(void * ptr);
FMOD_RESULT FMOD_OS_File_Open(const char * name, char * mode, int unicode, unsigned int * filesize, void * * handle);
FMOD_RESULT FMOD_OS_File_Close(void * handle);
FMOD_RESULT FMOD_OS_File_Read(void * handle, void * buf, unsigned int count, unsigned int * read);
FMOD_RESULT FMOD_OS_File_Seek(void * handle, int offset);
FMOD_RESULT FMOD_OS_Debug_OutputStr(const char * s);
FMOD_RESULT FMOD_OS_Time_GetNs(unsigned int * ns);
FMOD_RESULT FMOD_OS_Time_Sleep(unsigned int ms);
FMOD_RESULT FMOD_OS_Thread_GetCurrentID(unsigned int * id);
FMOD_RESULT FMOD_OS_Thread_Create(const char * name, void * (* func)(void *), void * param, FMOD_THREAD_PRIORITY priority, void * stack, int stacksize, void * * handle);
FMOD_RESULT FMOD_OS_Thread_Destroy(void * handle);
FMOD_RESULT FMOD_OS_CriticalSection_Create(FMOD_OS_CRITICALSECTION * * crit, bool memorycrit);
FMOD_RESULT FMOD_OS_CriticalSection_Free(FMOD_OS_CRITICALSECTION * crit);
FMOD_RESULT FMOD_OS_CriticalSection_Enter(FMOD_OS_CRITICALSECTION * crit);
FMOD_RESULT FMOD_OS_CriticalSection_Leave(FMOD_OS_CRITICALSECTION * crit);
FMOD_RESULT FMOD_OS_Semaphore_Create(FMOD_OS_SEMAPHORE * * sema);
FMOD_RESULT FMOD_OS_Semaphore_Free(FMOD_OS_SEMAPHORE * sema);
FMOD_RESULT FMOD_OS_Semaphore_Wait(FMOD_OS_SEMAPHORE * sema);
FMOD_RESULT FMOD_OS_Semaphore_Signal(FMOD_OS_SEMAPHORE * sema, bool interrupt);
FMOD_RESULT FMOD_OS_Library_Load(const char * dllname, FMOD_OS_LIBRARY * * handle);
FMOD_RESULT FMOD_OS_Library_GetProcAddress(FMOD_OS_LIBRARY * handle, const char * procname, void * * address);
FMOD_RESULT FMOD_OS_Library_Free(FMOD_OS_LIBRARY * handle);

#endif
