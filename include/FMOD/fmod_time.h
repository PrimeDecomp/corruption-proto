// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_TIME_H
#define _FMOD_TIME_H

#include "fmod.h"
#include "fmod_types.h"

namespace FMOD {
    struct TimeStamp;
}

namespace FMOD {

struct TimeStamp
{
    unsigned int mIn; // offset 0x0
    unsigned int mOut; // offset 0x4
    unsigned int mPausedIn; // offset 0x8
    unsigned int mPausedOut; // offset 0xC
    unsigned int mTotalIn; // offset 0x10
    unsigned int mTotalOut; // offset 0x14
    FMOD_UFLOAT mPercent; // offset 0x18
    FMOD_UFLOAT mAvTotal; // offset 0x1C
    FMOD_UFLOAT mAvCurrent; // offset 0x20
    FMOD_UFLOAT mCPUUsage; // offset 0x24
    bool mPaused; // offset 0x28
    unsigned int mPausedTotal; // offset 0x2C
    int mPausedRefCount; // offset 0x30
    bool mTiming; // offset 0x34
    TimeStamp();
    FMOD_RESULT stampIn();
    FMOD_RESULT stampOut(int damppercentage);
    FMOD_RESULT getCPUUsage(FMOD_UFLOAT * cpuusage);
    FMOD_RESULT setPaused(bool paused);
};

} // namespace FMOD

FMOD_RESULT FMOD_Time_Get(unsigned int * ms);
FMOD_RESULT FMOD_Time_Sleep(unsigned int sleeptime);

#endif
