// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x8060F80C..0x80610154 (6 native functions: the constructor, threadFunc, start, stop,
// the destructor and the Thread-base threadFunc thunk 0x8061014C).
// Evidence: the constructor 0x8060F80C calls Output() 0x8060E934 and Thread() 0x806212BC for +0xD4 and
// installs 0x806EE834 at +0x10 and +0x1F4; threadFunc 0x8060F860 polls getposition/lock/mix/unlock;
// start 0x8060FEA4 starts "FMOD output polling thread" and computes the period from the buffer size and
// rate; stop 0x80610028 closes it.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; OutputPolled is the G2MEAB layout.

#include "fmod_output_polled.h"
#include "fmod.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"
#include "fmod_time.h"

namespace FMOD {

OutputPolled::OutputPolled()
{
    mFillBlock = 0;
}

FMOD_RESULT OutputPolled::threadFunc()
{
    FMOD_RESULT result;
    unsigned int pcm;
    unsigned int blocksize;
    int numblocks;
    FMOD_SOUND_FORMAT outputformat;
    int outputchannels;

    result = mSystem->getDSPBufferSize(&blocksize, &numblocks);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mSystem->getSoftwareFormat(0, &outputformat, &outputchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mDescription.getposition)
    {
        result = mDescription.getposition(this, &pcm);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mSystem->mDSPTimeStamp.stampIn();

    pcm /= blocksize;
    pcm %= numblocks;

    while (mFillBlock != pcm)
    {
        void * ptr1 = 0;
        void * ptr2 = 0;
        unsigned int len1 = 0;
        unsigned int len2 = 0;
        unsigned int offbytes;
        unsigned int lenbytes;
        unsigned int numsamples;

        result = SoundI::getBytesFromSamples(blocksize, &lenbytes, outputchannels, outputformat);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = SoundI::getBytesFromSamples(mFillBlock * blocksize, &offbytes, outputchannels, outputformat);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mDescription.lock)
        {
            result = mDescription.lock(this, offbytes, lenbytes, &ptr1, &ptr2, &len1, &len2);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = SoundI::getSamplesFromBytes(len1, &numsamples, outputchannels, outputformat);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mix(ptr1, numsamples);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mDescription.unlock)
        {
            result = mDescription.unlock(this, ptr1, ptr2, len1, len2);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        mFillBlock++;
        if (mFillBlock >= numblocks)
        {
            mFillBlock = 0;
        }
    }

    if (mFinishedSema)
    {
        FMOD_OS_Semaphore_Signal(mFinishedSema, false);
    }

    mSystem->mDSPTimeStamp.stampOut(95);

    return FMOD_OK;
}

FMOD_RESULT OutputPolled::start()
{
    FMOD_RESULT result;

    if (mPolledFromMainThread)
    {
        result = initThread("FMOD output polling thread", 0, 0, PRIORITY_CRITICAL, 0, 4096, true, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = FMOD_OS_Semaphore_Create(&mFinishedSema);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else
    {
        unsigned int blocksize;
        int rate;
        float ms;

        result = mSystem->getDSPBufferSize(&blocksize, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mSystem->getSoftwareFormat(&rate, 0, 0, 0, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        ms = 1000.0f * blocksize / rate;
        if (ms < 20.0f)
        {
            ms /= 3.0f;
            if (ms < 1.0f)
            {
                ms = 1.0f;
            }
        }
        else
        {
            ms = 10.0f;
        }

        result = initThread("FMOD output polling thread", 0, 0, PRIORITY_CRITICAL, 0, 4096, false, (int)ms);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT OutputPolled::stop()
{
    FMOD_RESULT result;

    result = closeThread();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mFinishedSema)
    {
        result = FMOD_OS_Semaphore_Free(mFinishedSema);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
