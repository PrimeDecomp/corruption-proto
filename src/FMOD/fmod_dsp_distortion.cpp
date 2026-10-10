// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F4538..0x805F4AB8 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; DSPDistortion layout from G2MEAB accesses.

#include "fmod_dsp_distortion.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspdistortion;

FMOD_DSP_PARAMETERDESC dspdistortion_param[1] =
{
    { 0.0f, 1.0f, 0.5f, "Level", "", "Distortion value.  0.0 to 1.0.  Default = 0.5." }
};

FMOD_DSP_DESCRIPTION_EX * DSPDistortion::getDescriptionEx()
{
    memset(&dspdistortion, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspdistortion.name, "FMOD Distortion");
    dspdistortion.version = 0x00010100;
    dspdistortion.create = DSPDistortion::createCallback;
    dspdistortion.release = DSPDistortion::releaseCallback;
    dspdistortion.reset = DSPDistortion::resetCallback;
    dspdistortion.read = DSPDistortion::readCallback;

    dspdistortion.numparameters = sizeof(dspdistortion_param) / sizeof(dspdistortion_param[0]);
    dspdistortion.paramdesc = dspdistortion_param;
    dspdistortion.setparameter = DSPDistortion::setParameterCallback;
    dspdistortion.getparameter = DSPDistortion::getParameterCallback;

    dspdistortion.mType = FMOD_DSP_TYPE_DISTORTION;
    dspdistortion.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspdistortion.mSize = sizeof(DSPDistortion);

    return &dspdistortion;
}

FMOD_RESULT DSPDistortion::createInternal()
{
    int count;

    init();

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::releaseInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::resetInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    float k;
    unsigned int len;

    if (mLevel < 1.0f)
    {
        k = 2.0f * mLevel / (1.0f - mLevel);
    }
    else
    {
        k = 2.0f * 0.9999f / (1.0f - 0.9999f);
    }

    len = (length * inchannels) >> 3;
    while (len)
    {
        outbuffer[0] = (1.0f + k) * inbuffer[0] / (1.0f + k * fabsf(inbuffer[0]));
        outbuffer[1] = (1.0f + k) * inbuffer[1] / (1.0f + k * fabsf(inbuffer[1]));
        outbuffer[2] = (1.0f + k) * inbuffer[2] / (1.0f + k * fabsf(inbuffer[2]));
        outbuffer[3] = (1.0f + k) * inbuffer[3] / (1.0f + k * fabsf(inbuffer[3]));
        outbuffer[4] = (1.0f + k) * inbuffer[4] / (1.0f + k * fabsf(inbuffer[4]));
        outbuffer[5] = (1.0f + k) * inbuffer[5] / (1.0f + k * fabsf(inbuffer[5]));
        outbuffer[6] = (1.0f + k) * inbuffer[6] / (1.0f + k * fabsf(inbuffer[6]));
        outbuffer[7] = (1.0f + k) * inbuffer[7] / (1.0f + k * fabsf(inbuffer[7]));
        inbuffer += 8;
        outbuffer += 8;
        len--;
    }

    len = (length * inchannels) & 7;
    while (len)
    {
        outbuffer[0] = (1.0f + k) * inbuffer[0] / (1.0f + k * fabsf(inbuffer[0]));
        inbuffer++;
        outbuffer++;
        len--;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::setParameterInternal(int index, float value)
{
    mLevel = value;

    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::getParameterInternal(int index, float * value, char * valuestr)
{
    *value = mLevel;
    sprintf(valuestr, "%.02f", mLevel);

    return FMOD_OK;
}

FMOD_RESULT DSPDistortion::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->createInternal();
}

FMOD_RESULT DSPDistortion::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->releaseInternal();
}

FMOD_RESULT DSPDistortion::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->resetInternal();
}

FMOD_RESULT DSPDistortion::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPDistortion::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->setParameterInternal(index, value);
}

FMOD_RESULT DSPDistortion::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPDistortion * distortion = (DSPDistortion *)dsp;

    return distortion->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
