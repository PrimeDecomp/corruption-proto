/*
 * G2MEAB fmod_dsp_distortion.cpp translation-unit scaffold (NonMatching).
 * .text: 0x805F4538..0x805F4AB8 (14 native functions).
 * Descriptive inferred basename; descriptor/callback closure.
 * Complete native helper/callback inventory preserved in external research.
 * Implementation, declarations and data ownership remain unreconstructed.
 */

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_dsp_distortion.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspdistortion;
FMOD_DSP_PARAMETERDESC dspdistortion_param[1];

FMOD_DSP_DESCRIPTION_EX * DSPDistortion::getDescriptionEx()
{
}

FMOD_RESULT DSPDistortion::createInternal()
{
}

FMOD_RESULT DSPDistortion::releaseInternal()
{
}

FMOD_RESULT DSPDistortion::resetInternal()
{
}

FMOD_RESULT DSPDistortion::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPDistortion::setParameterInternal(int index, float value)
{
}

FMOD_RESULT DSPDistortion::getParameterInternal(int index, float * value, char * valuestr)
{
}

FMOD_RESULT DSPDistortion::createCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPDistortion::releaseCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPDistortion::resetCallback(FMOD_DSP_STATE * dsp)
{
}

FMOD_RESULT DSPDistortion::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
}

FMOD_RESULT DSPDistortion::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
}

FMOD_RESULT DSPDistortion::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
}

} // namespace FMOD
