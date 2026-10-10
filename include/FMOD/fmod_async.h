// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    Thread mThread; // offset 0xC
    bool mThreadActive; // offset 0x130
    LinkedListNode mHead; // offset 0x134
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x140
    bool mOwned; // offset 0x144
    bool mBusy; // offset 0x145
    bool mDone; // offset 0x146
    LinkedListNode mCallbackHead; // offset 0x148
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
