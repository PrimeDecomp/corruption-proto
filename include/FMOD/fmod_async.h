// G2MEAB AsyncThread (sizeof 0x158, vtable 0x806E25A8 with only the destructor). Evidence: ctor
// 0x805B60C0, init 0x805B6258, threadFunc 0x805B6468, allocation of 0x158 in getAsyncThread 0x805B65F8.
// There is no 4.06 callback list.

#ifndef _FMOD_ASYNC_H
#define _FMOD_ASYNC_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_thread.h"

namespace FMOD {
    class AsyncThread;
    class LinkedListNode;
    struct SoundI;
}

typedef FMOD_RESULT (* FMOD_ASYNC_CALLBACK)();
namespace FMOD {

class AsyncThread : public LinkedListNode
{
    Thread mThread; // offset 0x14
    bool mThreadActive; // offset 0x138
    LinkedListNode mHead; // offset 0x13C
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x150
    bool mOwned; // offset 0x154
    bool mBusy; // offset 0x155
    bool mDone; // offset 0x156
public:
    FMOD_RESULT threadFunc();
    FMOD_RESULT init(bool owned);
    FMOD_RESULT reallyRelease();
    static LinkedListNode gAsyncHead;
    static FMOD_OS_CRITICALSECTION * gAsyncCrit;
    AsyncThread();
    FMOD_RESULT release();
    FMOD_RESULT wakeupThread();
    static FMOD_RESULT getAsyncThread(SoundI * sound);
    static FMOD_RESULT update();
    static FMOD_RESULT shutDown();
    static FMOD_RESULT addCallback(FMOD_ASYNC_CALLBACK callback, AsyncThread * * asyncthread);
    static FMOD_RESULT removeCallback(FMOD_ASYNC_CALLBACK callback);
};

// 4.06 only, with 4.06 PS3 offsets (a 0x5C mExInfo). G2MEAB keeps these fields inline in SoundI
// (+0x2C4-+0x334); no separate AsyncData object has been observed.
struct AsyncData
{
    char mName[256]; // offset 0x0
    AsyncThread * mThread; // offset 0x100
    LinkedListNode mNode; // offset 0x104
    void * mNameData; // offset 0x110
    FMOD_CREATESOUNDEXINFO mExInfo; // offset 0x114
    bool mExInfoExists; // offset 0x170
    int mSubSound; // offset 0x174
    FMOD_RESULT mResult; // offset 0x178
};

void asyncThreadFunc(void * data);

} // namespace FMOD

#endif
