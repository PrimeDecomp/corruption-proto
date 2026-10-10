// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805FB6F0..0x805FC024 (12 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: Leading805FB6F0 initializes independent0x90 plugin descriptor8075409C, original plugin
// name, version10100, parameter metadata806ED578, class allocation size0x168, and callback pointers
// into this unit. Instance bodies close shared three parametric EQ parameters+124/+128/+12C;B7B4
// applies defaults,B85C resets stereo history,B888 processes biquad,BC3C computes coefficients with
// sin/cos/pow. Full callback closure is extracted into callback-evidence.json rather than inferred
// from filename alone. Preserve all retained helpers, callback thunks, raw-only natives and inline
// expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_dsp_parameq.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspparameq;
FMOD_DSP_PARAMETERDESC dspparameq_param[3];

FMOD_DSP_DESCRIPTION_EX * DSPParamEq::getDescriptionEx()
{
}

FMOD_RESULT DSPParamEq::resetInternal()
{
}

FMOD_RESULT DSPParamEq::createInternal()
{
}

FMOD_RESULT DSPParamEq::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPParamEq::setParameterInternal(int index, float value)
{
}

FMOD_RESULT DSPParamEq::getParameterInternal(int index, float * value, char * valuestr)
{
}

FMOD_RESULT DSPParamEq::createCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPParamEq::resetCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPParamEq::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPParamEq::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
}

FMOD_RESULT DSPParamEq::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
}

} // namespace FMOD
