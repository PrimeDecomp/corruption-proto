// G2MEAB DSPSoundCard (group D): sizeof 0x128, vtable 0x806EDD50 (implicit deleting dtor 0x80606328).
// mConversionBuffer at 0x124 is allocated by alloc 0x80606150, freed by release 0x806061FC and used by
// execute 0x80606264 (DSPI slot 0x18). The Gormiti mConversionBufferMem member is absent here.

#ifndef _FMOD_DSP_SOUNDCARD_H
#define _FMOD_DSP_SOUNDCARD_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

class DSPSoundCard : public DSPFilter
{
public:
    float * mConversionBuffer; // offset 0x124

    // DSPI overrides
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT release(bool freethis);
};

} // namespace FMOD

#endif
