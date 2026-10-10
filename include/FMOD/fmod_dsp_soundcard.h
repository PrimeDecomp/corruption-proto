// Reconstructed from a later FMOD Ex (Gormiti, Wii/MWCC) debug information. Member layout and offsets are the Gormiti reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_SOUNDCARD_H
#define _FMOD_DSP_SOUNDCARD_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

namespace FMOD {
    class DSPFilter;
    struct DSPSoundCard;
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

struct DSPSoundCard : public DSPFilter
{
    FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    FMOD_RESULT release(bool freethis);
    FMOD_RESULT execute(void * * outbuffer, unsigned int * length, int * outchannels, unsigned int tick);
    float * mConversionBuffer; // offset 0x118
    float * mConversionBufferMem; // offset 0x11C
};

} // namespace FMOD

#endif
