// G2MEAB oscillator (tone generator) effect. The 4.06 PS3 library ships no oscillator debug information,
// so the class and method names follow the DSP<Type> / xxxInternal / xxxCallback pattern of the
// referenced effects (Guessed names).

#ifndef _FMOD_DSP_OSCILLATOR_H
#define _FMOD_DSP_OSCILLATOR_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name. Size 0x138 (getDescriptionEx 0x805FAED8). Fields from readInternal 0x805FB038,
// setParameterInternal 0x805FB3B4 and getParameterInternal 0x805FB454.
class DSPOscillator : public DSPFilter
{
    float mRate; // offset 0x124, Guessed name (frequency / output rate, position step per sample)
    int mType; // offset 0x128, Guessed name (param 0: 0 sine, 1 square, 2 saw up, 3 saw down, 4 triangle, 5 noise)
    float mFrequency; // offset 0x12C, Guessed name (param 1, hz)
    int mDirection; // offset 0x130, Guessed name (+1/-1 for square and triangle)
    float mPosition; // offset 0x134, Guessed name (phase)

public:
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT releaseCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
