// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F4AB8..0x805F5914 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; this older build
// keeps a float echo buffer per channel instead of the 4.06 short buffers. DSPEcho layout from G2MEAB accesses.

#include "fmod_dsp_echo.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspecho;

FMOD_DSP_PARAMETERDESC dspecho_param[5] =
{
    { 1.0f, 5000.0f, 500.0f, "Delay", "ms", "Echo delay in ms.  10  to 5000.  Default = 500." },
    { 0.0f, 1.0f, 0.5f, "Decay", "%", "Echo decay per delay.  0 to 1.  1.0 = No decay, 0.0 = total decay.  Default = 0.5." },
    { 0.0f, 16.0f, 0.0f, "Max channels", "channels", "Maximum channels supported.  0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  Default = 0.  It is suggested to leave at 0!" },
    { 0.0f, 1.0f, 1.0f, "Drymix", "%", "Volume of original signal to pass to output.  0.0 to 1.0. Default = 1.0." },
    { 0.0f, 1.0f, 1.0f, "Wetmix", "%", "Volume of echo delay signal to pass to output.  0.0 to 1.0. Default = 1.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPEcho::getDescriptionEx()
{
    memset(&dspecho, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspecho.name, "FMOD Echo");
    dspecho.version = 0x00010100;
    dspecho.create = DSPEcho::createCallback;
    dspecho.release = DSPEcho::releaseCallback;
    dspecho.reset = DSPEcho::resetCallback;
    dspecho.read = DSPEcho::readCallback;

    dspecho.numparameters = sizeof(dspecho_param) / sizeof(dspecho_param[0]);
    dspecho.paramdesc = dspecho_param;
    dspecho.setparameter = DSPEcho::setParameterCallback;
    dspecho.getparameter = DSPEcho::getParameterCallback;

    dspecho.mType = FMOD_DSP_TYPE_ECHO;
    dspecho.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspecho.mSize = sizeof(DSPEcho);

    return &dspecho;
}

FMOD_RESULT DSPEcho::createInternal()
{
    int count;

    init();

    mChannels = 0;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::releaseInternal()
{
    if (mEchoBuffer)
    {
        FMOD_Memory_Free(mEchoBuffer);
        mEchoBuffer = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::resetInternal()
{
    mEchoPosition = 0;

    if (mEchoBuffer)
    {
        memset(mEchoBuffer, 0, mEchoBufferLengthBytes);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;

    if (inchannels > mChannels)
    {
        memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

    if (inchannels == 1)
    {
        while (length)
        {
            unsigned int len = length;
            float * echobuff = mEchoBuffer + mEchoPosition;

            if (mEchoPosition + length > mEchoLength)
            {
                len = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len; count++)
            {
                float in0 = inbuffer[0];

                outbuffer[0] = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                echobuff[0] = in0 + (echobuff[0] * mDecayRatio);

                inbuffer++;
                outbuffer++;
                echobuff++;
            }

            mEchoPosition += len;
            if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }

            length -= len;
        }
    }
    else if (inchannels == 2)
    {
        while (length)
        {
            unsigned int len = length;
            float * echobuff = mEchoBuffer + (mEchoPosition * 2);

            if (mEchoPosition + length > mEchoLength)
            {
                len = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len; count++)
            {
                float in0 = inbuffer[0];
                float in1 = inbuffer[1];

                outbuffer[0] = (in0 * mDryMix) + (echobuff[0] * mWetMix);
                outbuffer[1] = (in1 * mDryMix) + (echobuff[1] * mWetMix);
                echobuff[0] = in0 + (echobuff[0] * mDecayRatio);
                echobuff[1] = in1 + (echobuff[1] * mDecayRatio);

                inbuffer += 2;
                outbuffer += 2;
                echobuff += 2;
            }

            mEchoPosition += len;
            if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }

            length -= len;
        }
    }
    else
    {
        while (length)
        {
            unsigned int len = length;
            float * echobuff = mEchoBuffer + (mEchoPosition * inchannels);

            if (mEchoPosition + length > mEchoLength)
            {
                len = mEchoLength - mEchoPosition;
            }

            for (count = 0; count < len; count++)
            {
                int count2;

                for (count2 = 0; count2 < inchannels; count2++)
                {
                    float in0 = inbuffer[count2];

                    outbuffer[count2] = (in0 * mDryMix) + (mWetMix * echobuff[count2]);
                    echobuff[count2] = in0 + (mDecayRatio * echobuff[count2]);
                }

                inbuffer += inchannels;
                outbuffer += inchannels;
                echobuff += inchannels;
            }

            mEchoPosition += len;
            if (mEchoPosition >= mEchoLength)
            {
                mEchoPosition = 0;
            }

            length -= len;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    float olddelay = mDelay;
    int oldchannels = mChannels;

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, &mChannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    switch (index)
    {
        case 0:
        {
            mDelay = value;
            break;
        }
        case 1:
        {
            mDecayRatio = value;
            break;
        }
        case 2:
        {
            mMaxChannels = (int)value;
            if (mMaxChannels)
            {
                mChannels = mMaxChannels;
            }
            break;
        }
        case 3:
        {
            mDryMix = value;
            break;
        }
        case 4:
        {
            mWetMix = value;
            break;
        }
    }

    if (mDelay != olddelay || mChannels != oldchannels || !mEchoBuffer)
    {
        mEchoLength = (int)((float)mOutputRate * mDelay) / 1000;

        if (mEchoBuffer)
        {
            FMOD_Memory_Free(mEchoBuffer);
        }

        mEchoBufferLengthBytes = mEchoLength;
        mEchoBufferLengthBytes *= mChannels;
        mEchoBufferLengthBytes *= sizeof(float);

        mEchoBuffer = (float *)FMOD_Memory_Calloc(mEchoBufferLengthBytes);
        if (!mEchoBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    resetInternal();

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mDelay;
            sprintf(valuestr, "%.02f", mDelay);
            break;
        }
        case 1:
        {
            *value = mDecayRatio;
            sprintf(valuestr, "%.1f", mDecayRatio * 100.0f);
            break;
        }
        case 2:
        {
            *value = (float)mMaxChannels;
            sprintf(valuestr, "%d", mMaxChannels);
            break;
        }
        case 3:
        {
            *value = mDryMix;
            sprintf(valuestr, "%.1f", mDryMix * 100.0f);
            break;
        }
        case 4:
        {
            *value = mWetMix;
            sprintf(valuestr, "%.1f", mWetMix * 100.0f);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPEcho::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->createInternal();
}

FMOD_RESULT DSPEcho::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->releaseInternal();
}

FMOD_RESULT DSPEcho::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->resetInternal();
}

FMOD_RESULT DSPEcho::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPEcho::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->setParameterInternal(index, value);
}

FMOD_RESULT DSPEcho::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPEcho * echo = (DSPEcho *)dsp;

    return echo->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
