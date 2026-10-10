// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F8734..0x805F9180 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the IT echo; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects (description builder, xxxInternal methods, static callbacks).

#include "fmod_dsp_itecho.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspitecho;

FMOD_DSP_PARAMETERDESC dspitecho_param[5] =
{
    { 0.0f, 100.0f, 50.0f, "WetDryMix", "", "Ratio of wet (processed) signal to dry (unprocessed) signal. Must be in the range from 0.0 through 100.0 (all wet). The default value is 50." },
    { 0.0f, 100.0f, 50.0f, "Feedback", "%", "Percentage of output fed back into input, in the range from 0.0 through 100.0. The default value is 50." },
    { 1.0f, 2000.0f, 500.0f, "LeftDelay", "ms", "Delay for left channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms." },
    { 1.0f, 2000.0f, 500.0f, "RightDelay", "ms", "Delay for right channel, in milliseconds, in the range from 1.0 through 2000.0. The default value is 500 ms." },
    { 0.0f, 1.0f, 0.0f, "PanDelay", "", "Value that specifies whether to swap left and right delays with each successive echo. The default value is zero, meaning no swap. Possible values are defined as 0.0 (equivalent to FALSE) and 1.0 (equivalent to TRUE)." }
};

FMOD_DSP_DESCRIPTION_EX * DSPITEcho::getDescriptionEx()
{
    memset(&dspitecho, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspitecho.name, "FMOD IT Echo");
    dspitecho.version = 0x00010100;
    dspitecho.create = DSPITEcho::createCallback;
    dspitecho.release = DSPITEcho::releaseCallback;
    dspitecho.reset = DSPITEcho::resetCallback;
    dspitecho.read = DSPITEcho::readCallback;

    dspitecho.numparameters = sizeof(dspitecho_param) / sizeof(dspitecho_param[0]);
    dspitecho.paramdesc = dspitecho_param;
    dspitecho.setparameter = DSPITEcho::setParameterCallback;
    dspitecho.getparameter = DSPITEcho::getParameterCallback;

    dspitecho.mType = FMOD_DSP_TYPE_ITECHO;
    dspitecho.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspitecho.mSize = sizeof(DSPITEcho);

    return &dspitecho;
}

FMOD_RESULT DSPITEcho::createInternal()
{
    int count;

    init();

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::releaseInternal()
{
    int count;

    for (count = 0; count < 2; count++)
    {
        if (mEchoBuffer[count])
        {
            FMOD_Memory_Free(mEchoBuffer[count]);
            mEchoBuffer[count] = 0;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::resetInternal()
{
    mEchoPosition[0] = 0;
    mEchoPosition[1] = 0;

    if (mEchoBuffer[0])
    {
        memset(mEchoBuffer[0], 0, mEchoBufferLengthBytes[0]);
    }
    if (mEchoBuffer[1])
    {
        memset(mEchoBuffer[1], 0, mEchoBufferLengthBytes[1]);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    int count;

    for (count = 0; count < inchannels && count < 2; count++)
    {
        float * in = inbuffer + count;
        float * out = outbuffer + count;
        unsigned int remaining = length;

        while (remaining)
        {
            unsigned int count2;
            unsigned int len = remaining;
            float * echobuff = mEchoBuffer[count] + mEchoPosition[count];

            if (mEchoPosition[count] + remaining > mEchoLength[count])
            {
                len = mEchoLength[count] - mEchoPosition[count];
            }

            for (count2 = 0; count2 < len; count2++)
            {
                float in0 = *in;

                *out = (in0 * (1.0f - mWetDryMix)) + (*echobuff * mWetDryMix);
                *echobuff = in0 + (*echobuff * mFeedback);

                in += inchannels;
                out += inchannels;
                echobuff++;
            }

            mEchoPosition[count] += len;
            if (mEchoPosition[count] >= mEchoLength[count])
            {
                mEchoPosition[count] = 0;
            }

            remaining -= len;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int count;
    float olddelay[2];
    bool reset = false;

    olddelay[0] = mDelay[0];
    olddelay[1] = mDelay[1];

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    switch (index)
    {
        case 0:
        {
            mWetDryMix = value / 100.0f;
            break;
        }
        case 1:
        {
            mFeedback = value / 100.0f;
            break;
        }
        case 2:
        {
            mDelay[0] = value;
            break;
        }
        case 3:
        {
            mDelay[1] = value;
            break;
        }
        case 4:
        {
            mPanDelay = value < 0.5f ? false : true;
            break;
        }
    }

    for (count = 0; count < 2; count++)
    {
        if (mDelay[count] != olddelay[count] || !mEchoBuffer[count])
        {
            mEchoLength[count] = (int)((float)mOutputRate * mDelay[count]) / 1000;

            if (mEchoBuffer[count])
            {
                FMOD_Memory_Free(mEchoBuffer[count]);
            }

            mEchoBufferLengthBytes[count] = mEchoLength[count];
            mEchoBufferLengthBytes[count] *= sizeof(float);

            mEchoBuffer[count] = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes[count]);
            if (!mEchoBuffer[count])
            {
                return FMOD_ERR_MEMORY;
            }

            reset = true;
        }
    }

    if (reset)
    {
        resetInternal();
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mWetDryMix * 100.0f;
            sprintf(valuestr, "%.1f", mWetDryMix * 100.0f);
            break;
        }
        case 1:
        {
            *value = mFeedback * 100.0f;
            sprintf(valuestr, "%.1f", mFeedback * 100.0f);
            break;
        }
        case 2:
        {
            *value = mDelay[0];
            sprintf(valuestr, "%.02f", mDelay[0]);
            break;
        }
        case 3:
        {
            *value = mDelay[1];
            sprintf(valuestr, "%.02f", mDelay[1]);
            break;
        }
        case 4:
        {
            *value = mPanDelay ? 1.0f : 0.0f;
            sprintf(valuestr, "%s", mPanDelay ? "on" : "off");
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITEcho::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->createInternal();
}

FMOD_RESULT DSPITEcho::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->releaseInternal();
}

FMOD_RESULT DSPITEcho::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->resetInternal();
}

FMOD_RESULT DSPITEcho::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPITEcho::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->setParameterInternal(index, value);
}

FMOD_RESULT DSPITEcho::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPITEcho * itecho = (DSPITEcho *)dsp;

    return itecho->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
