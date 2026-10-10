// G2MEAB fmod_dsp_resampler.cpp: complete reconstruction (group D).
// .text: 0x805FDA60..0x80605958 (13 native functions): ctor 0x805FDA60, weak DSPI dtor 0x805FDAF0, release
// 0x805FDBD0, alloc 0x805FDC68, execute 0x805FDDA0, addInput 0x805FE3E4, setFrequency 0x805FE3EC, setPosition
// 0x805FE458, dtor 0x805FE4C8, then the interpolation kernels Cubic 0x805FE5C8, Linear 0x8060044C,
// NoInterp 0x8060242C and Spline 0x80603D20. The kernels follow in alphabetical order, as the separate
// 4.06 sources fmod_dsp_resampler_{cubic,linear,nointerp,spline}.cpp would link; the split keeps them in this
// unit. Their loop shapes and local names follow the 4.06 DWARF and the Gormiti (MWCC) linear kernel.

#include "fmod_dsp_resampler.h"
#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"
#include "fmod_channel_real.h"
#include "fmod_soundi.h"

#include <string.h>

namespace FMOD {

DSPResampler::DSPResampler()
{
    mResampleBufferMemory = 0;
    mResampleBuffer = 0;
    mTargetFrequency = 0;
    mSpeed.mValue = 0;
    mOverflowLength = FMOD_DSP_RESAMPLER_OVERFLOWLENGTH;
    mResampleBufferPos = 0;
    mResampleFinishPos = (unsigned int)-1;
    mFill = 2;
    mResamplePosition.mValue = 0;
    mPosition.mValue = 0;
}

FMOD_RESULT DSPResampler::release(bool freethis)
{
    FMOD_RESULT result;

    result = DSPI::release(false);

    if (mResampleBufferMemory)
    {
        FMOD_Memory_Free(mResampleBufferMemory);
        mResampleBufferMemory = 0;
    }

    if (freethis)
    {
        FMOD_Memory_Free(this);
    }

    return result;
}

FMOD_RESULT DSPResampler::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
    FMOD_RESULT result;
    int channels;

