// G2MEAB prototype translation unit; complete first-pass reconstruction.
// G2MEAB .text: 0x8061606C..0x80619D8C (52 native functions, including the weak ChannelStreamPool helper
// 0x80619BF8, __sinit and the weak MemSingleton destructor 0x80619CF0 for gReadBuffer).
// Evidence: validate 0x8061606C (errors 0x21/0x20); SoundI() 0x80616098 installs 0x806EF4DC and builds
// the embedded sync-point head (+0x1A8) and async node (+0x2C8); downmix 0x806161BC averages codec
// channels for readData; loadSubSound 0x80616938 calls SystemI::createSample and the codec soundcreate
// callback; read 0x80616B5C / seek 0x80617138 / clear 0x806171D4 drive lock/readData/unlock and
// Codec::setPosition; release 0x806175A8 tears down sync points, the codec, metadata, gReadBuffer,
// subsounds and the sentence list. setPositionInternal 0x80619BA0 is defined after setPosition.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; SoundI is the G2MEAB layout.

#include "fmod_soundi.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_channel_stream.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_metadata.h"
#include "fmod_os_misc.h"
#include "fmod_sound_sample.h"
#include "fmod_sound_stream.h"
#include "fmod_string.h"
#include "fmod_syncpoint.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

// Guessed name: the shared mono-downmix read buffer (allocated by loadSubSound for multichannel 3D
// subsounds, read by readData, released by release).
MemSingleton gReadBuffer;

FMOD_RESULT SoundI::validate(Sound * sound, SoundI * * soundi)
{
    if (!soundi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!sound)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *soundi = (SoundI *)sound;

    return FMOD_OK;
}

SoundI::SoundI()
{
    mMode = 0;
    mFormat = FMOD_SOUND_FORMAT_NONE;
    mChannels = 1;
    mMinDistance = 1.0f;
    mMaxDistance = 10000.0f;
    mConeInsideAngle = 360.0f;
    mConeOutsideAngle = 360.0f;
    mConeOutsideVolume = 1.0f;
    mDefaultVolume = 1.0f;
    mDefaultFrequency = (float)DEFAULT_FREQUENCY;
    mDefaultPan = 0.0f;
    mDefaultPriority = 128;
    mFrequencyVariation = 0.0f;
    mVolumeVariation = 0.0f;
    mPanVariation = 0.0f;
    mLoopStart = 0;
    mLoopLength = 0;
    mLoopCount = -1;
    mUserData = 0;
    mCodec = 0;
    mSubSound = 0;
    mSubSoundParent = 0;
    mSubSoundList = 0;
    mNumSubSounds = 0;
    mNumActiveSubSounds = 0;
    mAsyncThread = 0;
    mNumSyncPoints = 0;
    mOpenState = FMOD_OPENSTATE_READY;
    mAsyncResult = FMOD_OK;
    mAsyncNameData = 0;
    mExInfoExists = false;
    mMetadata = 0;
    mSubSampleParent = this;
    mRolloffPoint = 0;
    mNumRolloffPoints = 0;
    mPostReadCallback = 0;
    mPostSetPositionCallback = 0;
    mPostCallbackSound = 0;
}

