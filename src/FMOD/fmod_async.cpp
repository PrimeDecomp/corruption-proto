// Complete reconstruction of the G2MEAB unit (.text 0x805B60A0..0x805B67C0), in native order. The
// LinkedListNode destructor 0x805B6184 and the AsyncThread destructor 0x805B66E4 are compiler-emitted.
// addCallback, removeCallback and wakeupThread are not retained and stay as empty placeholders.

#include "fmod_async.h"
#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

namespace FMOD {

FMOD_OS_CRITICALSECTION * AsyncThread::gAsyncCrit;
LinkedListNode AsyncThread::gAsyncHead;

void asyncThreadFunc(void * data)
{
    AsyncThread * asyncthread = (AsyncThread *)data;

    asyncthread->threadFunc();
}

AsyncThread::AsyncThread()
{
    mCrit = 0;
    mThreadActive = false;
    mBusy = false;
    mDone = false;

    if (!gAsyncCrit)
    {
        FMOD_RESULT result;

        result = FMOD_OS_CriticalSection_Create(&gAsyncCrit, false);
        if (result != FMOD_OK)
        {
            return;
        }
    }
}

FMOD_RESULT AsyncThread::shutDown()
{
    if (gAsyncCrit)
    {
        LinkedListNode * current;

        FMOD_OS_CriticalSection_Enter(gAsyncCrit);

        current = gAsyncHead.getNext();
        while (current != &gAsyncHead)
        {
            LinkedListNode * next = current->getNext();

            ((AsyncThread *)current)->reallyRelease();
            current = next;
        }

        FMOD_OS_CriticalSection_Leave(gAsyncCrit);

        if (gAsyncCrit)
        {
            FMOD_OS_CriticalSection_Free(gAsyncCrit);
            gAsyncCrit = 0;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::init(bool owned)
{
    FMOD_RESULT result;

    mOwned = owned;

    result = FMOD_OS_CriticalSection_Create(&mCrit, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mThread.initThread("FMOD thread for FMOD_NONBLOCKING", asyncThreadFunc, this, Thread::PRIORITY_NORMAL, 0, 16 * 1024, false, 10);
    if (result != FMOD_OK)
    {
        return result;
    }

    mThreadActive = true;

    FMOD_OS_CriticalSection_Enter(gAsyncCrit);
    addBefore(&gAsyncHead);
    FMOD_OS_CriticalSection_Leave(gAsyncCrit);

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::release()
{
    if (mOwned)
    {
        mDone = true;
    }

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::reallyRelease()
{
    FMOD_OS_CriticalSection_Enter(mCrit);
    FMOD_OS_CriticalSection_Leave(mCrit);

    FMOD_OS_CriticalSection_Enter(gAsyncCrit);
    removeNode();
    FMOD_OS_CriticalSection_Leave(gAsyncCrit);

    mThreadActive = false;
    mThread.closeThread();

    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit);
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::update()
{
    if (gAsyncCrit)
    {
        LinkedListNode * current;

        FMOD_OS_CriticalSection_Enter(gAsyncCrit);

        current = gAsyncHead.getNext();
        while (current != &gAsyncHead)
        {
            LinkedListNode * next = current->getNext();

            if (((AsyncThread *)current)->mDone)
            {
                ((AsyncThread *)current)->reallyRelease();
            }
            current = next;
        }

        FMOD_OS_CriticalSection_Leave(gAsyncCrit);
    }

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::threadFunc()
{
    FMOD_RESULT result = FMOD_OK;
    SoundI * sound = 0;

    if (mThreadActive)
    {
        LinkedListNode * current;

        FMOD_OS_CriticalSection_Enter(mCrit);

        current = mHead.getNext();
        if (current != &mHead)
        {
            sound = (SoundI *)current->getData();
            current->removeNode();
            mBusy = true;
        }

        FMOD_OS_CriticalSection_Leave(mCrit);

        if (sound)
        {
            SystemI * system = sound->mSystem;

            if (sound->mOpenState == FMOD_OPENSTATE_LOADING)
            {
                if (sound->mMode & FMOD_OPENMEMORY)
                {
                    result = system->createSoundInternal((const char *)sound->mAsyncNameData, sound->mMode, sound->mExInfoExists ? &sound->mExInfo : 0, &sound);
                }
                else
                {
                    result = system->createSoundInternal(sound->mName, sound->mMode, sound->mExInfoExists ? &sound->mExInfo : 0, &sound);
                }
            }

            sound->mAsyncThread = 0;
            sound->mAsyncResult = result;
            sound->mOpenState = result == FMOD_OK ? FMOD_OPENSTATE_READY : FMOD_OPENSTATE_ERROR;

            mBusy = false;

            if (sound->mExInfoExists && sound->mExInfo.nonblockcallback)
            {
                sound->mExInfo.nonblockcallback((FMOD_SOUND *)sound, result);
            }

            release();
        }
    }

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::getAsyncThread(SoundI * sound)
{
    LinkedListNode * current;
    bool found = false;
    AsyncThread * asyncthread = 0;

    FMOD_OS_CriticalSection_Enter(gAsyncCrit);

    current = gAsyncHead.getNext();
    if (current != &gAsyncHead)
    {
        asyncthread = (AsyncThread *)current;

        FMOD_OS_CriticalSection_Enter(asyncthread->mCrit);
        found = true;
        FMOD_OS_CriticalSection_Leave(asyncthread->mCrit);
    }

    FMOD_OS_CriticalSection_Leave(gAsyncCrit);

    if (!found)
    {
        FMOD_RESULT result;

        asyncthread = FMOD_Object_Alloc(AsyncThread);
        if (!asyncthread)
        {
            return FMOD_ERR_MEMORY;
        }

        result = asyncthread->init(false);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    sound->mAsyncThread = asyncthread;

    return FMOD_OK;
}

FMOD_RESULT AsyncThread::wakeupThread()
{
}

FMOD_RESULT AsyncThread::addCallback(FMOD_ASYNC_CALLBACK callback, AsyncThread * * asyncthread)
{
}

FMOD_RESULT AsyncThread::removeCallback(FMOD_ASYNC_CALLBACK callback)
{
}

} // namespace FMOD