    result = DSPI::alloc(description);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mSystem->getSoftwareFormat(&mTargetFrequency, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (description->mResamplerBlockLength)
    {
        mResampleBlockLength = description->mResamplerBlockLength;
        channels = description->channels;
    }
    else
    {
        result = mSystem->getDSPBufferSize(&mResampleBlockLength, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
        channels = 8;
    }

    mResampleBufferLength = mResampleBlockLength * 2;

    mResampleBufferMemory = FMOD_Memory_Calloc((mResampleBufferLength + (mOverflowLength * 4)) * channels * sizeof(float));
    if (!mResampleBufferMemory)
    {
        return FMOD_ERR_MEMORY;
    }

    mResampleBuffer = (float *)mResampleBufferMemory + (mOverflowLength * channels);

    mResampleBufferPos = 0;
    mResampleFinishPos = (unsigned int)-1;
    mFill = 2;
    mPosition.mValue = 0;
    mResamplePosition.mValue = 0;

    return FMOD_OK;
}

FMOD_RESULT DSPResampler::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    float * out;
    int channels = 0;

    if (mUnk63)
    {
        return FMOD_OK;
    }

    mIdle = false;

    if (!mVisited)
    {
        unsigned int len = *length;
        int outpos = 0;
        ChannelReal * channel;
        unsigned int soundlength;
        FMOD_UINT64P endposition;

        do
        {
            int finished = 0;
            unsigned int outlength;

            while (mFill)
            {
                unsigned int resamplebufferpos = mResampleBufferPos;
                unsigned int blocklength = mResampleBlockLength;
                float * buffer = (float *)mResampleBuffer + (mResampleBufferPos * mDescription.channels);

                instance = (FMOD_DSP *)this;

                if (mDescription.read(this, 0, buffer, blocklength, mDescription.channels, mDescription.channels) != FMOD_OK)
                {
                    memset(buffer, 0, blocklength * mDescription.channels * sizeof(float));

                    mResampleFinishPos = mResampleBufferPos;
                    if (!mResampleFinishPos)
                    {
                        mResampleFinishPos = mResampleBufferLength;
                    }
                }
                else
                {
                    mResampleBufferPos += blocklength;
                    if (mResampleBufferPos >= mResampleBufferLength)
                    {
                        mResampleBufferPos = 0;
                    }

                    if (!resamplebufferpos)
                    {
                        unsigned int count;

                        for (count = 0; count < mDescription.channels * mOverflowLength * 2; count++)
                        {
                            ((float *)mResampleBuffer)[(mResampleBufferLength * mDescription.channels) + count] = ((float *)mResampleBuffer)[count];
                        }
                    }
                }

                mFill--;
            }

            outlength = len;

            if (mSpeed.mValue > 0x100)
            {
                FMOD_UINT64P samplesleft;
                FMOD_UINT64 remainder;
                bool atend = false;

                samplesleft.mHi = ((((int)(mResamplePosition.mHi - mOverflowLength) / (int)mResampleBlockLength) + 1) * mResampleBlockLength) + mOverflowLength;
                samplesleft.mLo = 0;
                samplesleft.mValue -= mResamplePosition.mValue;

                if (mResampleFinishPos != (unsigned int)-1)
                {
                    FMOD_UINT64P samplestoend;

                    samplestoend.mHi = mResampleFinishPos;
                    samplestoend.mLo = 0;
                    samplestoend.mValue -= mResamplePosition.mValue;

                    if (samplestoend.mValue <= samplesleft.mValue)
                    {
                        samplesleft.mValue = samplestoend.mValue;
                        atend = true;
                    }
                }

                remainder = samplesleft.mValue % mSpeed.mValue;
                samplesleft.mValue /= mSpeed.mValue;
                if (remainder)
                {
                    samplesleft.mValue++;
                }

                if (samplesleft.mValue <= outlength)
                {
                    if (atend)
                    {
                        finished = 2;
                    }
                    else
                    {
                        finished = 1;
                    }
                    outlength = samplesleft.mLo;
                }
            }

            if (mSpeed.mHi == 1 && mSpeed.mLo == 0)
            {
                memcpy(inbuffer + (outpos * mDescription.channels), (float *)mResampleBuffer + (mResamplePosition.mHi * mDescription.channels), outlength * sizeof(float) * mDescription.channels);

                mResamplePosition.mValue += mSpeed.mValue * outlength;
            }
            else
            {
                switch (mSystem->mResampleMethod)
                {
                    case 0:
                    {
                        FMOD_Resampler_NoInterp(inbuffer + (outpos * mDescription.channels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mDescription.channels);
                        break;
                    }
                    case 1:
                    {
                        FMOD_Resampler_Linear(inbuffer + (outpos * mDescription.channels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mDescription.channels);
                        break;
                    }
                    case 2:
                    {
                        FMOD_Resampler_Cubic(inbuffer + (outpos * mDescription.channels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mDescription.channels);
                        break;
                    }
                    case 3:
                    {
                        FMOD_Resampler_Spline(inbuffer + (outpos * mDescription.channels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mDescription.channels);
                        break;
                    }
                    default:
                    {
                        FMOD_Resampler_Linear(inbuffer + (outpos * mDescription.channels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mDescription.channels);
                        break;
                    }
                }
            }

            if (mResamplePosition.mHi >= mResampleBufferLength + mOverflowLength)
            {
                mResamplePosition.mHi -= mResampleBufferLength;
            }

            len -= outlength;
            outpos += outlength;

            if (finished == 2)
            {
                mUnk63 = true;
                break;
            }
            if (finished == 1)
            {
                mFill++;
            }
        } while ((int)len > 0);

        channel = mChannel;
        soundlength = channel->mSound ? channel->mSound->mLength : 0;

        if (soundlength < channel->mLoopStart + channel->mLoopLength)
        {
            channel->mLoopLength = soundlength - channel->mLoopStart;
        }

        if ((channel->mMode & FMOD_LOOP_NORMAL) && channel->mLoopCount)
        {
            endposition.mHi = channel->mLoopStart + channel->mLoopLength - 1;
            endposition.mLo = 0;
        }
        else
        {
            endposition.mHi = soundlength - 1;
            endposition.mLo = 0;
        }

        mPosition.mValue += mSpeed.mValue * *length;

        if (mPosition.mValue > endposition.mValue)
        {
            if (((mChannel->mMode & FMOD_LOOP_NORMAL) && mChannel->mLoopCount) || (soundlength == (unsigned int)-1 && soundlength))
            {
                mPosition.mHi -= mChannel->mLoopLength;
            }
            else
            {
                mPosition.mHi = soundlength;
                mUnk63 = true;
            }
        }

        out = inbuffer;
        channels = mDescription.channels;
    }
    else
    {
        out = mOutputBuffer;
    }

    *outbuffer = out;
    *outchannels = channels;

    return FMOD_OK;
}

FMOD_RESULT DSPResampler::addInput(DSPI * target)
{
    return FMOD_ERR_DSP_CONNECTION;
}

FMOD_RESULT DSPResampler::setFrequency(float frequency)
{
    mFrequency = frequency;

    mSpeed.mValue = (FMOD_UINT64)(4294967296.0f * (mFrequency / (float)mTargetFrequency));

    return FMOD_OK;
}

FMOD_RESULT DSPResampler::setPosition(unsigned int position)
{
    FMOD_RESULT result;

    result = DSPI::setPosition(position);
    if (result != FMOD_OK)
    {
        return result;
    }

    mResampleBufferPos = 0;
    mResampleFinishPos = (unsigned int)-1;
    mFill = 2;
    mResamplePosition.mValue = 0;
    mPosition.mHi = position;
    mPosition.mLo = 0;

    return FMOD_OK;
}

} // namespace FMOD

extern "C" {

void FMOD_Resampler_Cubic(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels)
{
    float scale = 1.0f;

    switch (srcformat)
    {
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char * inptr = (signed char *)src;

            scale /= 128.0f;

            if (channels == 1)
            {
                float f, p0, p1, p2, p3, r1, r2, r3, r4, a, b, c;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r1 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r2 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r3 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r4 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    *out = ((((((a * f) + b) * f) + c) * f) + p1);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float p0, p1, p2, p3, r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        p0 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        p1 = (float)inptr[(position->mHi * channels) + count] * scale;
                        p2 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        p3 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                        b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                        c = (p2 - p0) / 2.0f;
                        r = ((((((a * f) + b) * f) + c) * f) + p1);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM16:
        {
            short * inptr = (short *)src;

            scale /= 32768.0f;

            if (channels == 1)
            {
                float f, p0, p1, p2, p3, r1, r2, r3, r4, a, b, c;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r1 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r2 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r3 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r4 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    *out = ((((((a * f) + b) * f) + c) * f) + p1);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float p0, p1, p2, p3, r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        p0 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        p1 = (float)inptr[(position->mHi * channels) + count] * scale;
                        p2 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        p3 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                        b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                        c = (p2 - p0) / 2.0f;
                        r = ((((((a * f) + b) * f) + c) * f) + p1);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM24:
        {
            FMOD_INT24 * inptr = (FMOD_INT24 *)src;

            scale /= 8388608.0f;

            if (channels == 1)
            {
                while (outlength)
                {
                    float r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);
                    FMOD_INT24 * s0 = &inptr[position->mHi - 1];
                    FMOD_INT24 * s1 = &inptr[position->mHi];
                    FMOD_INT24 * s2 = &inptr[position->mHi + 1];
                    FMOD_INT24 * s3 = &inptr[position->mHi + 2];
                    float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                    float p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                    float p2 = (float)(((s2->val[0] << 8) | (s2->val[1] << 16) | (s2->val[2] << 24)) >> 8) * scale;
                    float p3 = (float)(((s3->val[0] << 8) | (s3->val[1] << 16) | (s3->val[2] << 24)) >> 8) * scale;

                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r = ((((((a * f) + b) * f) + c) * f) + p1);

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        FMOD_INT24 * s0 = &inptr[((position->mHi - 1) * channels) + count];
                        FMOD_INT24 * s1 = &inptr[(position->mHi * channels) + count];
                        FMOD_INT24 * s2 = &inptr[((position->mHi + 1) * channels) + count];
                        FMOD_INT24 * s3 = &inptr[((position->mHi + 2) * channels) + count];
                        float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                        float p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                        float p2 = (float)(((s2->val[0] << 8) | (s2->val[1] << 16) | (s2->val[2] << 24)) >> 8) * scale;
                        float p3 = (float)(((s3->val[0] << 8) | (s3->val[1] << 16) | (s3->val[2] << 24)) >> 8) * scale;

                        a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                        b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                        c = (p2 - p0) / 2.0f;
                        r = ((((((a * f) + b) * f) + c) * f) + p1);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM32:
        {
            int * inptr = (int *)src;

            scale /= -2147483648.0f;

            if (channels == 1)
            {
                float f, p0, p1, p2, p3, r1, r2, r3, r4, a, b, c;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r1 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r2 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r3 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r4 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 1] * scale;
                    p1 = (float)inptr[position->mHi] * scale;
                    p2 = (float)inptr[position->mHi + 1] * scale;
                    p3 = (float)inptr[position->mHi + 2] * scale;
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    *out = ((((((a * f) + b) * f) + c) * f) + p1);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float p0, p1, p2, p3, r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        p0 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        p1 = (float)inptr[(position->mHi * channels) + count] * scale;
                        p2 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        p3 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                        b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                        c = (p2 - p0) / 2.0f;
                        r = ((((((a * f) + b) * f) + c) * f) + p1);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float * inptr = (float *)src;

            if (channels == 1)
            {
                float f, p0, p1, p2, p3, r1, r2, r3, r4, a, b, c;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 1];
                    p1 = inptr[position->mHi];
                    p2 = inptr[position->mHi + 1];
                    p3 = inptr[position->mHi + 2];
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r1 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 1];
                    p1 = inptr[position->mHi];
                    p2 = inptr[position->mHi + 1];
                    p3 = inptr[position->mHi + 2];
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r2 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 1];
                    p1 = inptr[position->mHi];
                    p2 = inptr[position->mHi + 1];
                    p3 = inptr[position->mHi + 2];
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r3 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 1];
                    p1 = inptr[position->mHi];
                    p2 = inptr[position->mHi + 1];
                    p3 = inptr[position->mHi + 2];
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    r4 = ((((((a * f) + b) * f) + c) * f) + p1);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 1];
                    p1 = inptr[position->mHi];
                    p2 = inptr[position->mHi + 1];
                    p3 = inptr[position->mHi + 2];
                    a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                    b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                    c = (p2 - p0) / 2.0f;
                    *out = ((((((a * f) + b) * f) + c) * f) + p1);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float p0, p1, p2, p3, r, a, b, c;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        p0 = inptr[((position->mHi - 1) * channels) + count];
                        p1 = inptr[(position->mHi * channels) + count];
                        p2 = inptr[((position->mHi + 1) * channels) + count];
                        p3 = inptr[((position->mHi + 2) * channels) + count];
                        a = (((p1 - p2) * 3.0f) - p0 + p3) / 2.0f;
                        b = (p2 * 2.0f) + p0 - (((p1 * 5.0f) + p3) / 2.0f);
                        c = (p2 - p0) / 2.0f;
                        r = ((((((a * f) + b) * f) + c) * f) + p1);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
    }
}

void FMOD_Resampler_Linear(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels)
{
    float scale = 1.0f;

    switch (srcformat)
    {
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char * inptr = (signed char *)src;

            scale /= 128.0f;

            if (channels == 1)
            {
                float f, a, b, r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r1 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r2 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r3 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r4 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    *out = (a * (1.0f - f)) + (b * f);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else if (channels == 2)
            {
                float f, l_a, l_b, l_r1, l_r2, l_r3, l_r4, r_a, r_b, r_r1, r_r2, r_r3, r_r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r1 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r1 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r2 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r2 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r3 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r3 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r4 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r4 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;

                    out[0] = l_r1;
                    out[1] = r_r1;
                    out[2] = l_r2;
                    out[3] = r_r2;
                    out[4] = l_r3;
                    out[5] = r_r3;
                    out[6] = l_r4;
                    out[7] = r_r4;

                    len--;
                    out += 8;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r1 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r1 = (r_a * (1.0f - f)) + (r_b * f);
                    out[0] = l_r1;
                    out[1] = r_r1;
                    out += 2;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float p0 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM16:
        {
            short * inptr = (short *)src;

            scale /= 32768.0f;

            if (channels == 1)
            {
                float f, a, b, r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r1 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r2 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r3 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r4 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    *out = (a * (1.0f - f)) + (b * f);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else if (channels == 2)
            {
                float f, l_a, l_b, l_r1, l_r2, l_r3, l_r4, r_a, r_b, r_r1, r_r2, r_r3, r_r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r1 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r1 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r2 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r2 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r3 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r3 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r4 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r4 = (r_a * (1.0f - f)) + (r_b * f);
                    position->mValue += speed->mValue;

                    out[0] = l_r1;
                    out[1] = r_r1;
                    out[2] = l_r2;
                    out[3] = r_r2;
                    out[4] = l_r3;
                    out[5] = r_r3;
                    out[6] = l_r4;
                    out[7] = r_r4;

                    len--;
                    out += 8;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    l_a = (float)inptr[(position->mHi * 2) + 0] * scale;
                    l_b = (float)inptr[(position->mHi * 2) + 2] * scale;
                    r_a = (float)inptr[(position->mHi * 2) + 1] * scale;
                    r_b = (float)inptr[(position->mHi * 2) + 3] * scale;
                    l_r1 = (l_a * (1.0f - f)) + (l_b * f);
                    r_r1 = (r_a * (1.0f - f)) + (r_b * f);
                    out[0] = l_r1;
                    out[1] = r_r1;
                    position->mValue += speed->mValue;
                    out += 2;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float p0 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM24:
        {
            FMOD_INT24 * inptr = (FMOD_INT24 *)src;

            scale /= 8388608.0f;

            if (channels == 1)
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        FMOD_INT24 * s0 = &inptr[position->mHi];
                        FMOD_INT24 * s1 = &inptr[position->mHi + 1];
                        float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                        float p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        FMOD_INT24 * s0 = &inptr[(position->mHi * channels) + count];
                        FMOD_INT24 * s1 = &inptr[((position->mHi + 1) * channels) + count];
                        float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                        float p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM32:
        {
            int * inptr = (int *)src;

            scale /= -2147483648.0f;

            if (channels == 1)
            {
                float f, a, b, r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r1 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r2 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r3 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    r4 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = (float)inptr[position->mHi] * scale;
                    b = (float)inptr[position->mHi + 1] * scale;
                    *out = (a * (1.0f - f)) + (b * f);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float p0 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float * inptr = (float *)src;

            if (channels == 1)
            {
                float f, a, b, r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = inptr[position->mHi];
                    b = inptr[position->mHi + 1];
                    r1 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = inptr[position->mHi];
                    b = inptr[position->mHi + 1];
                    r2 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = inptr[position->mHi];
                    b = inptr[position->mHi + 1];
                    r3 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = inptr[position->mHi];
                    b = inptr[position->mHi + 1];
                    r4 = (a * (1.0f - f)) + (b * f);
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    a = inptr[position->mHi];
                    b = inptr[position->mHi + 1];
                    *out = (a * (1.0f - f)) + (b * f);
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float p0 = inptr[(position->mHi * channels) + count];
                        float p1 = inptr[((position->mHi + 1) * channels) + count];
                        float r = (p0 * (1.0f - f)) + (p1 * f);

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
    }
}

void FMOD_Resampler_NoInterp(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels)
{
    float scale = 1.0f;

    switch (srcformat)
    {
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char * inptr = (signed char *)src;

            scale /= 128.0f;

            if (channels == 1)
            {
                float r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    *out = (float)inptr[position->mHi] * scale;
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out = (float)inptr[(position->mHi * channels) + count] * scale;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM16:
        {
            short * inptr = (short *)src;

            scale /= 32768.0f;

            if (channels == 1)
            {
                float r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    *out = (float)inptr[position->mHi] * scale;
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out = (float)inptr[(position->mHi * channels) + count] * scale;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM24:
        {
            FMOD_INT24 * inptr = (FMOD_INT24 *)src;

            scale /= 8388608.0f;

            if (channels == 1)
            {
                int len = outlength >> 2;

                while (len)
                {
                    FMOD_INT24 * s;
                    float p0, p1, p2, p3;

                    s = &inptr[position->mHi];
                    p0 = (float)(((s->val[0] << 8) | (s->val[1] << 16) | (s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p1 = (float)(((s->val[0] << 8) | (s->val[1] << 16) | (s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p2 = (float)(((s->val[0] << 8) | (s->val[1] << 16) | (s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;
                    s = &inptr[position->mHi];
                    p3 = (float)(((s->val[0] << 8) | (s->val[1] << 16) | (s->val[2] << 24)) >> 8) * scale;
                    position->mValue += speed->mValue;

                    out[0] = p0;
                    out[1] = p1;
                    out[2] = p2;
                    out[3] = p3;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    FMOD_INT24 * s0 = &inptr[position->mHi];
                    float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;

                    *out = p0;
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        FMOD_INT24 * s0 = &inptr[(position->mHi * channels) + count];
                        float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;

                        *out = p0;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM32:
        {
            int * inptr = (int *)src;

            scale /= -2147483648.0f;

            if (channels == 1)
            {
                float r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    r1 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r2 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r3 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;
                    r4 = (float)inptr[position->mHi] * scale;
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    *out = (float)inptr[position->mHi] * scale;
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out = (float)inptr[(position->mHi * channels) + count] * scale;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float * inptr = (float *)src;

            if (channels == 1)
            {
                float r1, r2, r3, r4;
                int len = outlength >> 2;

                while (len)
                {
                    r1 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r2 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r3 = inptr[position->mHi];
                    position->mValue += speed->mValue;
                    r4 = inptr[position->mHi];
                    position->mValue += speed->mValue;

                    out[0] = r1;
                    out[1] = r2;
                    out[2] = r3;
                    out[3] = r4;

                    len--;
                    out += 4;
                }

                len = outlength & 3;
                while (len)
                {
                    *out = inptr[position->mHi];
                    out++;
                    position->mValue += speed->mValue;
                    len--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;

                    for (count = 0; count < channels; count++)
                    {
                        *out = inptr[(position->mHi * channels) + count];
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
    }
}

void FMOD_Resampler_Spline(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels)
{
    float scale = 1.0f;

    switch (srcformat)
    {
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char * inptr = (signed char *)src;

            scale /= 128.0f;

            if (channels == 1)
            {
                float f, r, p0, p1, p2, p3, p4, p5;

                while (outlength)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 2] * scale;
                    p1 = (float)inptr[position->mHi - 1] * scale;
                    p2 = (float)inptr[position->mHi] * scale;
                    p3 = (float)inptr[position->mHi + 1] * scale;
                    p4 = (float)inptr[position->mHi + 2] * scale;
                    p5 = (float)inptr[position->mHi + 3] * scale;

                    r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                        + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                        + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                        + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                        + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float r;
                        float p0 = (float)inptr[((position->mHi - 2) * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        float p2 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p3 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float p4 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        float p5 = (float)inptr[((position->mHi + 3) * channels) + count] * scale;

                        r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                            + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                            + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                            + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                            + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM16:
        {
            short * inptr = (short *)src;

            scale /= 32768.0f;

            if (channels == 1)
            {
                float f, r, p0, p1, p2, p3, p4, p5;

                while (outlength)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 2] * scale;
                    p1 = (float)inptr[position->mHi - 1] * scale;
                    p2 = (float)inptr[position->mHi] * scale;
                    p3 = (float)inptr[position->mHi + 1] * scale;
                    p4 = (float)inptr[position->mHi + 2] * scale;
                    p5 = (float)inptr[position->mHi + 3] * scale;

                    r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                        + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                        + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                        + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                        + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float r;
                        float p0 = (float)inptr[((position->mHi - 2) * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        float p2 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p3 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float p4 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        float p5 = (float)inptr[((position->mHi + 3) * channels) + count] * scale;

                        r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                            + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                            + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                            + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                            + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM24:
        {
            FMOD_INT24 * inptr = (FMOD_INT24 *)src;

            scale /= 8388608.0f;

            if (channels == 1)
            {
                float f, r, p0, p1, p2, p3, p4, p5;
                FMOD_INT24 * s0, * s1, * s2, * s3, * s4, * s5;

                while (outlength)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    s0 = &inptr[position->mHi - 2];
                    s1 = &inptr[position->mHi - 1];
                    s2 = &inptr[position->mHi];
                    s3 = &inptr[position->mHi + 1];
                    s4 = &inptr[position->mHi + 2];
                    s5 = &inptr[position->mHi + 3];
                    p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                    p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                    p2 = (float)(((s2->val[0] << 8) | (s2->val[1] << 16) | (s2->val[2] << 24)) >> 8) * scale;
                    p3 = (float)(((s3->val[0] << 8) | (s3->val[1] << 16) | (s3->val[2] << 24)) >> 8) * scale;
                    p4 = (float)(((s4->val[0] << 8) | (s4->val[1] << 16) | (s4->val[2] << 24)) >> 8) * scale;
                    p5 = (float)(((s5->val[0] << 8) | (s5->val[1] << 16) | (s5->val[2] << 24)) >> 8) * scale;

                    r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                        + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                        + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                        + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                        + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float r;
                        FMOD_INT24 * s0 = &inptr[((position->mHi - 2) * channels) + count];
                        FMOD_INT24 * s1 = &inptr[((position->mHi - 1) * channels) + count];
                        FMOD_INT24 * s2 = &inptr[(position->mHi * channels) + count];
                        FMOD_INT24 * s3 = &inptr[((position->mHi + 1) * channels) + count];
                        FMOD_INT24 * s4 = &inptr[((position->mHi + 2) * channels) + count];
                        FMOD_INT24 * s5 = &inptr[((position->mHi + 3) * channels) + count];
                        float p0 = (float)(((s0->val[0] << 8) | (s0->val[1] << 16) | (s0->val[2] << 24)) >> 8) * scale;
                        float p1 = (float)(((s1->val[0] << 8) | (s1->val[1] << 16) | (s1->val[2] << 24)) >> 8) * scale;
                        float p2 = (float)(((s2->val[0] << 8) | (s2->val[1] << 16) | (s2->val[2] << 24)) >> 8) * scale;
                        float p3 = (float)(((s3->val[0] << 8) | (s3->val[1] << 16) | (s3->val[2] << 24)) >> 8) * scale;
                        float p4 = (float)(((s4->val[0] << 8) | (s4->val[1] << 16) | (s4->val[2] << 24)) >> 8) * scale;
                        float p5 = (float)(((s5->val[0] << 8) | (s5->val[1] << 16) | (s5->val[2] << 24)) >> 8) * scale;

                        r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                            + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                            + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                            + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                            + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM32:
        {
            int * inptr = (int *)src;

            scale /= -2147483648.0f;

            if (channels == 1)
            {
                float f, r, p0, p1, p2, p3, p4, p5;

                while (outlength)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = (float)inptr[position->mHi - 2] * scale;
                    p1 = (float)inptr[position->mHi - 1] * scale;
                    p2 = (float)inptr[position->mHi] * scale;
                    p3 = (float)inptr[position->mHi + 1] * scale;
                    p4 = (float)inptr[position->mHi + 2] * scale;
                    p5 = (float)inptr[position->mHi + 3] * scale;

                    r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                        + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                        + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                        + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                        + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float r;
                        float p0 = (float)inptr[((position->mHi - 2) * channels) + count] * scale;
                        float p1 = (float)inptr[((position->mHi - 1) * channels) + count] * scale;
                        float p2 = (float)inptr[(position->mHi * channels) + count] * scale;
                        float p3 = (float)inptr[((position->mHi + 1) * channels) + count] * scale;
                        float p4 = (float)inptr[((position->mHi + 2) * channels) + count] * scale;
                        float p5 = (float)inptr[((position->mHi + 3) * channels) + count] * scale;

                        r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                            + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                            + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                            + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                            + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float * inptr = (float *)src;

            if (channels == 1)
            {
                float f, r, p0, p1, p2, p3, p4, p5;

                while (outlength)
                {
                    f = (float)position->mLo * (1.0f / 4294967296.0f);
                    p0 = inptr[position->mHi - 2];
                    p1 = inptr[position->mHi - 1];
                    p2 = inptr[position->mHi];
                    p3 = inptr[position->mHi + 1];
                    p4 = inptr[position->mHi + 2];
                    p5 = inptr[position->mHi + 3];

                    r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                        + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                        + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                        + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                        + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                    *out = r;
                    out++;
                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            else
            {
                while (outlength)
                {
                    int count;
                    float f = (float)position->mLo * (1.0f / 4294967296.0f);

                    for (count = 0; count < channels; count++)
                    {
                        float r;
                        float p0 = inptr[((position->mHi - 2) * channels) + count];
                        float p1 = inptr[((position->mHi - 1) * channels) + count];
                        float p2 = inptr[(position->mHi * channels) + count];
                        float p3 = inptr[((position->mHi + 1) * channels) + count];
                        float p4 = inptr[((position->mHi + 2) * channels) + count];
                        float p5 = inptr[((position->mHi + 3) * channels) + count];

                        r = p2 + 0.04166666666f * f * ((p3 - p1) * 16.0f + (p0 - p4) * 2.0f
                            + f * ((p3 + p1) * 16.0f - p0 - p2 * 30.0f - p4
                            + f * (p3 * 66.0f - p2 * 70.0f - p4 * 33.0f + p1 * 39.0f + p5 * 7.0f - p0 * 9.0f
                            + f * (p2 * 126.0f - p3 * 124.0f + p4 * 61.0f - p1 * 64.0f - p5 * 12.0f + p0 * 13.0f
                            + f * ((p3 - p2) * 50.0f + (p1 - p4) * 25.0f + (p5 - p0) * 5.0f)))));

                        *out = r;
                        out++;
                    }

                    position->mValue += speed->mValue;
                    outlength--;
                }
            }
            break;
        }
    }
}

}
