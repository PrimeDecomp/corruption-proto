// G2MEAB fmod_dsp_wavetable.cpp: complete reconstruction (group D).
// .text: 0x80606428..0x80606C50 (11 native functions, in this order): alloc 0x80606428, addInput 0x8060649C,
// execute 0x806064CC, setPositionInternal 0x806069E4, setParameterInternal 0x80606A1C, getParameterInternal
// 0x80606A24, setFrequency 0x80606A2C, the three FMOD_DSP_STATE callbacks and the implicit deleting dtor
// 0x80606B60 (vtable 0x806EDDF0).

#include "fmod_dsp_wavetable.h"
#include "fmod.h"
#include "fmod_channel_software.h"
#include "fmod_dsp.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dspi.h"
#include "fmod_sample_software.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

FMOD_RESULT DSPWaveTable::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
    FMOD_RESULT result;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    mFrequency = 0.0f;

    result = mSystem->getSoftwareFormat(&mTargetFrequency, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;

    return FMOD_OK;
}

FMOD_RESULT DSPWaveTable::addInput(DSPI * target)
{
    FMOD_RESULT result;

    result = DSPI::addInput(target);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPWaveTable::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    *outbuffer = inbuffer;
    mIdle = false;

    if (!mVisited)
    {
        unsigned int len = *length;
        int outpos = 0;
        SoundI * sound = mSound;
        void * data;

        if (!sound)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        if (mOutputBuffer)
        {
            *outbuffer = mOutputBuffer;
        }

        data = ((SampleSoftware *)sound)->mBuffer;

        do
        {
            int finished = 0;
            FMOD_SINT64P speed;
            unsigned int outlength;

            speed = mSpeed;
            if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
            {
                speed.mValue = -speed.mValue;
            }

            outlength = len;

            if (mSpeed.mValue > 0x100)
            {
                FMOD_UINT64P samplesleft;
                FMOD_UINT64 remainder;

                if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
                {
                    if (mPosition.mHi >= mChannel->mLoopStart)
                    {
                        samplesleft.mHi = mPosition.mHi - mChannel->mLoopStart;
                    }
                    else
                    {
                        samplesleft.mHi = mPosition.mHi;
                    }
                    samplesleft.mLo = 0;
                }
                else
                {
                    if ((mChannel->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI)) && mChannel->mLoopCount)
                    {
                        samplesleft.mHi = mChannel->mLoopStart + mChannel->mLoopLength;
                    }
                    else
                    {
                        samplesleft.mHi = sound->mLength;
                    }
                    samplesleft.mLo = 0;

                    if (mPosition.mValue < samplesleft.mValue)
                    {
                        samplesleft.mValue -= mPosition.mValue;
                    }
                    else
                    {
                        samplesleft.mValue = 0;
                    }
                }

                remainder = samplesleft.mValue % mSpeed.mValue;
                samplesleft.mValue /= mSpeed.mValue;
                if (remainder)
                {
                    samplesleft.mValue++;
                }

                if (samplesleft.mLo <= len)
                {
                    finished = 2;
                    outlength = samplesleft.mLo;
                }
            }

            if (mSpeed.mHi == 1 && speed.mLo == 0)
            {
                FMOD_Resampler_NoInterp(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
            }
            else
            {
                switch (mSystem->mResampleMethod)
                {
                    case 0:
                    {
                        FMOD_Resampler_NoInterp(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
                        break;
                    }
                    case 1:
                    {
                        FMOD_Resampler_Linear(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
                        break;
                    }
                    case 2:
                    {
                        FMOD_Resampler_Cubic(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
                        break;
                    }
                    case 3:
                    {
                        FMOD_Resampler_Spline(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
                        break;
                    }
                    default:
                    {
                        FMOD_Resampler_Linear(*outbuffer + (outpos * mSound->mChannels), outlength, data, mSound->mFormat, &mPosition, &speed, mSound->mChannels);
                        break;
                    }
                }
            }

            len -= outlength;
            outpos += outlength;

            if (finished == 2)
            {
                if ((mChannel->mMode & FMOD_LOOP_BIDI) && mChannel->mLoopCount)
                {
                    if ((int)mPosition.mHi < 0)
                    {
                        mPosition.mHi = 0;
                    }

                    if (mDirection == DSPWAVETABLE_SPEEDDIR_FORWARDS)
                    {
                        mDirection = DSPWAVETABLE_SPEEDDIR_BACKWARDS;
                    }
                    else
                    {
                        mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;
                    }
                }
                else if ((mChannel->mMode & FMOD_LOOP_NORMAL) && mChannel->mLoopCount)
                {
                    if (mDirection == DSPWAVETABLE_SPEEDDIR_BACKWARDS)
                    {
                        do
                        {
                            mPosition.mHi += mChannel->mLoopLength;
                        } while (mPosition.mHi < mChannel->mLoopStart);
                    }
                    else
                    {
                        do
                        {
                            mPosition.mHi -= mChannel->mLoopLength;
                        } while (mPosition.mHi >= mChannel->mLoopStart + mChannel->mLoopLength);

                        if (mChannel->mLoopCount > 0)
                        {
                            mChannel->mLoopCount--;
                        }
                    }
                }
                else
                {
                    mPosition.mHi = sound->mLength;
                    mPosition.mLo = 0;

                    memset(*outbuffer + (outpos * mSound->mChannels), 0, len * mSound->mChannels * sizeof(float));

                    mUnk63 = true;
                    break;
                }
            }
        } while (len);
    }
    else
    {
        *outbuffer = mOutputBuffer;
    }

    *outchannels = mSound->mChannels;

    return FMOD_OK;
}

FMOD_RESULT DSPWaveTable::setPositionInternal(unsigned int position)
{
    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (position >= mSound->mLength)
    {
        position = mSound->mLength;
    }

    mPosition.mHi = position;
    mPosition.mLo = 0;

    return FMOD_OK;
}

FMOD_RESULT DSPWaveTable::setParameterInternal(int index, float value)
{
    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT DSPWaveTable::getParameterInternal(int index, float * value, char * valuestr)
{
    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT DSPWaveTable::setFrequency(float frequency)
{
    if (frequency < 0.0f)
    {
        mDirection = DSPWAVETABLE_SPEEDDIR_BACKWARDS;
        frequency = -frequency;
    }
    else if (mSound && !(mChannel->mMode & FMOD_LOOP_BIDI))
    {
        mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;
    }

    mFrequency = frequency;

    mSpeed.mValue = (FMOD_UINT64)(4294967296.0f * (mFrequency / (float)mTargetFrequency));

    return FMOD_OK;
}

FMOD_RESULT DSPWaveTable::setPositionCallback(FMOD_DSP_STATE * dsp, unsigned int position)
{
    DSPWaveTable * dspwave = (DSPWaveTable *)dsp;

    return dspwave->setPositionInternal(position);
}

FMOD_RESULT DSPWaveTable::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPWaveTable * dspwave = (DSPWaveTable *)dsp;

    return dspwave->setParameterInternal(index, value);
}

FMOD_RESULT DSPWaveTable::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPWaveTable * dspwave = (DSPWaveTable *)dsp;

    return dspwave->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
