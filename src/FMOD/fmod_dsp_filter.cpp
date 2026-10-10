// G2MEAB fmod_dsp_filter.cpp: complete reconstruction (group D).
// .text: 0x805F69AC..0x805F70D0 (6 native functions): release 0x805F69AC, execute 0x805F69F0, startBuffering
// 0x805F6E68, getHistoryBuffer 0x805F6F54, stopBuffering 0x805F6F8C and the implicit deleting dtor 0x805F6FE0.

#include "fmod_dsp_filter.h"
#include "fmod.h"
#include "fmod_dsp_connection.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

FMOD_RESULT DSPFilter::release(bool freethis)
{
    stopBuffering();

    return DSPI::release(freethis);
}

FMOD_RESULT DSPFilter::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    FMOD_RESULT result = FMOD_OK;

    if (*length > mSystem->mDSPBlockSize)
    {
        *length = mSystem->mDSPBlockSize;
    }

    *outbuffer = inbuffer;
    *outchannels = inchannels;
    mIdle = true;

    if (!mVisited)
    {
        float * mixbuffer;
        float * readbuffer;
        int readbufferindex;
        int count;
        LinkedListNode * current;

        if (mOutputBuffer)
        {
            mixbuffer = mOutputBuffer;
        }
        else
        {
            mixbuffer = inbuffer;
        }

        readbufferindex = mSystem->mDSPReadBuffIndex;
        readbuffer = mSystem->mDSPReadBuff[readbufferindex];

        count = 0;
        current = mInputHead.getNext();
        if (current == &mInputHead)
        {
            memset(mixbuffer, 0, *length * inchannels * sizeof(float));
        }

        while (current != &mInputHead)
        {
            DSPConnection * connection = (DSPConnection *)current->getData();
            int newreadbufferindex;

            if (!connection->mInputUnit->mActive || connection->mInputUnit->mUnk63)
            {
                if (!count)
                {
                    memset(mixbuffer, 0, *length * inchannels * sizeof(float));
                }
                *outbuffer = mixbuffer;
                *outchannels = inchannels;
                connection->mInputUnit->mVisited = true;
                current = current->getNext();
                count++;
                continue;
            }

            newreadbufferindex = 1 - readbufferindex;
            mSystem->mDSPReadBuffIndex = newreadbufferindex;

            result = connection->mInputUnit->execute(readbuffer, outbuffer, length, inchannels, outchannels, speakermode);
            if (result != FMOD_OK)
            {
                break;
            }

            if (connection->mInputUnit->mIdle)
            {
                if (!count)
                {
                    memset(mixbuffer, 0, *length * inchannels * sizeof(float));
                }
                *outbuffer = mixbuffer;
                *outchannels = inchannels;
                connection->mInputUnit->mVisited = true;
                current = current->getNext();
                count++;
                continue;
            }

            mIdle = false;

            connection->updatePan(inchannels, *outchannels, speakermode);

            if (mNumInputs <= 1 && mNumOutputs <= 1 && connection->mVolume == 1.0f)
            {
                if ((!connection->mSetLevelsUsed && mDescription.mCategory != FMOD_DSP_CATEGORY_SOUNDCARD) || connection->checkUnity(*outchannels, inchannels) == FMOD_OK)
                {
                    connection->mInputUnit->mVisited = true;
                    break;
                }
            }

            if (!connection->mSetLevelsUsed)
            {
                connection->setPan(0.0f);
                connection->updatePan(inchannels, *outchannels, speakermode);
            }

            if (*outbuffer == mixbuffer)
            {
                if (mixbuffer == mSystem->mDSPReadBuff[0])
                {
                    mixbuffer = mSystem->mDSPReadBuff[1];
                }
                else
                {
                    mixbuffer = mSystem->mDSPReadBuff[0];
                }

                readbufferindex = newreadbufferindex;
                readbuffer = mSystem->mDSPReadBuff[readbufferindex];
            }

            if (!count)
            {
                memset(mixbuffer, 0, *length * inchannels * sizeof(float));
            }

            connection->mix(mixbuffer, *outbuffer, inchannels, *outchannels, *length);

            *outbuffer = mixbuffer;
            *outchannels = inchannels;
            connection->mInputUnit->mVisited = true;
            current = current->getNext();
            count++;
        }

        if (mDescription.read && !mBypass)
        {
            inchannels = *outchannels;

            if (mDescription.channels)
            {
                *outchannels = mDescription.channels;
                memset(inbuffer, 0, *length * mDescription.channels * sizeof(float));
            }
            else if (!mNumInputs)
            {
                memset(*outbuffer, 0, *length * inchannels * sizeof(float));
            }

            instance = (FMOD_DSP *)this;

            mDescription.read(this, *outbuffer, inbuffer, *length, inchannels, *outchannels);

            *outbuffer = inbuffer;
            mIdle = false;
        }

        float * historybuffer = mHistoryBuffer;

        if (historybuffer)
        {
            float * srcbuffer = *outbuffer;
            int len = *length;

            while (len)
            {
                unsigned int size = len;

                if (mHistoryPosition + len > mHistoryLength)
                {
                    size = mHistoryLength - mHistoryPosition;
                }

                memcpy(historybuffer + (mHistoryPosition * *outchannels), srcbuffer, size * *outchannels * sizeof(float));

                len -= size;
                srcbuffer += size * *outchannels;

                mHistoryPosition += size;
                if (mHistoryPosition >= mHistoryLength)
                {
                    mHistoryPosition = 0;
                }
            }
        }
    }
    else
    {
        *outbuffer = mOutputBuffer;
        mIdle = false;
    }

    return result;
}

FMOD_RESULT DSPFilter::startBuffering(unsigned int length)
{
    FMOD_RESULT result;
    int numoutputchannels;

    if (mHistoryBuffer && length == mHistoryLength)
    {
        return FMOD_OK;
    }

    result = mSystem->getSoftwareFormat(0, 0, &numoutputchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mHistoryBuffer)
    {
        FMOD_Memory_Free(mHistoryBuffer);
    }

    mHistoryLength = length;

    mHistoryBuffer = (float *)FMOD_Memory_Calloc(mHistoryLength * numoutputchannels * sizeof(float));
    if (!mHistoryBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    mHistoryPosition = 0;

    return FMOD_OK;
}

FMOD_RESULT DSPFilter::getHistoryBuffer(float * * buffer, unsigned int * position, unsigned int * length)
{
    if (buffer)
    {
        *buffer = mHistoryBuffer;
    }
    if (position)
    {
        *position = mHistoryPosition;
    }
    if (length)
    {
        *length = mHistoryLength;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFilter::stopBuffering()
{
    if (mHistoryBuffer)
    {
        FMOD_Memory_Free(mHistoryBuffer);
        mHistoryBuffer = 0;
    }

    return FMOD_OK;
}

} // namespace FMOD
