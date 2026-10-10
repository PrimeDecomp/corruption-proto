// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_CODECPOOL_H
#define _FMOD_DSP_CODECPOOL_H

#include "fmod.h"
#include "fmod_dspi.h"

namespace FMOD {
    class DSPI;
    struct SystemI;
}

namespace FMOD {

struct DSPCodecPool
{
    SystemI * mSystem; // offset 0x0
    int mNumDSPCodecs; // offset 0x4
    DSPI * * mPool; // offset 0x8
    bool mAllocated[256]; // offset 0xC
    unsigned char * mReadBuffer; // offset 0x10C
    FMOD_RESULT init(FMOD_DSP_CATEGORY category, int resamplerpcmblocksize, int numdspcodecs);
    FMOD_RESULT close();
    FMOD_RESULT alloc(DSPI * * dspcodec);
    FMOD_RESULT areAnyFree();
};

} // namespace FMOD

#endif
