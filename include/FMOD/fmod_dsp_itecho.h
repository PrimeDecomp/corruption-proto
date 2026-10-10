// G2MEAB IT echo effect. The 4.06 PS3 library ships no itecho debug information, so the class and method
// names follow the DSP<Type> / xxxInternal / xxxCallback pattern of the referenced effects (Guessed names).

#ifndef _FMOD_DSP_ITECHO_H
#define _FMOD_DSP_ITECHO_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name. Size 0x160 (getDescriptionEx 0x805F8734). Parameter offsets from setParameterInternal
// 0x805F8CA8, per-channel buffers (left, right) from releaseInternal 0x805F8894 and resetInternal 0x805F8918.
class DSPITEcho : public DSPFilter
{
    float mWetDryMix; // offset 0x124, Guessed name (param 0 / 100)
    float mFeedback; // offset 0x128, Guessed name (param 1 / 100)
    float mDelay[2]; // offset 0x12C, Guessed name (params 2 and 3, ms)
    bool mPanDelay; // offset 0x134, Guessed name (param 4; stored but not used by readInternal)
    float * mEchoBuffer[2]; // offset 0x138, Guessed name
    unsigned int mEchoBufferLengthBytes[2]; // offset 0x140, Guessed name
    unsigned int mEchoPosition[2]; // offset 0x148, Guessed name
    unsigned int mEchoLength[2]; // offset 0x150, Guessed name
    unsigned int mUnk158; // offset 0x158 (no access in this unit)
    int mOutputRate; // offset 0x15C, Guessed name (getSoftwareFormat samplerate)

public:
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT releaseCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT resetCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
