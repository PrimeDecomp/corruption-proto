// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_LOWPASS_H
#define _FMOD_DSP_LOWPASS_H

#include "fmod.h"
#include "fmod_dspi.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int LOWPASS_FILTER_SECTIONS = 2;
// Synthesized from the definitions in fmod_dsp_lowpass.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct DSPLowPass : public DSPI
{
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT prewarp(float * a0, float * a1, float * a2, float fc, float fs);
    FMOD_RESULT bilinear(float a0, float a1, float a2, float b0, float b1, float b2, float * k, float fs, float * coef);
    FMOD_RESULT szxform(float * a0, float * a1, float * a2, float * b0, float * b1, float * b2, float fc, float fs, float * k, float * coef);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
