// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    char mName[256]; // offset 0x4
    void * mHandle; // offset 0x104
    bool mRunning; // offset 0x108
    void * mUserData; // offset 0x10C
    void * mStack; // offset 0x110
    FMOD_OS_SEMAPHORE * mSema; // offset 0x114
    FMOD_OS_SEMAPHORE * mEndSema; // offset 0x118
    int mPeriod; // offset 0x11C
    void (* mUserCallback)(void *); // offset 0x120
public:
    enum PRIORITY {
        PRIORITY_VERYLOW = -2,
        PRIORITY_LOW = -1,
        PRIORITY_NORMAL = 0,
        PRIORITY_HIGH = 1,
        PRIORITY_VERYHIGH = 2,
        PRIORITY_CRITICAL = 3
    };
    virtual ~Thread() {}
    static void callback(void * data);
    virtual FMOD_RESULT threadFunc();
    Thread();
    FMOD_RESULT initThread(const char * name, void (* func)(void *), void * userdata, PRIORITY priority, void * stack, int stacksize, bool usesemaphore, int sleepperiod);
    FMOD_RESULT closeThread();
    FMOD_RESULT getCurrentThreadID(unsigned int * id);
    FMOD_RESULT wakeupThread(bool frominterrupt);
};

} // namespace FMOD

#endif
