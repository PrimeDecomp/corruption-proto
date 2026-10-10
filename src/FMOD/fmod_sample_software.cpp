// G2MEAB prototype translation unit; reconstruction of all retained functions (setLoopPoints is
// dead-stripped and stays an empty placeholder).
// G2MEAB .text: 0x80611C3C..0x80612998 (8 retained native functions, including the weak Sample and
// SampleSoftware destructors).
// Evidence: the constructor 0x80611C3C calls Sample(), installs 0x806EECE4 and clears +0x37C/+0x380;
// release 0x80611D3C frees mBufferMemory with "fmod_sample_software.cpp" line 0x4F after
// SystemI::stopSound; lockInternal/unlockInternal 0x80611DE4/0x80611EDC restore and save the loop
// point data (0x80612514/0x80611F0C).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; SampleSoftware is the G2MEAB layout.

#include "fmod_sample_software.h"
#include "fmod.h"
#include "fmod_dsp_resampler.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

SampleSoftware::SampleSoftware()
{
    mBuffer = 0;
    mBufferMemory = 0;
}

FMOD_RESULT SampleSoftware::release()
{
    if (!mSystem)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_ERROR)
    {
        return FMOD_ERR_NOTREADY;
    }

    if (mSystem->stopSound(this) != FMOD_OK)
    {
        return FMOD_OK;
    }

    if (mBufferMemory)
    {
        FMOD_Memory_Free(mBufferMemory);
        mBufferMemory = 0;
    }
    mBuffer = 0;

    return Sample::release();
}

FMOD_RESULT SampleSoftware::setLoopPointData()
{
    FMOD_RESULT result;
    unsigned int overflowbytes;
    unsigned int pointA;
    unsigned int pointB;
    int count;

    if (mFormat != FMOD_SOUND_FORMAT_PCM8 && mFormat != FMOD_SOUND_FORMAT_PCM16 && mFormat != FMOD_SOUND_FORMAT_PCM24 &&
        mFormat != FMOD_SOUND_FORMAT_PCM32 && mFormat != FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        return FMOD_OK;
    }

    result = getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart, &pointA, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart + mLoopLength, &pointB, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mMode & FMOD_LOOP_BIDI)
    {
        memcpy(mLoopPointDataEndMemory, (char *)mBuffer + pointB, overflowbytes);

        switch (mFormat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                signed char * destptr = (signed char *)mBuffer + pointB;
                signed char * srcptr = destptr - mChannels;

                for (count = 0; count < mChannels * FMOD_DSP_RESAMPLER_OVERFLOWLENGTH; count++)
                {
                    *destptr++ = *srcptr--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short * destptr = (short *)((char *)mBuffer + pointB);
                short * srcptr = destptr - mChannels;

                for (count = 0; count < mChannels * FMOD_DSP_RESAMPLER_OVERFLOWLENGTH; count++)
                {
                    *destptr++ = *srcptr--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                unsigned char * destptr = (unsigned char *)mBuffer + pointB;
                unsigned char * srcptr = destptr - mChannels * 3;

                for (count = 0; count < mChannels * FMOD_DSP_RESAMPLER_OVERFLOWLENGTH; count++)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[1];
                    destptr[2] = srcptr[2];
                    destptr += 3;
                    srcptr -= 3;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                int * destptr = (int *)((char *)mBuffer + pointB);
                int * srcptr = destptr - mChannels;

                for (count = 0; count < mChannels * FMOD_DSP_RESAMPLER_OVERFLOWLENGTH; count++)
                {
                    *destptr++ = *srcptr--;
                }
                break;
            }
        }
    }
    else if (mMode & FMOD_LOOP_NORMAL)
    {
        memcpy(mLoopPointDataEndMemory, (char *)mBuffer + pointB, overflowbytes);
        memcpy((char *)mBuffer + pointB, (char *)mBuffer + pointA, overflowbytes);
    }

    return FMOD_OK;
}

FMOD_RESULT SampleSoftware::restoreLoopPointData()
{
    FMOD_RESULT result;
    unsigned int overflowbytes;
    unsigned int pointA;
    unsigned int pointB;

    if (mFormat != FMOD_SOUND_FORMAT_PCM8 && mFormat != FMOD_SOUND_FORMAT_PCM16 && mFormat != FMOD_SOUND_FORMAT_PCM24 &&
        mFormat != FMOD_SOUND_FORMAT_PCM32 && mFormat != FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        return FMOD_OK;
    }

    result = getBytesFromSamples(FMOD_DSP_RESAMPLER_OVERFLOWLENGTH, &overflowbytes, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart, &pointA, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }
    result = getBytesFromSamples(mLoopStart + mLoopLength, &pointB, mChannels, mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }

    memcpy((char *)mBuffer + pointB, mLoopPointDataEndMemory, overflowbytes);

    return FMOD_OK;
}

FMOD_RESULT SampleSoftware::lockInternal(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    FMOD_RESULT result;

    result = restoreLoopPointData();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (offset >= mLengthBytes)
    {
        *ptr1 = 0;
        if (ptr2)
        {
            *ptr2 = 0;
        }
        *len1 = 0;
        if (len2)
        {
            *len2 = 0;
        }
        return FMOD_ERR_INVALID_PARAM;
    }

    if (offset + length <= mLengthBytes)
    {
        *ptr1 = (char *)mBuffer + offset;
        *len1 = length;
        if (ptr2)
        {
            *ptr2 = 0;
        }
        if (len2)
        {
            *len2 = 0;
        }
    }
    else
    {
        *ptr1 = (char *)mBuffer + offset;
        *len1 = mLengthBytes - offset;
        *ptr2 = mBuffer;
        *len2 = length - (mLengthBytes - offset);
    }

    return FMOD_OK;
}

FMOD_RESULT SampleSoftware::unlockInternal(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    FMOD_RESULT result;

    result = setLoopPointData();
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT SampleSoftware::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
}

} // namespace FMOD
