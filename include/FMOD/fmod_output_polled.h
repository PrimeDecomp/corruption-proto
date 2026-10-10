// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OUTPUT_POLLED_H
#define _FMOD_OUTPUT_POLLED_H

#include "fmod.h"
#include "fmod_os_misc.h"
#include "fmod_outputi.h"
#include "fmod_thread.h"

namespace FMOD {
    class OutputPolled;
}

namespace FMOD {

// G2MEAB layout (0x204): Output 0x0..0xD4, Thread 0xD4..0x1F8 (its vptr at +0x1F4). Evidence: the constructor
// 0x8060F80C (Thread ctor on +0xD4, vtables at +0x10/+0x1F4, mFillBlock = 0), threadFunc 0x8060F860
// (+0x1F8 block counter, +0x200 semaphore), start 0x8060FEA4 (+0x1FC test), stop 0x80610028.
class OutputPolled : public Output, public Thread
{
    int mFillBlock; // offset 0x1F8
protected:
    bool mPolledFromMainThread; // offset 0x1FC
    FMOD_OS_SEMAPHORE * mFinishedSema; // offset 0x200
public:
    OutputPolled();
    virtual FMOD_RESULT threadFunc();
    FMOD_RESULT start();
    FMOD_RESULT stop();
};

} // namespace FMOD

#endif
