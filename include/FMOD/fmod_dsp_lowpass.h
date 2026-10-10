// G2MEAB resonant lowpass effect (two cascaded biquad sections designed by bilinear transform).
// The 4.06 DWARF has no type entry for the class; the members follow the G2MEAB accesses and the
// 4.06 filter/setParameterInternal locals.

#ifndef _FMOD_DSP_LOWPASS_H
#define _FMOD_DSP_LOWPASS_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int LOWPASS_FILTER_SECTIONS = 2;

// Guessed name. s-domain prototype coefficients of one filter section (createInternal 0x805F9ED4).
struct DSPLowPassBiquad
{
    float a0; // offset 0x0
    float a1; // offset 0x4
    float a2; // offset 0x8
    float b0; // offset 0xC
    float b1; // offset 0x10
    float b2; // offset 0x14
};

// Size 0x1A4 (getDescriptionEx 0x805F9E1C). Section count, history and coefficient offsets from
// readInternal 0x805F9FAC and setParameterInternal 0x805FA564.
class DSPLowPass : public DSPFilter
{
    float mResonance; // offset 0x124, Guessed name (param 1)
    float mCutoff; // offset 0x128, Guessed name (param 0, hz)
    unsigned int mNumSections; // offset 0x12C, Guessed name (LOWPASS_FILTER_SECTIONS)
    float mHistory[2 * 2 * LOWPASS_FILTER_SECTIONS]; // offset 0x130, Guessed name (2 per section, 2 channels)
    float mCoefficients[1 + 4 * LOWPASS_FILTER_SECTIONS]; // offset 0x150, Guessed name (gain, then 4 per section)
    DSPLowPassBiquad mProtoCoef[LOWPASS_FILTER_SECTIONS]; // offset 0x174, Guessed name

    // Inlined into every readInternal branch (0x805F9FD4..0x805FA2F0); 4.06 declares it at
    // fmod_dsp_lowpass.cpp:164 with a static dc offset.
    inline float filter(float input, int channel);

public:
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
