// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x80615750..0x8061606C (8 native functions: the constructor, fill, flush, setPosition,
// getPosition, setLoopCount, the destructor and isStream).
// Evidence: Stream() 0x80615750 derives SoundI 0x80616098, installs 0x806EF030 and initializes the
// stream state; fill 0x806157B0, flush 0x80615AD8, setPosition 0x80615B1C and getPosition 0x80615D20
// follow the 4.06 control flow (SoundI::read/seek/clear on mSample, Codec::setPosition/getPosition).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; Stream is the G2MEAB layout.

#include "fmod_sound_stream.h"
#include "fmod.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_sound_sample.h"

#include <string.h>

namespace FMOD {

Stream::Stream()
{
    mLastPos = 0;
    mSubSound = 0;
    mLoopCountCurrent = -1;
    mBlockSize = 1;
    mFinished = false;
    mWantsToFlush = false;
}

FMOD_RESULT Stream::fill(unsigned int offset, unsigned int length)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int read = 0;

    if (!mFinished)
    {
        unsigned int len = length;

        do
        {
            unsigned int r;
            unsigned int size;
            unsigned int endpoint;
            Stream * stream = this;

            if (mSubSound)
            {
                stream = (Stream *)mSubSound[mSubSoundList[mSubSoundListCurrent]];
                if (!stream)
                {
                    break;
                }
            }

            if ((mMode & FMOD_LOOP_NORMAL) && mLoopCountCurrent)
            {
                endpoint = stream->mLoopStart + stream->mLoopLength - 1;
            }
            else if (mSample->mCodec->mAccurateLength)
            {
                endpoint = stream->mLength - 1;
            }
            else
            {
                endpoint = (unsigned int)-1;
            }

            size = len;
            if (offset + len > mSample->mLength)
            {
                size = mSample->mLength - offset;
            }

            if (stream->mPosition > endpoint)
            {
                size = 0;
            }
            else if (stream->mPosition + size > endpoint)
            {
                size = endpoint - stream->mPosition + 1;
            }

            result = mSample->read(offset, size, &r);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            read += r;
            len -= r;
            offset += r;
            if (offset >= mSample->mLength)
            {
                offset = 0;
            }

            stream->mLastPos = stream->mPosition;
            stream->mPosition += r;

            if (stream->mPosition > endpoint || result == FMOD_ERR_FILE_EOF)
            {
                int finished = 1;

                if (mSubSoundList)
                {
                    SoundI * subsound;

                    finished = 0;
                    do
                    {
                        mSubSoundListCurrent++;
                        if (mSubSoundListCurrent >= mSubSoundListNum)
                        {
                            finished = 1;
                            mSubSoundListCurrent = 0;
                        }

                        mSubSoundIndex = mSubSoundList[mSubSoundListCurrent];
                        subsound = mSubSound[mSubSoundIndex];
                    } while (!subsound && !finished);

                    if (subsound)
                    {
                        mSample->mCodec = subsound->mCodec;
                        if (!finished)
                        {
                            result = mSample->seek(mSubSoundIndex, 0);
                        }
                        subsound->mPosition = 0;
                    }
                }

                if (finished)
                {
                    if ((mMode & FMOD_LOOP_NORMAL) && mLoopCountCurrent)
                    {
                        mPosition = mLoopStart;
                        if (mLength != (unsigned int)-1)
                        {
                            result = mSample->seek(mSubSoundIndex, mPosition);
                            if (result != FMOD_OK)
                            {
                                return result;
                            }
                        }

                        if (mLoopCountCurrent > 0)
                        {
                            mLoopCountCurrent--;
                        }
                    }
                    else
                    {
                        mPosition = mLength;
                        mFinished = true;
                        break;
                    }
                }
            }
            else if (!r)
            {
                break;
            }
        } while (len);
    }

    if (read < length)
    {
        unsigned int len = length - read;

        do
        {
            unsigned int size = len;

            if (offset + len > mSample->mLength)
            {
                size = mSample->mLength - offset;
            }

            mSample->clear(offset, size);

            read += size;
            len -= size;
            offset += size;
            if (offset >= mSample->mLength)
            {
                offset = 0;
            }
        } while (len);
    }

    return result;
}

FMOD_RESULT Stream::flush()
{
    FMOD_RESULT result;

    result = fill(0, mSample->mLength);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT Stream::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    bool seekable = true;
    unsigned int endpoint;

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        if (mMode & FMOD_LOOP_OFF)
        {
            endpoint = mLength - 1;
        }
        else
        {
            endpoint = mLoopStart + mLoopLength - 1;
        }

        if (position > endpoint)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    if (mCodec->mFile)
    {
        seekable = mCodec->mFile->mSeekable;
    }

    mFinished = false;

    if (seekable)
    {
        if (mSubSound && mSubSoundList && postype == FMOD_TIMEUNIT_PCM)
        {
            int count;
            unsigned int offset = 0;

            for (count = 0; count < mSubSoundListNum; count++)
            {
                SoundI * subsound = mSubSound[mSubSoundList[count]];

                if (subsound)
                {
                    if (position >= offset && position < offset + subsound->mLength)
                    {
                        Stream * substream = (Stream *)subsound;

                        mSubSoundListCurrent = count;
                        mSubSoundIndex = mSubSoundList[mSubSoundListCurrent];
                        substream->setPosition(position - offset, postype);
                        break;
                    }

                    offset += subsound->mLength;
                }
            }
        }
        else
        {
            mCodec->mPCMBufferOffsetBytes = 0;
            if (mCodec->mPCMBuffer)
            {
                memset(mCodec->mPCMBuffer, 0, mCodec->mPCMBufferLengthBytes);
            }

            result = mCodec->setPosition(mSubSoundIndex, position, postype);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        if (mSample->mPostSetPositionCallback)
        {
            mSample->mPostSetPositionCallback((FMOD_SOUND *)this, mSubSoundIndex, position, postype);
        }

        if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
        {
            position = 0;
        }

        mPosition = position;
        mLastPos = position;
    }
    else if (mLastPos || position)
    {
        return FMOD_ERR_FILE_COULDNOTSEEK;
    }

    return FMOD_OK;
}

FMOD_RESULT Stream::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;

    if (mOpenState != FMOD_OPENSTATE_READY)
    {
        return FMOD_ERR_NOTREADY;
    }

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == (FMOD_TIMEUNIT_SENTENCE_SUBSOUND | FMOD_TIMEUNIT_BUFFERED))
    {
        *position = mSubSoundListCurrent;
    }
    else if (postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES || postype == FMOD_TIMEUNIT_MS)
    {
        if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = mLastPos;
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)((float)mLastPos / 1000.0f * mDefaultFrequency);
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            getBytesFromSamples(mLastPos, position, mChannels, mFormat);
        }
    }
    else
    {
        result = mCodec->getPosition(position, postype);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT Stream::setLoopCount(int loopcount)
{
    if (mOpenState != FMOD_OPENSTATE_READY)
    {
        return FMOD_ERR_NOTREADY;
    }

    mLoopCountCurrent = loopcount;
    mLoopCount = loopcount;
    return FMOD_OK;
}

bool Stream::isStream()
{
    return true;
}

} // namespace FMOD
