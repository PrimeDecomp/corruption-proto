// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_DSP_DISTORTION_H
#define _FMOD_DSP_DISTORTION_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// The 4.06 DWARF has no type entry. G2MEAB: getDescriptionEx (0x805F4538) stores mSize 0x12C;
// mLevel at 0x124 is read/written by readInternal/setParameterInternal (0x805F46A8, 0x805F48E8).
// The 4.06 object places the level directly after DSPFilter (0x114 of 0x11C), so the base is DSPFilter.
class DSPDistortion : public DSPFilter
{
    float mLevel; // offset 0x124, Guessed name
    unsigned int mUnk128; // offset 0x128, no G2MEAB access; completes the 0x12C size

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
