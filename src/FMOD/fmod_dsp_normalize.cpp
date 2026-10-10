// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805FA884..0x805FAED8 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference (local names and control
// flow); this older build reads the members directly instead of caching them in locals.

#include "fmod_dsp_normalize.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspnormalize;

FMOD_DSP_PARAMETERDESC dspnormalize_param[3] =
{
    { 0.0f, 20000.0f, 5000.0f, "Fade in time", "seconds", "Time to ramp the silence to full in ms.  0.0 to 20000.0. Default = 5000.0." },
    { 0.0f, 1.0f, 0.1f, "Lowest volume", "", "Lower volume range threshold to ignore.  0.0 to 1.0.  Default = 0.1.  Raise higher to stop amplification of very quiet signals." },
    { 0.0f, 100000.0f, 20.0f, "Maximum amp", "x", "Maximum amplification allowed.  1.0 to 100000.0.  Default = 20.0.  1.0 = no amplifaction, higher values allow more boost." }
};

FMOD_DSP_DESCRIPTION_EX * DSPNormalize::getDescriptionEx()
{
    memset(&dspnormalize, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspnormalize.name, "FMOD Normalize");
    dspnormalize.version = 0x00010100;
    dspnormalize.create = DSPNormalize::createCallback;
    dspnormalize.release = DSPNormalize::releaseCallback;
    dspnormalize.reset = DSPNormalize::resetCallback;
    dspnormalize.read = DSPNormalize::readCallback;

    dspnormalize.numparameters = sizeof(dspnormalize_param) / sizeof(dspnormalize_param[0]);
    dspnormalize.paramdesc = dspnormalize_param;
    dspnormalize.setparameter = DSPNormalize::setParameterCallback;
    dspnormalize.getparameter = DSPNormalize::getParameterCallback;

    dspnormalize.mType = FMOD_DSP_TYPE_NORMALIZE;
    dspnormalize.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspnormalize.mSize = sizeof(DSPNormalize);

    return &dspnormalize;
}

FMOD_RESULT DSPNormalize::createInternal()
{
    FMOD_RESULT result;
    int count;

    init();

    mPeak = mUnk138 = 1.0f;

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::releaseInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::resetInternal()
{
    mPeak = mUnk138 = 1.0f;

    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count, count2;

    for (count = 0; count < length; count++)
    {
        float scale;

        mPeak -= mAttackSpeed;
        if (mPeak < mThreshold)
        {
            mPeak = mThreshold;
        }

        for (count2 = 0; count2 < inchannels; count2++)
        {
            float in = fabs(inbuffer[(count * inchannels) + count2]);

            if (in > mPeak)
            {
                mPeak = in;
            }
        }

        scale = 1.0f / mPeak;
        if (scale > mMaxAmp)
        {
            scale = mMaxAmp;
        }

        for (count2 = 0; count2 < inchannels; count2++)
        {
            outbuffer[(count * inchannels) + count2] = inbuffer[(count * inchannels) + count2] * scale;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::setParameterInternal(int index, float value)
{
    switch (index)
    {
        case 0:
        {
            mFadeTime = value;
            break;
        }
        case 1:
        {
            mThreshold = value;
            break;
        }
        case 2:
        {
            mMaxAmp = value;
            break;
        }
    }

    if (mFadeTime != 0.0f)
    {
        mAttackSpeed = 1.0f / ((mFadeTime * (float)mOutputRate) / 1000.0f);
    }
    else
    {
        mAttackSpeed = 1.0f;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mFadeTime;
            sprintf(valuestr, "%.02f", mFadeTime);
            break;
        }
        case 1:
        {
            *value = mThreshold;
            sprintf(valuestr, "%.02f", mThreshold);
            break;
        }
        case 2:
        {
            *value = mMaxAmp;
            sprintf(valuestr, "%.02f", mMaxAmp);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPNormalize::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->createInternal();
}

FMOD_RESULT DSPNormalize::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->releaseInternal();
}

FMOD_RESULT DSPNormalize::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->resetInternal();
}

FMOD_RESULT DSPNormalize::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPNormalize::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->setParameterInternal(index, value);
}

FMOD_RESULT DSPNormalize::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPNormalize * normalize = (DSPNormalize *)dsp;

    return normalize->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
