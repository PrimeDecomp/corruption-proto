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

class OutputPolled : public Output, public Thread
{
    int mFillBlock; // offset 0x1E4
protected:
    bool mPolledFromMainThread; // offset 0x1E8
    FMOD_OS_SEMAPHORE * mFinishedSema; // offset 0x1EC
public:
    OutputPolled();
    virtual FMOD_RESULT threadFunc();
    FMOD_RESULT start();
    FMOD_RESULT stop();
};

} // namespace FMOD

#endif
