// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805F9E1C..0x805FA884 (13 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: Leading805F9E1C initializes independent0x90 plugin descriptor80753EBC, original plugin
// name, version10100, parameter metadata806ED188, class allocation size0x1A4, and callback pointers
// into this unit. Instance bodies close shared two-section biquad state+12C..+1A0;9ED4 initializes
// both sections,9FAC processes samples,three retained coefficient helpersA32C/A3D8/A4A4 support
// setterA564. Full callback closure is extracted into callback-evidence.json rather than inferred
// from filename alone. Preserve all retained helpers, callback thunks, raw-only natives and inline
// expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_dsp_lowpass.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsplowpass;
FMOD_DSP_PARAMETERDESC dsplowpass_param[2];

FMOD_DSP_DESCRIPTION_EX * DSPLowPass::getDescriptionEx()
{
}

FMOD_RESULT DSPLowPass::createInternal()
{
}

FMOD_RESULT DSPLowPass::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPLowPass::prewarp(float * a0, float * a1, float * a2, float fc, float fs)
{
}

FMOD_RESULT DSPLowPass::bilinear(float a0, float a1, float a2, float b0, float b1, float b2, float * k, float fs, float * coef)
{
}

FMOD_RESULT DSPLowPass::szxform(float * a0, float * a1, float * a2, float * b0, float * b1, float * b2, float fc, float fs, float * k, float * coef)
{
}

FMOD_RESULT DSPLowPass::setParameterInternal(int index, float value)
{
}

FMOD_RESULT DSPLowPass::getParameterInternal(int index, float * value, char * valuestr)
{
}

FMOD_RESULT DSPLowPass::createCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPLowPass::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPLowPass::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
}

FMOD_RESULT DSPLowPass::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
}

} // namespace FMOD
