// G2MEAB IT (Impulse Tracker) resonant low-pass effect. The 4.06 PS3 library ships no itlowpass debug
// information, so the class and method names follow the DSP<Type> / xxxInternal / xxxCallback pattern
// of the referenced effects (Guessed names).

#ifndef _FMOD_DSP_ITLOWPASS_H
#define _FMOD_DSP_ITLOWPASS_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name. Size 0x148 (getDescriptionEx 0x805F9180). Parameter offsets from setParameterInternal
// 0x805F9AE8, history layout from resetInternal 0x805F92DC and readInternal 0x805F92F8.
class DSPITLowPass : public DSPFilter
{
    float mResonance; // offset 0x124, Guessed name (param 1)
    float mCutoff; // offset 0x128, Guessed name (param 0, hz)
    float mHistory[2][2]; // offset 0x12C, Guessed name ([channel][0] = y1, [channel][1] = y2)
    float mCoefA; // offset 0x13C, Guessed name (input gain)
    float mCoefB; // offset 0x140, Guessed name (y1 feedback)
    float mCoefC; // offset 0x144, Guessed name (y2 feedback)

public:
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT resetCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
