// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F70D0..0x805F7D24 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the flange; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects and the sibling chorus (single tap instead of three).

#include "fmod_dsp_flange.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspflange;

FMOD_DSP_PARAMETERDESC dspflange_param[4] =
{
    { 0.0f, 1.0f, 0.45f, "Drymix", "%", "Volume of original signal to pass to output.  0.0 to 1.0. Default = 0.45." },
    { 0.0f, 1.0f, 0.55f, "Wetmix", "%", "Volume of flange signal to pass to output.  0.0 to 1.0. Default = 0.55." },
    { 0.01f, 1.0f, 1.0f, "Depth", "", "Flange depth.  0.01 to 1.0.  Default = 1.0." },
    { 0.0f, 20.0f, 0.1f, "Rate", "hz", "Flange speed in hz.  0.0 to 20.0.  Default = 0.1." }
};

FMOD_DSP_DESCRIPTION_EX * DSPFlange::getDescriptionEx()
{
    memset(&dspflange, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspflange.name, "FMOD Flange");
    dspflange.version = 0x00010100;
    dspflange.create = DSPFlange::createCallback;
    dspflange.release = DSPFlange::releaseCallback;
    dspflange.reset = DSPFlange::resetCallback;
    dspflange.read = DSPFlange::readCallback;

    dspflange.numparameters = sizeof(dspflange_param) / sizeof(dspflange_param[0]);
    dspflange.paramdesc = dspflange_param;
    dspflange.setparameter = DSPFlange::setParameterCallback;
    dspflange.getparameter = DSPFlange::getParameterCallback;

    dspflange.mType = FMOD_DSP_TYPE_FLANGE;
    dspflange.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspflange.mSize = sizeof(DSPFlange);

    return &dspflange;
}

FMOD_RESULT DSPFlange::createInternal()
{
    FMOD_RESULT result;
    int count;
    int channels;

    init();

    for (count = 0; count < DSPFLANGE_COSTABLESIZE; count++)
    {
        mCosTab[count] = (float)cos(1.5707963268f * (float)count / (float)DSPFLANGE_COSTABLESIZE);
    }

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, &channels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mFlangeBufferLengthBytes = (int)(40.0f * (float)mOutputRate) / 1000;
    mFlangeBufferLengthBytes *= channels;
    mFlangeBufferLengthBytes *= sizeof(float);
    mFlangeBufferLengthBytes += 1024;

    mFlangeBuffer = (float *)FMOD_Memory_Calloc(mFlangeBufferLengthBytes);
    if (!mFlangeBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    mFlangeTick = 0.0f;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::releaseInternal()
{
    if (mFlangeBuffer)
    {
        FMOD_Memory_Free(mFlangeBuffer);
        mFlangeBuffer = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::resetInternal()
{
    mFlangeBufferPosition = 0;

    if (mFlangeBuffer)
    {
        memset(mFlangeBuffer, 0, mFlangeBufferLengthBytes);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;
    float depth = 0.5f * mDepth;

    for (count = 0; count < length; count++)
    {
        int count2;
        unsigned int pos1 = (mFlangeBufferPosition + (unsigned int)mFlangeTapPosition) % mFlangeBufferLength;
        unsigned int pos2 = pos1 + 1;
        float frac = mFlangeTapPosition - (float)(int)mFlangeTapPosition;

        for (count2 = 0; count2 < inchannels; count2++)
        {
            float in = inbuffer[(count * inchannels) + count2];
            float out;

            out = mDryMix * in;
            out += mWetMix * (((1.0f - frac) * mFlangeBuffer[(pos1 * inchannels) + count2]) + (frac * mFlangeBuffer[(pos2 * inchannels) + count2]));

            mFlangeBuffer[(mFlangeBufferPosition * inchannels) + count2] = in;
            outbuffer[(count * inchannels) + count2] = out;
        }

        if (!mFlangeBufferPosition)
        {
            for (count2 = 0; count2 < inchannels; count2++)
            {
                mFlangeBuffer[(mFlangeBufferLength * inchannels) + count2] = mFlangeBuffer[count2];
            }
        }

        mFlangeBufferPosition++;
        if (mFlangeBufferPosition >= mFlangeBufferLength)
        {
            mFlangeBufferPosition = 0;
        }

        mFlangeTapPosition = depth * (1.0f + sine(mFlangeTick));
        mFlangeTapPosition = mFlangeTapPosition * (float)(mFlangeBufferLength - 1);

        mFlangeTick += mFlangeSpeed;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::setParameterInternal(int index, float value)
{
    float olddepth = mDepth;

    switch (index)
    {
        case 0:
        {
            mDryMix = value;
            break;
        }
        case 1:
        {
            mWetMix = value;
            break;
        }
        case 2:
        {
            mDepth = value;
            break;
        }
        case 3:
        {
            mRate = value;
            break;
        }
    }

    if (mDepth != olddepth)
    {
        mFlangeBufferLength = (int)((float)mOutputRate * (10.0f * mDepth) / 1000.0f);
        if (mFlangeBufferLength < 4)
        {
            mFlangeBufferLength = 4;
        }

        resetInternal();
    }

    mFlangeSpeed = mRate / (float)mOutputRate;

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mDryMix;
            sprintf(valuestr, "%.1f", mDryMix * 100.0f);
            break;
        }
        case 1:
        {
            *value = mWetMix;
            sprintf(valuestr, "%.1f", mWetMix * 100.0f);
            break;
        }
        case 2:
        {
            *value = mDepth;
            sprintf(valuestr, "%.02f", mDepth);
            break;
        }
        case 3:
        {
            *value = mRate;
            sprintf(valuestr, "%.02f", mRate);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFlange::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->createInternal();
}

FMOD_RESULT DSPFlange::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->releaseInternal();
}

FMOD_RESULT DSPFlange::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->resetInternal();
}

FMOD_RESULT DSPFlange::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPFlange::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->setParameterInternal(index, value);
}

FMOD_RESULT DSPFlange::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPFlange * flange = (DSPFlange *)dsp;

    return flange->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
