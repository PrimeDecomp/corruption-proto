// G2MEAB fmod_dsp_codec.cpp: partial reconstruction (group D).
// .text: 0x806281C4..0x80628C88 (19 native functions): getDescriptionEx, the *Internal methods, the seven
// FMOD_DSP_STATE callbacks, __sinit (dspcodec description), execute 0x80628648 (still a placeholder),
// addInput 0x80628B1C and the implicit deleting dtor 0x80628B78.

#include "fmod_dsp_codec.h"
#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_codeci.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspcodec;

FMOD_DSP_DESCRIPTION_EX * DSPCodec::getDescriptionEx()
{
    memset(&dspcodec, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspcodec.name, "FMOD DSP Codec");
    dspcodec.version = 0x00010100;
    dspcodec.create = DSPCodec::createCallback;
    dspcodec.release = DSPCodec::releaseCallback;
    dspcodec.reset = DSPCodec::resetCallback;
    dspcodec.read = DSPCodec::readCallback;
    dspcodec.setposition = DSPCodec::setPositionCallback;

    dspcodec.numparameters = 0;
    dspcodec.paramdesc = 0;
    dspcodec.setparameter = DSPCodec::setParameterCallback;
    dspcodec.getparameter = DSPCodec::getParameterCallback;

    dspcodec.mType = (FMOD_DSP_TYPE)FMOD_DSP_TYPE_CODECREADER;
    dspcodec.mCategory = FMOD_DSP_CATEGORY_DSPCODEC;
    dspcodec.mSize = sizeof(DSPCodec);

    return &dspcodec;
}

FMOD_RESULT DSPCodec::createInternal()
{
    init();

    return FMOD_OK;
}

FMOD_RESULT DSPCodec::releaseInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPCodec::resetInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPCodec::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    FMOD_RESULT result;
    unsigned int endpos;

    while (length)
    {
        unsigned int toread;
        unsigned int bytesread;

        if ((mChannel->mMode & FMOD_LOOP_NORMAL) && mChannel->mLoopCount)
        {
            endpos = mChannel->mLoopStart + mChannel->mLoopLength - 1;
        }
        else if (mCodec->mAccurateLength)
        {
            endpos = mChannel->mLength - 1;
        }
        else
        {
            endpos = (unsigned int)-1;
        }

        toread = length;
        if (mPosition > endpos)
        {
            toread = 0;
        }
        else if (mPosition + length > endpos)
        {
            toread = endpos - mPosition + 1;
        }

        result = mCodec->read(outbuffer, toread * inchannels * sizeof(float), &bytesread);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        bytesread /= sizeof(float);
        bytesread /= inchannels;

        outbuffer += bytesread * inchannels;
        mPosition += bytesread;
        length -= bytesread;

        if (result == FMOD_ERR_FILE_EOF || mPosition > endpos)
        {
            if ((mChannel->mMode & FMOD_LOOP_NORMAL) && mChannel->mLoopCount)
            {
                DSPI::setPosition(mChannel->mLoopStart);

                if (mChannel->mLoopCount > 0)
                {
                    mChannel->mLoopCount--;
                }
            }
            else
            {
                memset(outbuffer, 0, length * inchannels * sizeof(float));
                return result;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPCodec::setPositionInternal(unsigned int position)
{
    mPosition = position;

    return mCodec->setPosition(0, position, FMOD_TIMEUNIT_PCM);
}

FMOD_RESULT DSPCodec::setParameterInternal(int index, float value)
{
    return FMOD_OK;
}

FMOD_RESULT DSPCodec::getParameterInternal(int index, float * value, char * valuestr)
{
    return FMOD_OK;
}

FMOD_RESULT DSPCodec::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->createInternal();
}

FMOD_RESULT DSPCodec::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->releaseInternal();
}

FMOD_RESULT DSPCodec::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->resetInternal();
}

FMOD_RESULT DSPCodec::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPCodec::setPositionCallback(FMOD_DSP_STATE * dsp, unsigned int pos)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->setPositionInternal(pos);
}

FMOD_RESULT DSPCodec::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->setParameterInternal(index, value);
}

FMOD_RESULT DSPCodec::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPCodec * dspcodec = (DSPCodec *)dsp;

    return dspcodec->getParameterInternal(index, value, valuestr);
}

