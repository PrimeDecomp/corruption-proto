// G2MEAB DSPCodecPool (group D): sizeof 0x10. SystemI embeds the MPEG pool at +0xFA4 and the ADPCM pool at
// +0xFB4 (SystemI 0x8061E564 dispatches on format 10/7); there is no RAW pool. init 0x80628C88 takes a
// template Codec and clones it into every pool DSPCodec (the 4.06 category/blocksize arguments are absent).
// The per-unit allocated flag lives in DSPI +0x114 (alloc 0x806292A4) instead of a 4.06 mAllocated array.

#ifndef _FMOD_DSP_CODECPOOL_H
#define _FMOD_DSP_CODECPOOL_H

#include "fmod.h"
#include "fmod_dspi.h"

namespace FMOD {
    struct Codec;
    class DSPI;
    struct SystemI;
}

namespace FMOD {

struct DSPCodecPool
{
    SystemI * mSystem; // offset 0x0
    int mNumDSPCodecs; // offset 0x4, init 0x80628CE0
    int mUnk8; // offset 0x8, incremented by alloc 0x806292D4
    DSPI * * mPool; // offset 0xC, init 0x80628CFC (calloc line 0x2E)

    FMOD_RESULT init(Codec * codec, int numdspcodecs);
    FMOD_RESULT close();
    FMOD_RESULT alloc(DSPI * * dspcodec);
};

} // namespace FMOD

#endif
