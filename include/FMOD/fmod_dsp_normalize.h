// G2MEAB normalize effect. The 4.06 DWARF has no type entry for the class; the members follow the
// G2MEAB accesses (offsets 0x10 above the 4.06 -O0 object, whose base ends at 0x114).

#ifndef _FMOD_DSP_NORMALIZE_H
#define _FMOD_DSP_NORMALIZE_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Size 0x140 (getDescriptionEx 0x805FA884). Parameter offsets from setParameterInternal 0x805FAC04.
class DSPNormalize : public DSPFilter
{
    float mThreshold; // offset 0x124, Guessed name (param 1, 4.06 readInternal local "threshold")
    float mMaxAmp; // offset 0x128, Guessed name (param 2, 4.06 readInternal local "maxamp")
    float mFadeTime; // offset 0x12C, Guessed name (param 0, ms)
    float mAttackSpeed; // offset 0x130, Guessed name (4.06 readInternal local "attackspeed")
    float mPeak; // offset 0x134, Guessed name (decaying peak envelope)
    float mUnk138; // offset 0x138 (set to 1.0 by resetInternal 0x805FAA24, otherwise unused)
    int mOutputRate; // offset 0x13C, Guessed name (getSoftwareFormat samplerate)

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
