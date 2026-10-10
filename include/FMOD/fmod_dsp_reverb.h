// G2MEAB Freeverb-based reverb effect. The 4.06 PS3 library ships no reverb object, so the class and method
// names follow the DSP<Type> / xxxInternal / xxxCallback pattern of the referenced effects (Guessed names).

#ifndef _FMOD_DSP_REVERB_H
#define _FMOD_DSP_REVERB_H

#include "fmod.h"
#include "fmod_dsp_filter.h"
#include "revmodel.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name. Size 0x19138 (getDescriptionEx 0x80605958) = 0x124 + sizeof(revmodel) 0x19014; createInternal
// (0x80605A30) constructs the revmodel in place at 0x124 and every parameter forwards to it.
class DSPReverb : public DSPFilter
{
    revmodel mReverb; // offset 0x124, Guessed name

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
