// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_WAVETABLE_H
#define _FMOD_DSP_WAVETABLE_H

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_types.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    class ChannelSoftware;
    class DSPI;
    struct FMOD_DSP_DESCRIPTION_EX;
    struct SoundI;
}

namespace FMOD {

enum DSPWAVETABLE_SPEEDDIR {
    DSPWAVETABLE_SPEEDDIR_FORWARDS = 0,
    DSPWAVETABLE_SPEEDDIR_BACKWARDS = 1
};

struct DSPWaveTable : public DSPI
{
    FMOD_UINT64P mPosition; // offset 0x108
    FMOD_SINT64P mSpeed; // offset 0x110
    DSPWAVETABLE_SPEEDDIR mDirection; // offset 0x118
    float mFrequency; // offset 0x11C
    ChannelSoftware * mChannel; // offset 0x120
    SoundI * mSound; // offset 0x124
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT resampleLinear(float *, void *, int, FMOD_SOUND_FORMAT, int);
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    virtual FMOD_RESULT addInput(DSPI * target);
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setPositionInternal(unsigned int position);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    FMOD_RESULT setFrequency(float frequency);
    FMOD_RESULT getFrequency(float * frequency);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
    static FMOD_RESULT setPositionCallback(FMOD_DSP_STATE * dsp, unsigned int position);
};

} // namespace FMOD

#endif
