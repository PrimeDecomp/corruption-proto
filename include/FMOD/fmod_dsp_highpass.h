// G2MEAB highpass effect (RBJ biquad). The 4.06 DWARF has no type entry for the class; the members follow
// the G2MEAB accesses and the 4.06 readInternal/setParameterInternal locals.

#ifndef _FMOD_DSP_HIGHPASS_H
#define _FMOD_DSP_HIGHPASS_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Size 0x164 (getDescriptionEx 0x805F7D24). History cleared by resetInternal 0x805F7E80 (two channels),
// coefficients written by setParameterInternal 0x805F8384.
class DSPHighPass : public DSPFilter
{
    float mCutoff; // offset 0x124, Guessed name (param 0, hz)
    float mResonance; // offset 0x128, Guessed name (param 1, Q)
    float mIn1[2]; // offset 0x12C, Guessed name
    float mIn2[2]; // offset 0x134, Guessed name
    float mOut1[2]; // offset 0x13C, Guessed name
    float mOut2[2]; // offset 0x144, Guessed name
    float mA0; // offset 0x14C, Guessed name (4.06 readInternal local "a0")
    float mA1; // offset 0x150, Guessed name
    float mA2; // offset 0x154, Guessed name
    float mB0; // offset 0x158, Guessed name
    float mB1; // offset 0x15C, Guessed name
    float mB2; // offset 0x160, Guessed name

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
