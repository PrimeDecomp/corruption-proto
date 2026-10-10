// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. G2MEAB layout (group D): sizeof 0x124
// (DSPDistortion mSize 0x12C keeps its own member at 0x124); history members from startBuffering 0x805F6E68.
// vtable 0x806EC948 overrides execute(float *) 0x805F69F0 and release 0x805F69AC.

#ifndef _FMOD_DSP_FILTER_H
#define _FMOD_DSP_FILTER_H

#include "fmod.h"
#include "fmod_dspi.h"

namespace FMOD {

class DSPFilter : public DSPI
{
public:
    float * mHistoryBuffer; // offset 0x118
    unsigned int mHistoryPosition; // offset 0x11C
    unsigned int mHistoryLength; // offset 0x120

    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT release(bool freethis);
    FMOD_RESULT startBuffering(unsigned int length);
    FMOD_RESULT getHistoryBuffer(float * * buffer, unsigned int * position, unsigned int * length);
    FMOD_RESULT stopBuffering();
};

} // namespace FMOD

#endif
