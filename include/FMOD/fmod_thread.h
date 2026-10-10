// G2MEAB Thread (sizeof 0x124): the 4.06 members with MWCC's trailing vptr (+0x120). Evidence: ctor
// 0x806212BC, initThread 0x806212F0, closeThread 0x80621424, callback 0x80621208.

#ifndef _FMOD_THREAD_H
#define _FMOD_THREAD_H

#include "fmod.h"
#include "fmod_os_misc.h"

namespace FMOD {
    class Thread;
}

namespace FMOD {

class Thread
{
    char mName[256]; // offset 0x0
    void * mHandle; // offset 0x100
    bool mRunning; // offset 0x104
    void * mUserData; // offset 0x108
    void * mStack; // offset 0x10C
    FMOD_OS_SEMAPHORE * mSema; // offset 0x110
    FMOD_OS_SEMAPHORE * mEndSema; // offset 0x114
    int mPeriod; // offset 0x118
    void (* mUserCallback)(void *); // offset 0x11C
public:
    enum PRIORITY {
        PRIORITY_VERYLOW = -2,
        PRIORITY_LOW = -1,
        PRIORITY_NORMAL = 0,
        PRIORITY_HIGH = 1,
        PRIORITY_VERYHIGH = 2,
        PRIORITY_CRITICAL = 3
    };
    // G2MEAB: the OS thread entry returns 0 (0x8062129C).
    static void * callback(void * data);
    // vptr offset 0x120; the vtable 0x806EF798 holds only threadFunc (no virtual destructor).
    virtual FMOD_RESULT threadFunc();
    Thread();
    FMOD_RESULT initThread(const char * name, void (* func)(void *), void * userdata, PRIORITY priority, void * stack, int stacksize, bool usesemaphore, int sleepperiod);
    FMOD_RESULT closeThread();
    FMOD_RESULT getCurrentThreadID(unsigned int * id);
    FMOD_RESULT wakeupThread(bool frominterrupt);
};

} // namespace FMOD

#endif