FMOD_RESULT DSPInputResampler::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    FMOD_RESULT result = FMOD_OK;
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
        int readbufferindex;

        out = mOutputBuffer;
        if (!out)
        {
            return FMOD_ERR_INTERNAL;
        }

        readbufferindex = mSystem->mDSPReadBuffIndex;

        do
        {
            int finished = 0;
            unsigned int outlength;

            while (mFill)
            {
                unsigned int resamplebufferpos = mResampleBufferPos;
                unsigned int blocklength = mResampleBlockLength;
                float * buffer = (float *)mResampleBuffer + (mResampleBufferPos * mResampleBufferChannels);
                float * readbuffer;
                int readchannels;

                resetVisited();

                mSystem->mDSPReadBuffIndex = readbufferindex;

                result = DSPFilter::execute(buffer, &readbuffer, &blocklength, inchannels, &readchannels, speakermode);
                if (result != FMOD_OK)
                {
                    readbuffer = buffer;
                    memset(buffer, 0, blocklength * mResampleBufferChannels * sizeof(float));
                    mResampleFinishPos = mResampleBufferPos;
                }

                mResampleBufferChannels = readchannels;

                if (readbuffer != buffer)
                {
                    memcpy(buffer, readbuffer, blocklength * mResampleBufferChannels * sizeof(float));
                }

                mResampleBufferPos += blocklength;
                if (mResampleBufferPos >= mResampleBufferLength)
                {
                    mResampleBufferPos = 0;
                }

                if (!resamplebufferpos)
                {
                    unsigned int count;

                    for (count = 0; count < mResampleBufferChannels * mOverflowLength * 2; count++)
                    {
                        ((float *)mResampleBuffer)[(mResampleBufferLength * mResampleBufferChannels) + count] = ((float *)mResampleBuffer)[count];
                    }
                }

                mFill--;
            }

            outlength = len;

            if (mSpeed.mValue > 0x100)
            {
                FMOD_UINT64P samplesleft;
                FMOD_UINT64 remainder;

                samplesleft.mHi = ((((int)(mResamplePosition.mHi - mOverflowLength) / (int)mResampleBlockLength) + 1) * mResampleBlockLength) + mOverflowLength;
                samplesleft.mLo = 0;
                samplesleft.mValue -= mResamplePosition.mValue;

                remainder = samplesleft.mValue % mSpeed.mValue;
                samplesleft.mValue /= mSpeed.mValue;
                if (remainder)
                {
                    samplesleft.mValue++;
                }

                if (samplesleft.mValue <= outlength)
                {
                    finished = 1;
                    outlength = samplesleft.mLo;
                }
            }

            if (mSpeed.mHi == 1 && mSpeed.mLo == 0)
            {
                memcpy(out + (outpos * mResampleBufferChannels), (float *)mResampleBuffer + (mResamplePosition.mHi * mResampleBufferChannels), outlength * sizeof(float) * mResampleBufferChannels);

                mResamplePosition.mValue += mSpeed.mValue * outlength;
            }
            else
            {
                switch (mSystem->mResampleMethod)
                {
                    case 0:
                    {
                        FMOD_Resampler_NoInterp(out + (outpos * mResampleBufferChannels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mResampleBufferChannels);
                        break;
                    }
                    case 1:
                    {
                        FMOD_Resampler_Linear(out + (outpos * mResampleBufferChannels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mResampleBufferChannels);
                        break;
                    }
                    case 2:
                    {
                        FMOD_Resampler_Cubic(out + (outpos * mResampleBufferChannels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mResampleBufferChannels);
                        break;
                    }
                    case 3:
                    {
                        FMOD_Resampler_Spline(out + (outpos * mResampleBufferChannels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mResampleBufferChannels);
                        break;
                    }
                    default:
                    {
                        FMOD_Resampler_Linear(out + (outpos * mResampleBufferChannels), outlength, mResampleBuffer, FMOD_SOUND_FORMAT_PCMFLOAT, &mResamplePosition, &mSpeed, mResampleBufferChannels);
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

        channels = mResampleBufferChannels;
    }
    else
    {
        out = mOutputBuffer;
    }

    *outbuffer = out;
    *outchannels = channels;

    return result;
}

FMOD_RESULT DSPInputResampler::addInput(DSPI * target)
{
    FMOD_RESULT result;

    result = DSPI::addInput(target);
    if (result != FMOD_OK)
    {
        return result;
    }

    mResampleBufferPos = 0;
    mFill = 2;
    DSPResampler::mPosition.mValue = 0;
    mResamplePosition.mValue = 0;

    return FMOD_OK;
}

} // namespace FMOD
