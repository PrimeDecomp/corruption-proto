// G2MEAB prototype translation unit; all native functions reconstructed.
// G2MEAB .text: 0x80612B54..0x80615750 (14 native functions, including the weak SoundI destructor
// 0x80612BA0 emitted here).
// Evidence: Sample() 0x80612B54 derives SoundI 0x80616098, installs 0x806EEF18, clears mNumSubSamples
// and sets mLockCanRead. lock 0x80612C48 / unlock 0x80613F58 interleave the subsamples through the
// SystemI multi-subsample lock buffer (+0xE20/+0xE24); release 0x806151E0 frees that buffer, releases
// the subsamples and calls SoundI::release. The setters forward to SoundI and to each subsample.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; Sample is the G2MEAB layout.

#include "fmod_sound_sample.h"
#include "fmod.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

Sample::Sample()
{
    mNumSubSamples = 0;
    mLockCanRead = true;
}

FMOD_RESULT Sample::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    unsigned int lengthsamples;
    int count;

    if (!ptr1 || !len1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mNumSubSamples < 1)
    {
        return lockInternal(offset, length, ptr1, ptr2, len1, len2);
    }

    if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        return FMOD_ERR_FORMAT;
    }

    *ptr1 = mLockBuffer;
    if (ptr2)
    {
        *ptr2 = 0;
    }
    *len1 = length;
    if (len2)
    {
        *len2 = 0;
    }

    mLockLength = length;
    mLockOffset = offset;

    getSamplesFromBytes(length, &lengthsamples, mChannels, mFormat);

    length /= mNumSubSamples;
    offset /= mNumSubSamples;

    /*
        Interleave the subsamples into the lock buffer.
    */
    for (count = 0; count < mNumSubSamples; count++)
    {
        void * subptr1;
        void * subptr2;
        unsigned int sublen1;
        unsigned int sublen2;

        if (!mSubSample[count]->mLockCanRead)
        {
            continue;
        }

        mSubSample[count]->lock(offset, length, &subptr1, &subptr2, &sublen1, &sublen2);

        switch (mFormat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                unsigned char * destptr = (unsigned char *)*ptr1 + count;
                unsigned char * srcptr = (unsigned char *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[mNumSubSamples] = srcptr[1];
                    destptr[mNumSubSamples * 2] = srcptr[2];
                    destptr[mNumSubSamples * 3] = srcptr[3];
                    destptr[mNumSubSamples * 4] = srcptr[4];
                    destptr[mNumSubSamples * 5] = srcptr[5];
                    destptr[mNumSubSamples * 6] = srcptr[6];
                    destptr[mNumSubSamples * 7] = srcptr[7];
                    destptr += mNumSubSamples * 8;
                    srcptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    destptr += mNumSubSamples;
                    srcptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_GCADPCM:
            {
                lengthsamples = length >> 1;
                // fall through
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short * destptr = (short *)*ptr1 + count;
                short * srcptr = (short *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[mNumSubSamples] = srcptr[1];
                    destptr[mNumSubSamples * 2] = srcptr[2];
                    destptr[mNumSubSamples * 3] = srcptr[3];
                    destptr[mNumSubSamples * 4] = srcptr[4];
                    destptr[mNumSubSamples * 5] = srcptr[5];
                    destptr[mNumSubSamples * 6] = srcptr[6];
                    destptr[mNumSubSamples * 7] = srcptr[7];
                    destptr += mNumSubSamples * 8;
                    srcptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    destptr += mNumSubSamples;
                    srcptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                unsigned int sample;

                for (sample = 0; sample < lengthsamples; sample++)
                {
                    ((unsigned char *)*ptr1)[((count + (mNumSubSamples * sample)) * 3) + 0] = ((unsigned char *)subptr1)[(sample * 3) + 0];
                    ((unsigned char *)*ptr1)[((count + (mNumSubSamples * sample)) * 3) + 1] = ((unsigned char *)subptr1)[(sample * 3) + 1];
                    ((unsigned char *)*ptr1)[((count + (mNumSubSamples * sample)) * 3) + 2] = ((unsigned char *)subptr1)[(sample * 3) + 2];
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                int * destptr = (int *)*ptr1 + count;
                int * srcptr = (int *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[mNumSubSamples] = srcptr[1];
                    destptr[mNumSubSamples * 2] = srcptr[2];
                    destptr[mNumSubSamples * 3] = srcptr[3];
                    destptr[mNumSubSamples * 4] = srcptr[4];
                    destptr[mNumSubSamples * 5] = srcptr[5];
                    destptr[mNumSubSamples * 6] = srcptr[6];
                    destptr[mNumSubSamples * 7] = srcptr[7];
                    destptr += mNumSubSamples * 8;
                    srcptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    destptr += mNumSubSamples;
                    srcptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_IMAADPCM:
            {
                if (mNumSubSamples == 2)
                {
                    lengthsamples = length >> 2;

                    {
                        int * destptr = (int *)*ptr1 + count;
                        int * srcptr = (int *)subptr1;
                        unsigned int len;

                        for (len = lengthsamples >> 3; len; len--)
                        {
                            destptr[0] = srcptr[0];
                            destptr[mNumSubSamples] = srcptr[1];
                            destptr[mNumSubSamples * 2] = srcptr[2];
                            destptr[mNumSubSamples * 3] = srcptr[3];
                            destptr[mNumSubSamples * 4] = srcptr[4];
                            destptr[mNumSubSamples * 5] = srcptr[5];
                            destptr[mNumSubSamples * 6] = srcptr[6];
                            destptr[mNumSubSamples * 7] = srcptr[7];
                            destptr += mNumSubSamples * 8;
                            srcptr += 8;
                        }
                        for (len = lengthsamples & 7; len; len--)
                        {
                            *destptr = *srcptr;
                            destptr += mNumSubSamples;
                            srcptr++;
                        }
                    }
                    break;
                }
                // fall through
            }
            case FMOD_SOUND_FORMAT_VAG:
            {
                char * destptr = (char *)*ptr1;
                char * srcptr = (char *)subptr1;
                unsigned int blockalign;
                unsigned int numblocks;
                unsigned int block;

                SoundI::getBytesFromSamples(1, &blockalign, 1, mFormat);

                numblocks = length / blockalign;
                destptr += count * blockalign;

                for (block = 0; block < numblocks; block++)
                {
                    memcpy(destptr, srcptr, blockalign);
                    destptr += blockalign * mNumSubSamples;
                    srcptr += blockalign;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_XMA:
            {
                unsigned char * destptr = (unsigned char *)*ptr1 + count;
                unsigned char * srcptr = (unsigned char *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[mNumSubSamples] = srcptr[1];
                    destptr[mNumSubSamples * 2] = srcptr[2];
                    destptr[mNumSubSamples * 3] = srcptr[3];
                    destptr[mNumSubSamples * 4] = srcptr[4];
                    destptr[mNumSubSamples * 5] = srcptr[5];
                    destptr[mNumSubSamples * 6] = srcptr[6];
                    destptr[mNumSubSamples * 7] = srcptr[7];
                    destptr += mNumSubSamples * 8;
                    srcptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    destptr += mNumSubSamples;
                    srcptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_NONE:
            default:
            {
                return FMOD_ERR_FORMAT;
            }
        }

        mSubSample[count]->unlock(subptr1, subptr2, sublen1, sublen2);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::lockInternal(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    return FMOD_ERR_INTERNAL;
}

FMOD_RESULT Sample::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    unsigned int lengthsamples;
    unsigned int length;
    unsigned int offset;
    int count;

    if (!ptr1 || !len1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mNumSubSamples < 1)
    {
        return unlockInternal(ptr1, ptr2, len1, len2);
    }

    length = mLockLength;

    getSamplesFromBytes(length, &lengthsamples, mChannels, mFormat);

    length /= mNumSubSamples;
    offset = mLockOffset / mNumSubSamples;

    /*
        Deinterleave the lock buffer back into the subsamples.
    */
    for (count = 0; count < mNumSubSamples; count++)
    {
        void * subptr1;
        void * subptr2;
        unsigned int sublen1;
        unsigned int sublen2;

        mSubSample[count]->lock(offset, length, &subptr1, &subptr2, &sublen1, &sublen2);

        switch (mFormat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                unsigned char * srcptr = (unsigned char *)ptr1 + count;
                unsigned char * destptr = (unsigned char *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[mNumSubSamples];
                    destptr[2] = srcptr[mNumSubSamples * 2];
                    destptr[3] = srcptr[mNumSubSamples * 3];
                    destptr[4] = srcptr[mNumSubSamples * 4];
                    destptr[5] = srcptr[mNumSubSamples * 5];
                    destptr[6] = srcptr[mNumSubSamples * 6];
                    destptr[7] = srcptr[mNumSubSamples * 7];
                    srcptr += mNumSubSamples * 8;
                    destptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    srcptr += mNumSubSamples;
                    destptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_GCADPCM:
            {
                lengthsamples = length >> 1;
                // fall through
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short * srcptr = (short *)ptr1 + count;
                short * destptr = (short *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[mNumSubSamples];
                    destptr[2] = srcptr[mNumSubSamples * 2];
                    destptr[3] = srcptr[mNumSubSamples * 3];
                    destptr[4] = srcptr[mNumSubSamples * 4];
                    destptr[5] = srcptr[mNumSubSamples * 5];
                    destptr[6] = srcptr[mNumSubSamples * 6];
                    destptr[7] = srcptr[mNumSubSamples * 7];
                    srcptr += mNumSubSamples * 8;
                    destptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    srcptr += mNumSubSamples;
                    destptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                unsigned int sample;

                for (sample = 0; sample < lengthsamples; sample++)
                {
                    ((unsigned char *)subptr1)[(sample * 3) + 0] = ((unsigned char *)ptr1)[((count + (mNumSubSamples * sample)) * 3) + 0];
                    ((unsigned char *)subptr1)[(sample * 3) + 1] = ((unsigned char *)ptr1)[((count + (mNumSubSamples * sample)) * 3) + 1];
                    ((unsigned char *)subptr1)[(sample * 3) + 2] = ((unsigned char *)ptr1)[((count + (mNumSubSamples * sample)) * 3) + 2];
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                int * srcptr = (int *)ptr1 + count;
                int * destptr = (int *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[mNumSubSamples];
                    destptr[2] = srcptr[mNumSubSamples * 2];
                    destptr[3] = srcptr[mNumSubSamples * 3];
                    destptr[4] = srcptr[mNumSubSamples * 4];
                    destptr[5] = srcptr[mNumSubSamples * 5];
                    destptr[6] = srcptr[mNumSubSamples * 6];
                    destptr[7] = srcptr[mNumSubSamples * 7];
                    srcptr += mNumSubSamples * 8;
                    destptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    srcptr += mNumSubSamples;
                    destptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_IMAADPCM:
            {
                if (mNumSubSamples == 2)
                {
                    lengthsamples = length >> 2;

                    {
                        int * srcptr = (int *)ptr1 + count;
                        int * destptr = (int *)subptr1;
                        unsigned int len;

                        for (len = lengthsamples >> 3; len; len--)
                        {
                            destptr[0] = srcptr[0];
                            destptr[1] = srcptr[mNumSubSamples];
                            destptr[2] = srcptr[mNumSubSamples * 2];
                            destptr[3] = srcptr[mNumSubSamples * 3];
                            destptr[4] = srcptr[mNumSubSamples * 4];
                            destptr[5] = srcptr[mNumSubSamples * 5];
                            destptr[6] = srcptr[mNumSubSamples * 6];
                            destptr[7] = srcptr[mNumSubSamples * 7];
                            srcptr += mNumSubSamples * 8;
                            destptr += 8;
                        }
                        for (len = lengthsamples & 7; len; len--)
                        {
                            *destptr = *srcptr;
                            srcptr += mNumSubSamples;
                            destptr++;
                        }
                    }
                    break;
                }
                // fall through
            }
            case FMOD_SOUND_FORMAT_VAG:
            {
                char * destptr = (char *)subptr1;
                char * srcptr = (char *)ptr1;
                unsigned int blockalign;
                unsigned int numblocks;
                unsigned int block;

                SoundI::getBytesFromSamples(1, &blockalign, 1, mFormat);

                numblocks = length / blockalign;
                srcptr += count * blockalign;

                for (block = 0; block < numblocks; block++)
                {
                    memcpy(destptr, srcptr, blockalign);
                    destptr += blockalign;
                    srcptr += blockalign * mNumSubSamples;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_XMA:
            {
                unsigned char * srcptr = (unsigned char *)ptr1 + count;
                unsigned char * destptr = (unsigned char *)subptr1;
                unsigned int len;

                for (len = lengthsamples >> 3; len; len--)
                {
                    destptr[0] = srcptr[0];
                    destptr[1] = srcptr[mNumSubSamples];
                    destptr[2] = srcptr[mNumSubSamples * 2];
                    destptr[3] = srcptr[mNumSubSamples * 3];
                    destptr[4] = srcptr[mNumSubSamples * 4];
                    destptr[5] = srcptr[mNumSubSamples * 5];
                    destptr[6] = srcptr[mNumSubSamples * 6];
                    destptr[7] = srcptr[mNumSubSamples * 7];
                    srcptr += mNumSubSamples * 8;
                    destptr += 8;
                }
                for (len = lengthsamples & 7; len; len--)
                {
                    *destptr = *srcptr;
                    srcptr += mNumSubSamples;
                    destptr++;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_NONE:
            default:
            {
                return FMOD_ERR_FORMAT;
            }
        }

        mSubSample[count]->unlock(subptr1, subptr2, sublen1, sublen2);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::unlockInternal(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    return FMOD_OK;
}

FMOD_RESULT Sample::release()
{
    int count;

    if (mSystem)
    {
        mSystem->stopSound(this);

        if (mLockBuffer)
        {
            mSystem->mMultiSubSampleLockBuffer.free("", 0);
        }
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        if (mSubSample[count])
        {
            mSubSample[count]->mCodec = 0;
            mSubSample[count]->release();
            mSubSample[count] = 0;
        }
    }

    return SoundI::release();
}

FMOD_RESULT Sample::setDefaults(float frequency, float volume, float pan, int priority)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::setDefaults(frequency, volume, pan, priority);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->setDefaults(frequency, volume, pan, priority);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::setVariations(float frequencyvar, float volumevar, float panvar)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::setVariations(frequencyvar, volumevar, panvar);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->setVariations(frequencyvar, volumevar, panvar);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::set3DMinMaxDistance(float min, float max)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::set3DMinMaxDistance(min, max);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->set3DMinMaxDistance(min, max);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::set3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->set3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->setMode(mode);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::setLoopCount(int loopcount)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->setLoopCount(loopcount);
    }

    return FMOD_OK;
}

FMOD_RESULT Sample::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result;
    int count;

    result = SoundI::setLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumSubSamples; count++)
    {
        mSubSample[count]->setLoopPoints(loopstart, loopstarttype, loopend, loopendtype);
    }

    return FMOD_OK;
}

} // namespace FMOD
