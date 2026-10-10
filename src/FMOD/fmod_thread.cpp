// Complete reconstruction of the G2MEAB unit (.text 0x80621208..0x80621568). getCurrentThreadID is
// dead-stripped and stays as an empty placeholder.

#include "fmod_thread.h"
#include "fmod.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_time.h"

namespace FMOD {

void * Thread::callback(void * data)
{
    Thread * thread = (Thread *)data;

    thread->mRunning = true;

    while (thread->mRunning)
    {
        if (thread->mSema)
        {
            FMOD_OS_Semaphore_Wait(thread->mSema);
        }

        if (!thread->mRunning)
        {
            break;
        }

        if (thread->mUserCallback)
        {
            thread->mUserCallback(thread->mUserData);
        }
        else
        {
            thread->threadFunc();
        }

        if (thread->mPeriod)
        {
            FMOD_Time_Sleep(thread->mPeriod);
        }
    }

    FMOD_OS_Semaphore_Signal(thread->mEndSema, false);

    return 0;
}

FMOD_RESULT Thread::threadFunc()
{
    return FMOD_OK;
}

Thread::Thread()
{
    mHandle = 0;
    mRunning = false;
    mStack = 0;
    mUserData = 0;
    mSema = 0;
    mEndSema = 0;
    mPeriod = 0;
    mUserCallback = 0;
}

FMOD_RESULT Thread::initThread(const char * name, void (* func)(void *), void * userdata, PRIORITY priority, void * stack, int stacksize, bool usesemaphore, int sleepperiod)
{
    FMOD_RESULT result;
    FMOD_THREAD_PRIORITY pri;

    mUserCallback = func;
    mUserData = userdata;
    mPeriod = sleepperiod;

    if (usesemaphore)
    {
        result = FMOD_OS_Semaphore_Create(&mSema);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    switch (priority)
    {
        case PRIORITY_VERYLOW:
        {
            pri = FMOD_THREAD_PRIORITY_VERYLOW;
            break;
        }
        case PRIORITY_LOW:
        {
            pri = FMOD_THREAD_PRIORITY_LOW;
            break;
        }
        case PRIORITY_NORMAL:
        {
            pri = FMOD_THREAD_PRIORITY_NORMAL;
            break;
        }
        case PRIORITY_HIGH:
        {
            pri = FMOD_THREAD_PRIORITY_HIGH;
            break;
        }
        case PRIORITY_VERYHIGH:
        {
            pri = FMOD_THREAD_PRIORITY_VERYHIGH;
            break;
        }
        case PRIORITY_CRITICAL:
        {
            pri = FMOD_THREAD_PRIORITY_CRITICAL;
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    if (name)
    {
        FMOD_strncpy(mName, name, 256);
    }
    else
    {
        FMOD_strcpy(mName, "?????");
    }

    result = FMOD_OS_Thread_Create(name, callback, this, pri, stack, stacksize, &mHandle);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT Thread::closeThread()
{
    FMOD_RESULT result;

    if (mRunning)
    {
        result = FMOD_OS_Semaphore_Create(&mEndSema);
        if (result != FMOD_OK)
        {
            return result;
        }

        mRunning = false;

        if (mSema)
        {
            result = FMOD_OS_Semaphore_Signal(mSema, false);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = FMOD_OS_Semaphore_Wait(mEndSema);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mSema)
        {
            result = FMOD_OS_Semaphore_Free(mSema);
            if (result != FMOD_OK)
            {
                return result;
            }
            mSema = 0;
        }

        result = FMOD_OS_Semaphore_Free(mEndSema);
        if (result != FMOD_OK)
        {
            return result;
        }
        mEndSema = 0;

        if (mStack)
        {
            FMOD_Memory_Free(mStack);
            mStack = 0;
        }

        result = FMOD_OS_Thread_Destroy(mHandle);
        if (result != FMOD_OK)
        {
            return result;
        }
        mHandle = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT Thread::getCurrentThreadID(unsigned int * id)
{
}

FMOD_RESULT Thread::wakeupThread(bool frominterrupt)
{
    if (mSema)
    {
        return FMOD_OS_Semaphore_Signal(mSema, frominterrupt);
    }

    return FMOD_OK;
}

} // namespace FMOD
