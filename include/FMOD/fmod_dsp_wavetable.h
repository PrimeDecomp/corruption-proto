// G2MEAB DSPWaveTable (group D): the category-3 unit, derives from DSPI (sizeof 0x118), sizeof 0x138
// (PluginFactory::createDSP 0x806116F0 minimum size). Vtable 0x806EDDF0 overrides only alloc 0x80606428,
// execute 0x806064CC and addInput 0x8060649C (deleting dtor 0x80606B60). The 4.06 members keep their order,
// shifted by +0x10: setPositionInternal 0x806069E4 stores mPosition 0x118/0x11C, setFrequency 0x80606A2C
// stores mDirection 0x128, mFrequency 0x12C and mSpeed 0x120, and execute reads mChannel 0x130 / mSound 0x134.
// The 4.06 resampleLinear, readInternal/readCallback and getFrequency are absent from G2MEAB.

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
    FMOD_UINT64P mPosition; // offset 0x118
    FMOD_SINT64P mSpeed; // offset 0x120
    DSPWAVETABLE_SPEEDDIR mDirection; // offset 0x128
    float mFrequency; // offset 0x12C
    ChannelSoftware * mChannel; // offset 0x130
    SoundI * mSound; // offset 0x134

    // DSPI overrides
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT addInput(DSPI * target);

    FMOD_RESULT setPositionInternal(unsigned int position);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    FMOD_RESULT setFrequency(float frequency);

    static FMOD_RESULT setPositionCallback(FMOD_DSP_STATE * dsp, unsigned int position);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