FMOD_RESULT SoundI::downmix(void * dest, void * src, FMOD_SOUND_FORMAT format, int channels, unsigned int length)
{
    int count;
    int count2;

    switch (format)
    {
        case FMOD_SOUND_FORMAT_PCM8:
        {
            signed char * destptr = (signed char *)dest;
            signed char * srcptr = (signed char *)src;

            for (count = 0; count < (int)length; count++)
            {
                int total = 0;

                for (count2 = 0; count2 < mCodec->mWaveFormat.channels; count2++)
                {
                    total += srcptr[count * mCodec->mWaveFormat.channels + count2];
                }

                *destptr++ = (signed char)(total / mCodec->mWaveFormat.channels);
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM16:
        {
            short * destptr = (short *)dest;
            short * srcptr = (short *)src;

            for (count = 0; count < (int)length; count++)
            {
                int total = 0;

                for (count2 = 0; count2 < mCodec->mWaveFormat.channels; count2++)
                {
                    total += srcptr[count * mCodec->mWaveFormat.channels + count2];
                }

                *destptr++ = (short)(total / mCodec->mWaveFormat.channels);
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM24:
        {
            unsigned char * destptr = (unsigned char *)dest;
            unsigned char * srcptr = (unsigned char *)src;

            for (count = 0; count < (int)length; count++)
            {
                int total = 0;

                for (count2 = 0; count2 < mCodec->mWaveFormat.channels; count2++)
                {
                    unsigned char * sample = &srcptr[(count * mCodec->mWaveFormat.channels + count2) * 3];

                    total += ((sample[0] << 8) | (sample[1] << 16) | (sample[2] << 24)) >> 8;
                }

                total /= mCodec->mWaveFormat.channels;
                destptr[0] = (unsigned char)(total & 0xFF);
                destptr[1] = (unsigned char)((total >> 8) & 0xFF);
                destptr[2] = (unsigned char)((total >> 16) & 0xFF);
                destptr += 3;
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCM32:
        {
            int * destptr = (int *)dest;
            int * srcptr = (int *)src;

            for (count = 0; count < (int)length; count++)
            {
                int total = 0;

                for (count2 = 0; count2 < mCodec->mWaveFormat.channels; count2++)
                {
                    total += srcptr[count * mCodec->mWaveFormat.channels + count2];
                }

                *destptr++ = total / mCodec->mWaveFormat.channels;
            }
            break;
        }
        case FMOD_SOUND_FORMAT_PCMFLOAT:
        {
            float * destptr = (float *)dest;
            float * srcptr = (float *)src;

            for (count = 0; count < (int)length; count++)
            {
                float total = 0.0f;

                for (count2 = 0; count2 < mCodec->mWaveFormat.channels; count2++)
                {
                    total += srcptr[count * mCodec->mWaveFormat.channels + count2];
                }

                *destptr++ = total / mCodec->mWaveFormat.channels;
            }
            break;
        }
        default:
            break;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::loadSubSound(int index, FMOD_MODE mode)
{
    FMOD_RESULT result;
    Sample * sample = 0;

    if (!mNumSubSounds)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (index < 0 || index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->createSample(mode, &mCodec->waveformat[index], &sample);
    if (result != FMOD_OK)
    {
        return result;
    }

    sample->mType = mType;
    sample->mCodec = mCodec;

    if ((mode & FMOD_3D) && mCodec->waveformat[index].channels > 1)
    {
        gReadBuffer.alloc(SOUND_READCHUNKSIZE, "", 0);
    }

    if (mCodec->mDescription.soundcreate)
    {
        result = mCodec->mDescription.soundcreate(mCodec, index, (FMOD_SOUND *)sample);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mCodec->mPCMBufferOffsetBytes = 0;
    if (mCodec->mPCMBuffer)
    {
        memset(mCodec->mPCMBuffer, 0, mCodec->mPCMBufferLengthBytes);
    }

    result = mCodec->setPosition(index, 0, FMOD_TIMEUNIT_PCM);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mPostSetPositionCallback)
    {
        mPostSetPositionCallback((FMOD_SOUND *)this, index, 0, FMOD_TIMEUNIT_PCM);
    }

    if (!(mode & FMOD_OPENONLY))
    {
        result = sample->read(0, sample->mLength, 0);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }
    }

    mSubSound[index] = sample;

    result = sample->setPositionInternal(0);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::read(unsigned int offset, unsigned int numsamples, unsigned int * read)
{
    FMOD_RESULT result;
    unsigned int offsetbytes;
    unsigned int lengthbytes;
    unsigned int totalread;
    unsigned int chunksize;

    mPosition = offset;

    if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        offsetbytes = offset;
        lengthbytes = numsamples;
    }
    else
    {
        getBytesFromSamples(offset, &offsetbytes);
        getBytesFromSamples(numsamples, &lengthbytes);
    }

    totalread = 0;
    result = FMOD_OK;

    if (read)
    {
        *read = 0;
    }

    chunksize = SOUND_READCHUNKSIZE;
    if (mCodec->mWaveFormat.blockalign >= 1)
    {
        chunksize /= mCodec->mBlockAlign;
        chunksize *= mCodec->mBlockAlign;
    }

    while (lengthbytes)
    {
        void * ptr1;
        void * ptr2;
        unsigned int len1;
        unsigned int len2;
        unsigned int size;
        unsigned int bytesread = 0;

        size = lengthbytes > chunksize ? chunksize : lengthbytes;

        result = lock(offsetbytes, size, &ptr1, &ptr2, &len1, &len2);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (!len1 && !len2)
        {
            return FMOD_ERR_FILE_BAD;
        }

        if (ptr1 && len1)
        {
            unsigned int r = 0;

            result = readData(ptr1, len1, &r);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            bytesread = r;
        }

        if (ptr2 && len2)
        {
            unsigned int r = 0;

            result = readData(ptr2, len2, &r);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            bytesread += r;
        }

        if (result == FMOD_ERR_FILE_EOF)
        {
            lengthbytes = bytesread;
        }

        {
            FMOD_RESULT result2 = unlock(ptr1, ptr2, len1, len2);
            if (result2 != FMOD_OK)
            {
                return result2;
            }
        }

        lengthbytes -= bytesread;
        offsetbytes += bytesread;
        totalread += bytesread;
    }

    if (read)
    {
        getSamplesFromBytes(totalread, read);
    }

    return result;
}

FMOD_RESULT SoundI::seek(int subsound, unsigned int position)
{
    FMOD_RESULT result;

    if (!mCodec->mFile)
    {
        return FMOD_ERR_FILE_COULDNOTSEEK;
    }

    mPosition = position;

    result = mCodec->setPosition(subsound, position, FMOD_TIMEUNIT_PCM);

    if (mPostSetPositionCallback)
    {
        mPostSetPositionCallback((FMOD_SOUND *)this, subsound, position, FMOD_TIMEUNIT_PCM);
    }

    return result;
}

FMOD_RESULT SoundI::clear(unsigned int offset, unsigned int numsamples)
{
    FMOD_RESULT result;
    unsigned int offsetbytes;
    unsigned int lengthbytes;

    if (!numsamples)
    {
        return FMOD_OK;
    }

    getBytesFromSamples(offset, &offsetbytes);
    getBytesFromSamples(numsamples, &lengthbytes);

    while (lengthbytes)
    {
        void * ptr1;
        void * ptr2;
        unsigned int len1;
        unsigned int len2;
        unsigned int size;
        unsigned int cleared;

        size = lengthbytes > SOUND_READCHUNKSIZE ? SOUND_READCHUNKSIZE : lengthbytes;
        cleared = 0;

        result = lock(offsetbytes, size, &ptr1, &ptr2, &len1, &len2);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (ptr1 && len1)
        {
            memset(ptr1, 0, len1);
            cleared = len1;
        }

        if (ptr2 && len2)
        {
            memset(ptr2, 0, len2);
            cleared += len2;
        }

        result = unlock(ptr1, ptr2, len1, len2);
        if (result != FMOD_OK)
        {
            return result;
        }

        lengthbytes -= cleared;
        offsetbytes += cleared;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::release()
{
    FMOD_RESULT result;
    Codec * codec;

    if (mOpenState != FMOD_OPENSTATE_READY && mOpenState != FMOD_OPENSTATE_ERROR)
    {
        return FMOD_ERR_NOTREADY;
    }

    if (mSystem)
    {
        result = mSystem->stopSound(this);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    while (mNumSyncPoints)
    {
        deleteSyncPoint((FMOD_SYNCPOINT *)mSyncPointHead.getNext());
    }

    codec = mCodec;
    if (codec)
    {
        if (!mSubSoundParent || mSubSoundParent == this || (mSubSoundParent && codec != mSubSoundParent->mCodec))
        {
            if (isStream())
            {
                Stream * stream = (Stream *)this;

                FMOD_OS_CriticalSection_Enter(SystemI::gStreamListCrit);

                if (mSystem)
                {
                    mSystem->mStreamPool.remove();
                }

                if (stream->mSample)
                {
                    stream->mSample->mCodec = 0;
                    stream->mSample->release();
                    stream->mSample = 0;
                }

                mCodec->release();
                mCodec = 0;

                FMOD_OS_CriticalSection_Leave(SystemI::gStreamListCrit);
            }
            else
            {
                mCodec->release();
                mCodec = 0;
            }
        }
    }

    if (mMetadata)
    {
        mMetadata->release();
    }

    gReadBuffer.free("", 0);

    if (mNumSubSounds && mSubSound)
    {
        if (mNumActiveSubSounds)
        {
            int count;

            for (count = 0; count < mNumSubSounds; count++)
            {
                if (mSubSound[count])
                {
                    if (mSubSound[count]->mCodec == codec)
                    {
                        mSubSound[count]->mCodec = 0;
                    }

                    mSubSound[count]->release();
                    mSubSound[count] = 0;
                }
            }
        }

        FMOD_Memory_Free(mSubSound);
        mSubSound = 0;
    }

    if (mSubSoundParent)
    {
        int count;

        for (count = 0; count < mSubSoundParent->mNumSubSounds; count++)
        {
            if (mSubSoundParent->mSubSound[count] == this)
            {
                mSubSoundParent->setSubSound(count, 0);
                break;
            }
        }
    }

    if (mSubSoundList)
    {
        FMOD_Memory_Free(mSubSoundList);
        mSubSoundList = 0;
    }

    removeNode();

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT SoundI::getSystemObject(System * * system)
{
    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *system = (System *)mSystem;

    return FMOD_OK;
}

FMOD_RESULT SoundI::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    return FMOD_ERR_BADCOMMAND;
}

FMOD_RESULT SoundI::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    return FMOD_ERR_BADCOMMAND;
}

FMOD_RESULT SoundI::setDefaults(float frequency, float volume, float pan, int priority)
{
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    if (pan < -1.0f)
    {
        pan = -1.0f;
    }
    if (pan > 1.0f)
    {
        pan = 1.0f;
    }
    if (priority < 0)
    {
        priority = 0;
    }
    if (priority > 256)
    {
        priority = 256;
    }

    mDefaultFrequency = frequency;
    mDefaultVolume = volume;
    mDefaultPan = pan;
    mDefaultPriority = priority;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getDefaults(float * frequency, float * volume, float * pan, int * priority)
{
    if (frequency)
    {
        *frequency = mDefaultFrequency;
    }
    if (volume)
    {
        *volume = mDefaultVolume;
    }
    if (pan)
    {
        *pan = mDefaultPan;
    }
    if (priority)
    {
        *priority = mDefaultPriority;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::setVariations(float frequencyvar, float volumevar, float panvar)
{
    if (frequencyvar >= 0.0f)
    {
        mFrequencyVariation = frequencyvar;
    }
    if (volumevar >= 0.0f)
    {
        mVolumeVariation = volumevar;
    }
    if (panvar >= 0.0f)
    {
        mPanVariation = panvar;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getVariations(float * frequencyvar, float * volumevar, float * panvar)
{
    if (frequencyvar)
    {
        *frequencyvar = mFrequencyVariation;
    }
    if (volumevar)
    {
        *volumevar = mVolumeVariation;
    }
    if (panvar)
    {
        *panvar = mPanVariation;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::set3DMinMaxDistance(float min, float max)
{
    if (!(mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (min < 0.0f || max < 0.0f || max < min)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mMinDistance = min;
    mMaxDistance = max;

    return FMOD_OK;
}

FMOD_RESULT SoundI::get3DMinMaxDistance(float * min, float * max)
{
    if (!(mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (min)
    {
        *min = mMinDistance;
    }
    if (max)
    {
        *max = mMaxDistance;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    if (!(mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (insideconeangle < 0.0f)
    {
        insideconeangle = 0.0f;
    }
    if (outsideconeangle < 0.0f)
    {
        outsideconeangle = 0.0f;
    }
    if (insideconeangle > 360.0f)
    {
        insideconeangle = 360.0f;
    }
    if (outsideconeangle > 360.0f)
    {
        outsideconeangle = 360.0f;
    }
    if (outsidevolume < 0.0f)
    {
        outsidevolume = 0.0f;
    }
    if (outsidevolume > 1.0f)
    {
        outsidevolume = 1.0f;
    }

    mConeInsideAngle = insideconeangle;
    mConeOutsideAngle = outsideconeangle;
    mConeOutsideVolume = outsidevolume;

    return FMOD_OK;
}

FMOD_RESULT SoundI::get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
    if (!(mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (insideconeangle)
    {
        *insideconeangle = mConeInsideAngle;
    }
    if (outsideconeangle)
    {
        *outsideconeangle = mConeOutsideAngle;
    }
    if (outsidevolume)
    {
        *outsidevolume = mConeOutsideVolume;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::set3DCustomRolloff(FMOD_VECTOR * points, int numpoints)
{
    if (numpoints < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (points)
    {
        int count;

        for (count = 1; count < numpoints; count++)
        {
            if (points[count].x <= points[count - 1].x)
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            if (points[count].y < 0.0f || points[count].y > 1.0f)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
        }
    }

    mRolloffPoint = points;
    mNumRolloffPoints = numpoints;

    return FMOD_OK;
}

FMOD_RESULT SoundI::get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints)
{
    if (points)
    {
        *points = mRolloffPoint;
    }
    if (numpoints)
    {
        *numpoints = mNumRolloffPoints;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::setSubSound(int index, SoundI * subsound)
{
    if (index < 0 || index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (subsound && subsound->mSubSoundParent)
    {
        return FMOD_ERR_SUBSOUND_ALLOCATED;
    }

    if (isStream())
    {
        FMOD_OS_CriticalSection_Enter(SystemI::gStreamListCrit);
    }

    if (mSubSound[index])
    {
        mSubSound[index]->mSubSoundParent = 0;
        if (!subsound)
        {
            mNumActiveSubSounds--;
        }
    }
    else if (subsound)
    {
        mNumActiveSubSounds++;
    }

    mSubSound[index] = subsound;
    if (mSubSound[index])
    {
        mSubSound[index]->mSubSoundIndex = index;
    }

    if (mSubSoundListNum)
    {
        int count;

        mLength = 0;
        for (count = 0; count < mSubSoundListNum; count++)
        {
            SoundI * sound = mSubSound[mSubSoundList[count]];

            if (sound)
            {
                mLength += sound->mLength;
            }
        }
    }

    mLoopStart = 0;
    mLoopLength = mLength;

    if (subsound)
    {
        subsound->mSubSoundParent = this;
    }

    if (isStream())
    {
        ((Stream *)this)->mWantsToFlush = true;
        FMOD_OS_CriticalSection_Leave(SystemI::gStreamListCrit);
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getSubSound(int index, SoundI * * subsound)
{
    if (!subsound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *subsound = 0;

    if (index < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (index >= mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *subsound = mSubSound[index];

    return FMOD_OK;
}

FMOD_RESULT SoundI::setSubSoundSentence(int * subsoundlist, int numsubsounds)
{
    int count;

    if (!mNumSubSounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (subsoundlist && !numsubsounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!isStream())
    {
        return FMOD_ERR_FORMAT;
    }

    if (subsoundlist)
    {
        for (count = 0; count < numsubsounds; count++)
        {
            if (subsoundlist[count] < 0 || subsoundlist[count] >= mNumSubSounds || !mSubSound[subsoundlist[count]])
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            if (isStream() != mSubSound[subsoundlist[count]]->isStream())
            {
                return FMOD_ERR_FORMAT;
            }

            if (isStream())
            {
                SoundI * subsound = mSubSound[subsoundlist[count]];

                if (mFormat != subsound->mFormat)
                {
                    return FMOD_ERR_FORMAT;
                }

                if (mChannels != subsound->mChannels)
                {
                    return FMOD_ERR_FORMAT;
                }
            }
        }
    }

    if (mSubSoundList)
    {
        FMOD_Memory_Free(mSubSoundList);
    }

    mSubSoundListNum = numsubsounds;

    if (mSubSoundListNum)
    {
        mSubSoundList = (int *)FMOD_Memory_Calloc(mSubSoundListNum * sizeof(int));
        if (!mSubSoundList)
        {
            return FMOD_ERR_MEMORY;
        }

        mLength = 0;
        for (count = 0; count < mSubSoundListNum; count++)
        {
            if (subsoundlist)
            {
                mSubSoundList[count] = subsoundlist[count];
            }
            else
            {
                mSubSoundList[count] = count;
            }

            if (mSubSound[mSubSoundList[count]])
            {
                mSubSound[mSubSoundList[count]]->seekData(0);
                mLength += mSubSound[mSubSoundList[count]]->mLength;
            }
        }
    }

    mLoopStart = 0;
    mLoopLength = mLength;
    mSubSoundListCurrent = 0;

    if (isStream())
    {
        SoundI * subsound = mSubSound[mSubSoundList[mSubSoundListCurrent]];

        if (subsound)
        {
            Stream * stream = (Stream *)this;

            stream->mSample->mCodec = subsound->mCodec;
            stream->mWantsToFlush = true;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getName(char * name, int namelen)
{
    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (namelen > 256)
    {
        namelen = 256;
    }

    FMOD_strncpy(name, mName, namelen);

    return FMOD_OK;
}

FMOD_RESULT SoundI::getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
    if (!length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (lengthtype == FMOD_TIMEUNIT_SENTENCE)
    {
        *length = mSubSoundListNum;
    }
    else if (lengthtype == FMOD_TIMEUNIT_PCM)
    {
        *length = mLength;
    }
    else if (lengthtype == FMOD_TIMEUNIT_MS)
    {
        if (mDefaultFrequency == 0.0f)
        {
            *length = (unsigned int)-1;
        }
        else if (mLength == (unsigned int)-1)
        {
            *length = (unsigned int)-1;
        }
        else
        {
            *length = (unsigned int)((FMOD_UINT64)mLength * 1000 / (FMOD_UINT64)mDefaultFrequency);
        }
    }
    else if (lengthtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        if (mLength == (unsigned int)-1)
        {
            *length = (unsigned int)-1;
        }
        else
        {
            getBytesFromSamples(mLength, length);
        }
    }
    else if (mCodec)
    {
        return mCodec->getLength(length, lengthtype);
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getFormat(FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits)
{
    if (type)
    {
        *type = mType;
    }
    if (format)
    {
        *format = mFormat;
    }
    if (channels)
    {
        *channels = mChannels;
    }
    if (bits)
    {
        getBitsFromFormat(mFormat, bits);
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getNumSubSounds(int * numsubsounds)
{
    if (!numsubsounds)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numsubsounds = mNumSubSounds;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getNumTags(int * numtags, int * numtagsupdated)
{
    if (!numtags && !numtagsupdated)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numtags)
    {
        *numtags = 0;
    }
    if (numtagsupdated)
    {
        *numtagsupdated = 0;
    }

    if (mMetadata)
    {
        return mMetadata->getNumTags(numtags, numtagsupdated);
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getTag(const char * name, int index, FMOD_TAG * tag)
{
    if (!tag)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mMetadata)
    {
        return mMetadata->getTag(name, index, tag);
    }

    return FMOD_ERR_TAGNOTFOUND;
}

FMOD_RESULT SoundI::getOpenState(FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, bool * starving)
{
    if (openstate)
    {
        *openstate = mOpenState;
    }

    if (percentbuffered)
    {
        if (mCodec && mCodec->mFile && (mOpenState == FMOD_OPENSTATE_BUFFERING || mOpenState == FMOD_OPENSTATE_READY))
        {
            mCodec->mFile->isBusy(0, percentbuffered);
        }
        else
        {
            *percentbuffered = 0;
        }
    }

    if (mCodec && mCodec->mFile)
    {
        mCodec->mFile->isStarving(starving);
    }

    return mAsyncResult;
}

FMOD_RESULT SoundI::readData(void * buffer, unsigned int numbytes, unsigned int * read)
{
    FMOD_RESULT result;
    unsigned int offset;
    unsigned int totalread;
    unsigned int chunksize;
    int bits;
    int channelscale;

    if (!mCodec)
    {
        return FMOD_ERR_PLUGIN_MISSING;
    }

    if (!buffer)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getBitsFromFormat(mFormat, &bits);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mCodec)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (!mCodec->mFile)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    offset = 0;
    totalread = 0;

    channelscale = 1;
    if (mChannels != mCodec->mWaveFormat.channels && mChannels == 1)
    {
        channelscale = mCodec->mWaveFormat.channels;
    }

    chunksize = SOUND_READCHUNKSIZE;
    if (mCodec->mWaveFormat.blockalign)
    {
        chunksize /= mCodec->mWaveFormat.blockalign;
        chunksize *= mCodec->mWaveFormat.blockalign;
    }
    chunksize /= channelscale;

    while (numbytes)
    {
        unsigned int size;
        unsigned int bytesread;
        unsigned int samples;
        char * ptr;

        size = numbytes > chunksize ? chunksize : numbytes;
        bytesread = 0;
        ptr = (char *)buffer + offset;

        if (mMode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            result = mCodec->mFile->read(ptr, 1, size, &bytesread);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }
        }
        else if (mChannels != mCodec->mWaveFormat.channels && mChannels == 1)
        {
            void * readbuffer = gReadBuffer.getData();

            if (!bits)
            {
                return FMOD_ERR_FORMAT;
            }

            result = mCodec->read(readbuffer, size * channelscale, &bytesread);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            downmix(ptr, readbuffer, mFormat, mCodec->mWaveFormat.channels, bytesread * 8 / bits / mCodec->mWaveFormat.channels);
        }
        else
        {
            result = mCodec->read(ptr, size, &bytesread);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }
        }

        if (mPostReadCallback && bytesread)
        {
            mPostReadCallback(mPostCallbackSound, ptr, bytesread);
        }

        bytesread /= channelscale;

        if (result == FMOD_ERR_FILE_EOF)
        {
            numbytes = bytesread;
        }

        numbytes -= bytesread;
        offset += bytesread;
        totalread += bytesread;

        getSamplesFromBytes(bytesread, &samples);

        mPosition += samples;
        if (mPosition > mLength)
        {
            mPosition = mLength;
        }
    }

    if (read)
    {
        *read = totalread;
    }

    return result;
}

FMOD_RESULT SoundI::seekData(unsigned int position)
{
    return seek(mSubSoundIndex, position);
}

FMOD_RESULT SoundI::getNumSyncPoints(int * numsyncpoints)
{
    if (!numsyncpoints)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numsyncpoints = mNumSyncPoints;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getSyncPoint(int index, FMOD_SYNCPOINT * * point)
{
    SyncPoint * current;
    int count;

    if (index < 0 || index >= mNumSyncPoints || !point)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = (SyncPoint *)mSyncPointHead.getNext();
    count = 0;
    while (1)
    {
        if (count >= index)
        {
            *point = (FMOD_SYNCPOINT *)current;
            break;
        }

        count++;
        current = (SyncPoint *)current->getNext();
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getSyncPointInfo(FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype)
{
    SyncPoint * syncpoint = (SyncPoint *)point;

    if (name)
    {
        FMOD_strncpy(name, syncpoint->mName, namelen);
    }

    if (!offset)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (offsettype == FMOD_TIMEUNIT_PCM)
    {
        *offset = syncpoint->mOffset;
    }
    else if (offsettype == FMOD_TIMEUNIT_PCMBYTES)
    {
        getBytesFromSamples(syncpoint->mOffset, offset);
    }
    else if (offsettype == FMOD_TIMEUNIT_MS)
    {
        *offset = (unsigned int)(1000.0f * syncpoint->mOffset / mDefaultFrequency);
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::addSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * syncpoint)
{
    SyncPoint * point;
    unsigned int pcm;

    point = FMOD_Object_Alloc(SyncPoint);
    if (!point)
    {
        return FMOD_ERR_MEMORY;
    }

    if (syncpoint)
    {
        *syncpoint = (FMOD_SYNCPOINT *)point;
    }

    if (offsettype == FMOD_TIMEUNIT_PCM)
    {
        pcm = offset;
    }
    else if (offsettype == FMOD_TIMEUNIT_PCMBYTES)
    {
        getSamplesFromBytes(offset, &pcm);
    }
    else if (offsettype == FMOD_TIMEUNIT_MS)
    {
        pcm = (unsigned int)((float)offset / 1000.0f * mDefaultFrequency);
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(point->mName, name, 256);
    point->mOffset = pcm;
    point->addAt(&mSyncPointHead, pcm);

    mNumSyncPoints++;

    return FMOD_OK;
}

FMOD_RESULT SoundI::deleteSyncPoint(FMOD_SYNCPOINT * point)
{
    SyncPoint * syncpoint = (SyncPoint *)point;

    if (!point)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    syncpoint->removeNode();
    FMOD_Memory_Free(syncpoint);

    mNumSyncPoints--;

    return FMOD_OK;
}

FMOD_RESULT SoundI::setMode(FMOD_MODE mode)
{
    if (mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);

        if (mode & FMOD_LOOP_OFF)
        {
            mMode |= FMOD_LOOP_OFF;
        }
        else if (mode & FMOD_LOOP_NORMAL)
        {
            mMode |= FMOD_LOOP_NORMAL;

            if (isStream())
            {
                Stream * stream = (Stream *)this;

                if (!stream)
                {
                    return FMOD_ERR_INTERNAL;
                }

                stream->mFinished = false;
            }
        }
        else if (mode & FMOD_LOOP_BIDI)
        {
            mMode |= FMOD_LOOP_BIDI;

            if (isStream())
            {
                Stream * stream = (Stream *)this;

                if (!stream)
                {
                    return FMOD_ERR_INTERNAL;
                }

                stream->mFinished = false;
            }
        }
    }

    if (mode & FMOD_3D_HEADRELATIVE)
    {
        mMode &= ~FMOD_3D_WORLDRELATIVE;
        mMode |= FMOD_3D_HEADRELATIVE;
    }
    else if (mode & FMOD_3D_WORLDRELATIVE)
    {
        mMode &= ~FMOD_3D_HEADRELATIVE;
        mMode |= FMOD_3D_WORLDRELATIVE;
    }

    if (mode & FMOD_3D_LOGROLLOFF)
    {
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |= FMOD_3D_LOGROLLOFF;
    }
    else if (mode & FMOD_3D_LINEARROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |= FMOD_3D_LINEARROLLOFF;
    }
    else if (mode & FMOD_3D_CUSTOMROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode |= FMOD_3D_CUSTOMROLLOFF;
    }

    if (mode & FMOD_2D)
    {
        mMode &= ~FMOD_3D;
        mMode |= FMOD_2D;
    }
    else if (mode & FMOD_3D)
    {
        mMode &= ~FMOD_2D;
        mMode |= FMOD_3D;
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::getMode(FMOD_MODE * mode)
{
    if (!mode)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *mode = mMode;

    return FMOD_OK;
}

FMOD_RESULT SoundI::setLoopCount(int loopcount)
{
    FMOD_RESULT result;

    if (!loopcount)
    {
        result = setMode(FMOD_LOOP_OFF);
    }
    else
    {
        result = setMode(FMOD_LOOP_NORMAL);
    }
    if (result != FMOD_OK)
    {
        return result;
    }

    mLoopCount = loopcount;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getLoopCount(int * loopcount)
{
    if (!loopcount)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *loopcount = mLoopCount;

    return FMOD_OK;
}

FMOD_RESULT SoundI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    unsigned int loopstartpcm = 0;
    unsigned int loopendpcm = 0;

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) ||
        (loopendtype != FMOD_TIMEUNIT_MS && loopendtype != FMOD_TIMEUNIT_PCM && loopendtype != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (loopstarttype == FMOD_TIMEUNIT_PCM)
    {
        loopstartpcm = loopstart;
    }
    else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
    {
        getSamplesFromBytes(loopstart, &loopstartpcm);
    }
    else if (loopstarttype == FMOD_TIMEUNIT_MS)
    {
        loopstartpcm = (unsigned int)((float)loopstart / 1000.0f * mDefaultFrequency);
    }

    if (loopendtype == FMOD_TIMEUNIT_PCM)
    {
        loopendpcm = loopend;
    }
    else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        getSamplesFromBytes(loopend, &loopendpcm);
    }
    else if (loopendtype == FMOD_TIMEUNIT_MS)
    {
        loopendpcm = (unsigned int)((float)loopend / 1000.0f * mDefaultFrequency);
    }

    if (loopstartpcm >= mLength || loopendpcm >= mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!loopendpcm)
    {
        loopendpcm = mLength - 1;
    }

    if (loopstartpcm >= loopendpcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mLoopStart = loopstartpcm;
    mLoopLength = loopendpcm - loopstartpcm + 1;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) ||
        (loopendtype != FMOD_TIMEUNIT_MS && loopendtype != FMOD_TIMEUNIT_PCM && loopendtype != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (loopstart)
    {
        if (loopstarttype == FMOD_TIMEUNIT_PCM)
        {
            *loopstart = mLoopStart;
        }
        else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
        {
            getBytesFromSamples(mLoopStart, loopstart);
        }
        else if (loopstarttype == FMOD_TIMEUNIT_MS)
        {
            *loopstart = (unsigned int)(1000.0f * mLoopStart / mDefaultFrequency);
        }
    }

    if (loopend)
    {
        unsigned int end = mLoopStart + mLoopLength - 1;

        if (loopendtype == FMOD_TIMEUNIT_PCM)
        {
            *loopend = end;
        }
        else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
        {
            getBytesFromSamples(end, loopend);
        }
        else if (loopendtype == FMOD_TIMEUNIT_MS)
        {
            *loopend = (unsigned int)(1000.0f * end / mDefaultFrequency);
        }
    }

    return FMOD_OK;
}

FMOD_RESULT SoundI::setPosition(unsigned int pos)
{
    return setPositionInternal(pos);
}

FMOD_RESULT SoundI::setPositionInternal(unsigned int pcm)
{
    mPosition = pcm;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getPosition(unsigned int * pcm)
{
    if (!pcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *pcm = mPosition;

    return FMOD_OK;
}

FMOD_RESULT SoundI::setUserData(void * userdata)
{
    mUserData = userdata;

    return FMOD_OK;
}

FMOD_RESULT SoundI::getUserData(void * * userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mUserData;

    return FMOD_OK;
}

// Inline, emitted weak after getUserData (0x80619BF8). The native file string is "fmod_freelist.h"
// line 94, so the original is a free-list template header; held here until that header is recovered.
inline FMOD_RESULT ChannelStreamPool::remove()
{
    ChannelStream * channel;

    if (mFreeHead.isEmpty())
    {
        return FMOD_ERR_MEMORY;
    }

    channel = (ChannelStream *)mFreeHead.getNext();
    channel->LinkedListNode::removeNode();

    FMOD_Memory_Free(channel);

    return FMOD_OK;
}

} // namespace FMOD
