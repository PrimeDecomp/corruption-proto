// G2MEAB chorus effect. The 4.06 PS3 library ships no chorus object, so the class and method names follow
// the DSP<Type> / xxxInternal / xxxCallback pattern of the referenced effects (Guessed names).

#ifndef _FMOD_DSP_CHORUS_H
#define _FMOD_DSP_CHORUS_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int DSPCHORUS_COSTABLESIZE = 8192; // Guessed name; createInternal 0x805F04E4 loop bound
const int DSPCHORUS_COSTABLEMASK = 0x7FFF; // Guessed name; loaded from .sdata2 0x807A2EB8 by readInternal

// Guessed name. Size 0x816C (getDescriptionEx 0x805F0414). Parameter offsets from the setParameterInternal
// jump table (0x805F0F48), buffer fields from createInternal/releaseInternal/resetInternal.
class DSPChorus : public DSPFilter
{
    float mDepth; // offset 0x124, Guessed name (param 6)
    float mDryMix; // offset 0x128, Guessed name (param 0)
    float mWetMix1; // offset 0x12C, Guessed name (param 1)
    float mWetMix2; // offset 0x130, Guessed name (param 2)
    float mWetMix3; // offset 0x134, Guessed name (param 3)
    float mFeedback; // offset 0x138, Guessed name (param 7)
    float mDelay; // offset 0x13C, Guessed name (param 4, ms)
    float mRate; // offset 0x140, Guessed name (param 5, hz)
    float * mChorusBuffer; // offset 0x144, Guessed name
    unsigned int mChorusBufferLength; // offset 0x148, Guessed name (delay in samples * 2, at least 4)
    unsigned int mChorusBufferLengthBytes; // offset 0x14C, Guessed name
    unsigned int mChorusBufferPosition; // offset 0x150, Guessed name (cleared by resetInternal)
    float mTapPosition1; // offset 0x154, Guessed name (delay of tap 1 in samples)
    float mTapPosition2; // offset 0x158, Guessed name
    float mTapPosition3; // offset 0x15C, Guessed name
    float mChorusTick; // offset 0x160, Guessed name (cleared by createInternal)
    float mRateHz; // offset 0x164, Guessed name (mRate / mOutputRate)
    int mOutputRate; // offset 0x168, Guessed name (getSoftwareFormat samplerate)
    float mCosTab[DSPCHORUS_COSTABLESIZE]; // offset 0x16C, Guessed name (quarter cosine wave)

    // Guessed name. Inlined three times into readInternal (0x805F0BDC..0x805F0E18): a sine read from the
    // quarter-wave cosine table, 32768 steps per cycle.
    inline float sine(float x)
    {
        int index = (int)((x - 0.25f) * 32768.0f);

        if (index < 0)
        {
            index = -index;
        }
        index &= DSPCHORUS_COSTABLEMASK;

        switch (index >> 13)
        {
            case 0:
            {
                return mCosTab[index];
            }
            case 1:
            {
                return -mCosTab[(DSPCHORUS_COSTABLESIZE - 1) - (index - 8192)];
            }
            case 2:
            {
                return -mCosTab[index - 16384];
            }
            case 3:
            {
                return mCosTab[(DSPCHORUS_COSTABLESIZE - 1) - (index - 24576)];
            }
        }

        return 0.0f;
    }

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
