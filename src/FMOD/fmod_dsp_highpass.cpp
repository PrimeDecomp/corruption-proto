// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805F7D24..0x805F8734 (12 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: Leading805F7D24 initializes independent0x90 plugin descriptor80753CDC, original plugin
// name, version10100, parameter metadata806ECBC8, class allocation size0x164, and callback pointers
// into this unit. Instance bodies close shared biquad coefficients and stereo history+12C..+148;
// init7DE8 applies descriptor defaults and reset7E80 clears history,7EAC processes samples,8384
// recalculates coefficients. Full callback closure is extracted into callback-evidence.json rather
// than inferred from filename alone. Preserve all retained helpers, callback thunks, raw-only
// natives and inline expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_dsp_highpass.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsphighpass;
FMOD_DSP_PARAMETERDESC dsphighpass_param[2];

FMOD_DSP_DESCRIPTION_EX * DSPHighPass::getDescriptionEx()
{
}

FMOD_RESULT DSPHighPass::resetInternal()
{
}

FMOD_RESULT DSPHighPass::createInternal()
{
}

FMOD_RESULT DSPHighPass::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPHighPass::setParameterInternal(int index, float value)
{
}

FMOD_RESULT DSPHighPass::getParameterInternal(int index, float * value, char * valuestr)
{
}

FMOD_RESULT DSPHighPass::createCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPHighPass::resetCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPHighPass::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPHighPass::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
}

FMOD_RESULT DSPHighPass::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
}

} // namespace FMOD
