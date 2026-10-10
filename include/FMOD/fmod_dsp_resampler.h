// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_RESAMPLER_H
#define _FMOD_DSP_RESAMPLER_H

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_types.h"

namespace FMOD {
    struct DSPResampler;
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int FMOD_DSP_RESAMPLER_OVERFLOWLENGTH = 4;
enum FMOD_RESAMPLER_END {
    FMOD_RESAMPLER_END_MIXBUFFER = 0,
    FMOD_RESAMPLER_END_RESAMPLEBUFFER = 1,
    FMOD_RESAMPLER_END_SOUND = 2
};

struct DSPResampler : public DSPI
{
    FMOD_UINT64P mPosition; // offset 0x108
    FMOD_SINT64P mSpeed; // offset 0x110
    float mFrequency; // offset 0x118
    int mTargetFrequency; // offset 0x11C
    FMOD_UINT64P mResamplePosition; // offset 0x120
    void * mResampleBufferMemory; // offset 0x128
    void * mResampleBuffer; // offset 0x12C
    int mResampleBufferChannels; // offset 0x130
    unsigned int mResampleBlockLength; // offset 0x134
    unsigned int mResampleBufferLength; // offset 0x138
    unsigned int mResampleBufferPos; // offset 0x13C
    unsigned int mResampleFinishPos; // offset 0x140
    unsigned int mOverflowLength; // offset 0x144
    unsigned int mReadPosition; // offset 0x148
    int mFill; // offset 0x14C
    unsigned int mLength; // offset 0x150
    unsigned int mLoopStart; // offset 0x154
    unsigned int mLoopLength; // offset 0x158
    int mLoopCount; // offset 0x15C
    FMOD_MODE mMode; // offset 0x160
    DSPResampler();
    virtual FMOD_RESULT release(bool freethis);
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    FMOD_RESULT update(unsigned int length, int * outchannels, void * * outbuffer);
    FMOD_RESULT setFrequency(float frequency);
    FMOD_RESULT getFrequency(float * frequency);
    virtual FMOD_RESULT setPosition(unsigned int position);
};

} // namespace FMOD

#endif
