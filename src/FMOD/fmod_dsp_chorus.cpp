// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F0414..0x805F1364 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the chorus; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects (description builder, xxxInternal methods, static callbacks).

#include "fmod_dsp_chorus.h"
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

FMOD_DSP_DESCRIPTION_EX dspchorus;

FMOD_DSP_PARAMETERDESC dspchorus_param[8] =
{
    { 0.0f, 1.0f, 0.5f, "Dry mix", "", "Volume of original signal to pass to output.  0.0 to 1.0. Default = 0.5." },
    { 0.0f, 1.0f, 0.5f, "Wet mix tap 1", "", "Volume of 1st chorus tap.  0.0 to 1.0.  Default = 0.5." },
    { 0.0f, 1.0f, 0.5f, "Wet mix tap 2", "", "Volume of 2nd chorus tap. This tap is 90 degrees out of phase of the first tap.  0.0 to 1.0.  Default = 0.5." },
    { 0.0f, 1.0f, 0.5f, "Wet mix tap 3", "", "Volume of 3rd chorus tap. This tap is 90 degrees out of phase of the second tap.  0.0 to 1.0.  Default = 0.5." },
    { 0.0f, 100.0f, 40.0f, "Delay", "ms", "Chorus delay in ms.  0.1 to 100.0.  Default = 40.0 ms." },
    { 0.0f, 20.0f, 0.8f, "Rate", "hz", "Chorus modulation rate in hz.  0.0 to 20.0.  Default = 0.8 hz." },
    { 0.0f, 1.0f, 0.03f, "Depth", "", "Chorus modulation depth.  0.0 to 1.0.  Default = 0.03." },
    { 0.0f, 1.0f, 0.0f, "Feedback", "", "Chorus feedback.  Controls how much of the wet signal gets fed back into the chorus buffer.  0.0 to 1.0.  Default = 0.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPChorus::getDescriptionEx()
{
    memset(&dspchorus, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspchorus.name, "FMOD Chorus");
    dspchorus.version = 0x00010100;
    dspchorus.create = DSPChorus::createCallback;
    dspchorus.release = DSPChorus::releaseCallback;
    dspchorus.reset = DSPChorus::resetCallback;
    dspchorus.read = DSPChorus::readCallback;

    dspchorus.numparameters = sizeof(dspchorus_param) / sizeof(dspchorus_param[0]);
    dspchorus.paramdesc = dspchorus_param;
    dspchorus.setparameter = DSPChorus::setParameterCallback;
    dspchorus.getparameter = DSPChorus::getParameterCallback;

    dspchorus.mType = FMOD_DSP_TYPE_CHORUS;
    dspchorus.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspchorus.mSize = sizeof(DSPChorus);

    return &dspchorus;
}

FMOD_RESULT DSPChorus::createInternal()
{
    FMOD_RESULT result;
    int count;
    int channels;

    init();

    for (count = 0; count < DSPCHORUS_COSTABLESIZE; count++)
    {
        mCosTab[count] = (float)cos(1.5707963268f * (float)count / (float)DSPCHORUS_COSTABLESIZE);
    }

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, &channels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mChorusBufferLengthBytes = (int)(200.0f * (float)mOutputRate) / 1000;
    mChorusBufferLengthBytes *= channels;
    mChorusBufferLengthBytes *= sizeof(float);
    mChorusBufferLengthBytes += 1024;

    mChorusBuffer = (float *)FMOD_Memory_Calloc(mChorusBufferLengthBytes);
    if (!mChorusBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    mChorusTick = 0.0f;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::releaseInternal()
{
    if (mChorusBuffer)
    {
        FMOD_Memory_Free(mChorusBuffer);
        mChorusBuffer = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::resetInternal()
{
    mChorusBufferPosition = 0;

    if (mChorusBuffer)
    {
        memset(mChorusBuffer, 0, mChorusBufferLengthBytes);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;
    int halflength = mChorusBufferLength / 2;
    float depth = 0.5f * mDepth;

    for (count = 0; count < length; count++)
    {
        int count2;
        unsigned int pos1 = (mChorusBufferPosition + (unsigned int)mTapPosition1) % mChorusBufferLength;
        unsigned int pos2 = (mChorusBufferPosition + (unsigned int)mTapPosition2) % mChorusBufferLength;
        unsigned int pos3 = (mChorusBufferPosition + (unsigned int)mTapPosition3) % mChorusBufferLength;
        unsigned int pos1next = pos1 + 1;
        unsigned int pos2next = pos2 + 1;
        unsigned int pos3next = pos3 + 1;
        float frac1 = mTapPosition1 - (float)(int)mTapPosition1;
        float frac2 = mTapPosition2 - (float)(int)mTapPosition2;
        float frac3 = mTapPosition3 - (float)(int)mTapPosition3;

        for (count2 = 0; count2 < inchannels; count2++)
        {
            float in = inbuffer[(count * inchannels) + count2];
            float out;

            out = mDryMix * in;
            out += mWetMix1 * (((1.0f - frac1) * mChorusBuffer[(pos1 * inchannels) + count2]) + (frac1 * mChorusBuffer[(pos1next * inchannels) + count2]));
            out += mWetMix2 * (((1.0f - frac2) * mChorusBuffer[(pos2 * inchannels) + count2]) + (frac2 * mChorusBuffer[(pos2next * inchannels) + count2]));
            out += mWetMix3 * (((1.0f - frac3) * mChorusBuffer[(pos3 * inchannels) + count2]) + (frac3 * mChorusBuffer[(pos3next * inchannels) + count2]));

            mChorusBuffer[(mChorusBufferPosition * inchannels) + count2] = in + (out * mFeedback);
            outbuffer[(count * inchannels) + count2] = out;
        }

        if (!mChorusBufferPosition)
        {
            for (count2 = 0; count2 < inchannels; count2++)
            {
                mChorusBuffer[(mChorusBufferLength * inchannels) + count2] = mChorusBuffer[count2];
            }
        }

        mChorusBufferPosition++;
        if (mChorusBufferPosition >= mChorusBufferLength)
        {
            mChorusBufferPosition = 0;
        }

        mTapPosition1 = depth * (1.0f + sine(mChorusTick));
        mTapPosition2 = depth * (1.0f + sine(mChorusTick + 0.25f));
        mTapPosition3 = depth * (1.0f + sine(mChorusTick + 0.5f));

        mTapPosition1 = (float)halflength + (mTapPosition1 * (float)mChorusBufferLength);
        mTapPosition2 = (float)halflength + (mTapPosition2 * (float)mChorusBufferLength);
        mTapPosition3 = (float)halflength + (mTapPosition3 * (float)mChorusBufferLength);

        mChorusTick += mRateHz;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::setParameterInternal(int index, float value)
{
    float olddelay = mDelay;

    switch (index)
    {
        case 0:
        {
            mDryMix = value;
            break;
        }
        case 1:
        {
            mWetMix1 = value;
            break;
        }
        case 2:
        {
            mWetMix2 = value;
            break;
        }
        case 3:
        {
            mWetMix3 = value;
            break;
        }
        case 4:
        {
            mDelay = value;
            break;
        }
        case 5:
        {
            mRate = value;
            break;
        }
        case 6:
        {
            mDepth = value;
            break;
        }
        case 7:
        {
            mFeedback = value;
            break;
        }
    }

    if (mDelay != olddelay)
    {
        mChorusBufferLength = (int)((float)mOutputRate * mDelay / 1000.0f) * 2;
        if (mChorusBufferLength < 4)
        {
            mChorusBufferLength = 4;
        }

        resetInternal();
    }

    mRateHz = mRate / (float)mOutputRate;

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mDryMix;
            sprintf(valuestr, "%.02f", mDryMix);
            break;
        }
        case 1:
        {
            *value = mWetMix1;
            sprintf(valuestr, "%.02f", mWetMix1);
            break;
        }
        case 2:
        {
            *value = mWetMix2;
            sprintf(valuestr, "%.02f", mWetMix2);
            break;
        }
        case 3:
        {
            *value = mWetMix3;
            sprintf(valuestr, "%.02f", mWetMix3);
            break;
        }
        case 4:
        {
            *value = mDelay;
            sprintf(valuestr, "%.02f", mDelay);
            break;
        }
        case 5:
        {
            *value = mRate;
            sprintf(valuestr, "%.02f", mRate);
            break;
        }
        case 6:
        {
            *value = mDepth;
            sprintf(valuestr, "%.02f", mDepth);
            break;
        }
        case 7:
        {
            *value = mFeedback;
            sprintf(valuestr, "%.02f", mFeedback);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPChorus::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->createInternal();
}

FMOD_RESULT DSPChorus::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->releaseInternal();
}

FMOD_RESULT DSPChorus::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->resetInternal();
}

FMOD_RESULT DSPChorus::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPChorus::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->setParameterInternal(index, value);
}

FMOD_RESULT DSPChorus::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPChorus * chorus = (DSPChorus *)dsp;

    return chorus->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
