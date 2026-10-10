// G2MEAB flange effect. The 4.06 PS3 library ships no flange debug information, so the class and method
// names follow the DSP<Type> / xxxInternal / xxxCallback pattern of the referenced effects (Guessed names).

#ifndef _FMOD_DSP_FLANGE_H
#define _FMOD_DSP_FLANGE_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int DSPFLANGE_COSTABLESIZE = 8192; // Guessed name; createInternal 0x805F71A0 loop bound
const int DSPFLANGE_COSTABLEMASK = 0x7FFF; // Guessed name; loaded from .sdata2 0x807A2FE0 by readInternal

// Guessed name. Size 0x8154 (getDescriptionEx 0x805F70D0). Parameter offsets from the setParameterInternal
// switch (0x805F79A4), buffer fields from createInternal/releaseInternal/resetInternal.
class DSPFlange : public DSPFilter
{
    float mDepth; // offset 0x124, Guessed name (param 2)
    float mDryMix; // offset 0x128, Guessed name (param 0)
    float mWetMix; // offset 0x12C, Guessed name (param 1)
    float mRate; // offset 0x130, Guessed name (param 3, hz)
    float * mFlangeBuffer; // offset 0x134, Guessed name
    unsigned int mFlangeBufferLength; // offset 0x138, Guessed name (depth * 10 ms in samples, at least 4)
    unsigned int mFlangeBufferLengthBytes; // offset 0x13C, Guessed name
    unsigned int mFlangeBufferPosition; // offset 0x140, Guessed name (cleared by resetInternal)
    float mFlangeTapPosition; // offset 0x144, Guessed name (delay of the tap in samples)
    float mFlangeTick; // offset 0x148, Guessed name (cleared by createInternal)
    float mFlangeSpeed; // offset 0x14C, Guessed name (mRate / mOutputRate)
    int mOutputRate; // offset 0x150, Guessed name (getSoftwareFormat samplerate)
    float mCosTab[DSPFLANGE_COSTABLESIZE]; // offset 0x154, Guessed name (quarter cosine wave)

    // Guessed name. Inlined into readInternal (0x805F7850..0x805F7904): a sine read from the quarter-wave
    // cosine table, 32768 steps per cycle (same helper as DSPChorus).
    inline float sine(float x)
    {
        int index = (int)((x - 0.25f) * 32768.0f);

        if (index < 0)
        {
            index = -index;
        }
        index &= DSPFLANGE_COSTABLEMASK;

        switch (index >> 13)
        {
            case 0:
            {
                return mCosTab[index];
            }
            case 1:
            {
                return -mCosTab[(DSPFLANGE_COSTABLESIZE - 1) - (index - 8192)];
            }
            case 2:
            {
                return -mCosTab[index - 16384];
            }
            case 3:
            {
                return mCosTab[(DSPFLANGE_COSTABLESIZE - 1) - (index - 24576)];
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
