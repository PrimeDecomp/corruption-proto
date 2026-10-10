// G2MEAB prototype translation unit; ChannelStream reconstruction. The basename is the configured split name:
// the code is FMOD ChannelStream (4.06 fmod_channel_stream.cpp), no public ChannelGroup wrapper is retained.
// All 34 retained functions are reconstructed; remaining diffs are stmw/_savegpr (profile) and getPosition register use.
// G2MEAB .text: 0x805BAB64..0x805BC8B8 (34 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: 805BAB64 constructs a0xB4-byte low-level aggregate voice with child count+4, child
// slots+8, base table+74, intrusive lists+78/+A0 and distinct vtable806E2A4C. SystemI8061E5B4
// embeds two such aggregates at+FE0/+1094;80621038 allocates0xB4 from a freelist and calls the same
// constructor, independently proving lifecycle closure. Native dispatch methods iterate child
// voices through their+74 vtables, retaining one-child exceptions and first-error handling;
// prepare/start/position/mode/stop methods share that object. Final805BC8B0 is an8-byte -0x78
// destructor adjustor to805BC7C0. Next805BC8B8 operates on a different high-level channel-group
// object and explicitly asserted channelgroupi allocation/free. Inferred basename; group here is
// the low-level aggregate voice rather than a claim of identical public API class naming. Preserve
// every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty
// are recorded externally.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelStream uses the G2MEAB layout.

#include "fmod_channel_stream.h"
#include "fmod_codeci.h"
#include "fmod_localcriticalsection.h"
#include "fmod_os_misc.h"
#include "fmod_sound_sample.h"
#include "fmod_sound_stream.h"
#include "fmod_systemi.h"

namespace FMOD {

ChannelStream::ChannelStream()
{
    mSamplesProcessed = mSamplesProcessedLast = 0;
}

FMOD_RESULT ChannelStream::set2DFreqVolumePanFor3D()
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set2DFreqVolumePanFor3D();
    }

    return result;
}

FMOD_RESULT ChannelStream::alloc()
{
    FMOD_RESULT result;
    Stream * stream = (Stream *)mSound;
    int count;

    mFlags &= ~CHANNELREAL_FLAG_STOPPED;
    mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;

    mSystem = stream->mSystem;
    mFinished = false;
    mBusy = false;
    mLastPCM = 0;
    mDecodeOffset = 0;
    mPosition = 0;
    mSamplesProcessed = 0;
    mSamplesProcessedLast = 0;

    mMinFrequency = mRealChannel[0]->mMinFrequency;
    if (mMinFrequency < 100.0f)
    {
        mMinFrequency = 100.0f;
    }
    mMaxFrequency = mRealChannel[0]->mMaxFrequency;

    for (count = 0; count < mNumRealChannels; count++)
    {
        Sample * sample = stream->mSample;

        if (sample->mNumSubSamples)
        {
            sample = sample->mSubSample[count];
        }

        mRealChannel[count]->mSubChannelIndex = count;
        mRealChannel[count]->mParent = mParent;
        mRealChannel[count]->mSound = sample;
        mRealChannel[count]->mMode = sample->mMode;

        result = mRealChannel[count]->alloc();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    FMOD_OS_CriticalSection_Enter(SystemI::gStreamListCrit);
    mStreamNode.setData(this);
    mStreamNode.addBefore(&SystemI::gStreamHead);
    FMOD_OS_CriticalSection_Leave(SystemI::gStreamListCrit);

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::start()
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->start();
        if (result != FMOD_OK)
        {
            return result;
        }

        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;
        mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_PLAYING;
    }

    return result;
}

FMOD_RESULT ChannelStream::update(int delta)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->update(delta);
    }

    return result;
}

FMOD_RESULT ChannelStream::updateStream()
{
    FMOD_RESULT result;
    Stream * stream;
    Sample * sample;
    unsigned int pcm;
    int delta;
    LocalCriticalSection crit(SystemI::gStreamCrit);
    LocalCriticalSection fillcrit(SystemI::gStreamFillCrit);

    if (mFinished)
    {
        return FMOD_OK;
    }

    fillcrit.enter();
    crit.enter();

    stream = (Stream *)mSound;
    sample = stream->mSample;

    if (stream->mOpenState)
    {
        return FMOD_ERR_NOTREADY;
    }

    result = mRealChannel[0]->getPosition(&pcm, FMOD_TIMEUNIT_PCM);
    if (result != FMOD_OK)
    {
        return result;
    }

    crit.leave();

    while ((mSamplesProcessed > mSamplesProcessedLast && mSamplesProcessed - mSamplesProcessedLast >= (unsigned int)stream->mBlockSize) ||
           (mSamplesProcessed < mSamplesProcessedLast && mSamplesProcessedLast - mSamplesProcessed >= (unsigned int)stream->mBlockSize))
    {
        unsigned int len = stream->mBlockSize;
        unsigned int endpoint;

        if (mDecodeOffset > sample->mLength)
        {
            len = 0;
        }
        else if (mDecodeOffset + len > sample->mLength)
        {
            len = sample->mLength - mDecodeOffset;
        }

        result = stream->fill(mDecodeOffset, len);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            int count;

            for (count = 0; count < mNumRealChannels; count++)
            {
                mRealChannel[count]->setPaused(true);
            }

            mFinished = true;
            mStreamNode.removeNode();
            return result;
        }

        stream->mWantsToFlush = true;

        mDecodeOffset += len;
        if (mDecodeOffset >= sample->mLength)
        {
            mDecodeOffset -= sample->mLength;
        }

        mSamplesProcessedLast += len;

        if (stream->mLength < mLoopStart + mLoopLength)
        {
            mLoopLength = stream->mLength - mLoopStart;
        }

        if ((mMode & FMOD_LOOP_NORMAL) && mLoopCount)
        {
            endpoint = mLoopStart + mLoopLength - 1;
        }
        else
        {
            endpoint = mSound->mLength - 1;
        }

        if (mPosition > endpoint)
        {
            len = 0;
        }
        else if (mPosition + len > endpoint)
        {
            len = endpoint - mPosition + 1;
        }

        mPosition += len;

        if (mPosition > endpoint)
        {
            if (((mMode & FMOD_LOOP_NORMAL) && mLoopCount) || mSound->mLength == (unsigned int)-1)
            {
                mPosition = mLoopStart;
                if (mLoopCount > 0)
                {
                    mLoopCount--;
                }
            }
            else if (stream->mFinished)
            {
                int count;

                mPosition = mSound->mLength;

                for (count = 0; count < mNumRealChannels; count++)
                {
                    mRealChannel[count]->setPaused(true);
                }

                mFinished = true;
                mStreamNode.removeNode();
                break;
            }
        }
    }

    delta = pcm - mLastPCM;
    if (delta < 0)
    {
        delta += sample->mLength;
    }

    mSamplesProcessed += delta;
    mLastPCM = pcm;

    fillcrit.leave();

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::setMode(FMOD_MODE mode)
{
    FMOD_RESULT result;
    int count;

    result = ChannelReal::setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mSound->setMode(mode);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setMode(mode);

        mRealChannel[count]->mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
        mRealChannel[count]->mMode |= FMOD_LOOP_NORMAL;
    }

    return result;
}

FMOD_RESULT ChannelStream::stop(bool force, bool updateflags)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    mFinished = true;

    FMOD_OS_CriticalSection_Enter(SystemI::gStreamListCrit);
    mStreamNode.removeNode();
    FMOD_OS_CriticalSection_Leave(SystemI::gStreamListCrit);

    FMOD_OS_CriticalSection_Enter(SystemI::gStreamCrit);
    mBusy = true;

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (!mRealChannel[count])
        {
            continue;
        }

        if (updateflags)
        {
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_RESERVED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PLAYING;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_UNK200;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;
            mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_STOPPED;
        }

        if (mRealChannel[count]->mEndDelay && !force)
        {
            mFlags |= CHANNELREAL_FLAG_ENDDELAY;
            mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_ENDDELAY;
        }

        result = mRealChannel[count]->stop(force, true);

        mRealChannel[count]->mSound = 0;
        mRealChannel[count]->mDSP = 0;
        mRealChannel[count]->mParent = 0;
        mRealChannel[count] = 0;
    }

    mSystem->mStreamPool.free(this);

    mBusy = false;
    FMOD_OS_CriticalSection_Leave(SystemI::gStreamCrit);

    return result;
}

FMOD_RESULT ChannelStream::setPaused(bool paused)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setPaused(paused);
    }

    return result;
}

FMOD_RESULT ChannelStream::setVolume(float volume)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setVolume(volume);
    }

    return result;
}

FMOD_RESULT ChannelStream::setFrequency(float frequency)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setFrequency(frequency);
    }

    return result;
}

FMOD_RESULT ChannelStream::setPan(float pan, float fbpan)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (mNumRealChannels == 2)
        {
            if (count == 0)
            {
                pan = -1.0f;
            }
            else
            {
                pan = 1.0f;
            }
        }

        result = mRealChannel[count]->setPan(pan, fbpan);
    }

    return result;
}

FMOD_RESULT ChannelStream::setDelay(unsigned int startdelay, unsigned int enddelay)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setDelay(startdelay, enddelay);
    }

    return result;
}

FMOD_RESULT ChannelStream::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setSpeakerMix(frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright);
    }

    return result;
}

FMOD_RESULT ChannelStream::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setSpeakerLevels(speaker, levels, numlevels);
    }

    return result;
}

FMOD_RESULT ChannelStream::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;
    bool paused;
    Stream * stream;

    if (mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    stream = (Stream *)mSound;
    if (!stream)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == FMOD_TIMEUNIT_MS || postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        switch (postype)
        {
            case FMOD_TIMEUNIT_MS:
            {
                position = (unsigned int)((float)position / 1000.0f * stream->mDefaultFrequency);
                postype = FMOD_TIMEUNIT_PCM;
                break;
            }
            case FMOD_TIMEUNIT_PCM:
            {
                break;
            }
            case FMOD_TIMEUNIT_PCMBYTES:
            {
                position = (unsigned int)((float)position / 1000.0f * stream->mDefaultFrequency);
                stream->getSamplesFromBytes(position, &position);
                postype = FMOD_TIMEUNIT_PCM;
                break;
            }
        }
    }

    if (postype == FMOD_TIMEUNIT_SENTENCE)
    {
        stream->mSubSoundIndex = stream->mSubSoundList[position];
        postype = FMOD_TIMEUNIT_MS;
        position = 0;
    }

    if (postype != FMOD_TIMEUNIT_PCM || stream->mSubSoundIndex != stream->mCodec->mSubSoundIndex || stream->mWantsToFlush || (mFlags & CHANNELREAL_FLAG_PLAYING))
    {
        int count;

        FMOD_OS_CriticalSection_Enter(SystemI::gStreamFillCrit);

        stream->mWantsToFlush = false;

        mRealChannel[0]->getPaused(&paused);

        for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count]->setPaused(true);
        }

        result = stream->setPosition(position, postype);
        if (result == FMOD_OK)
        {
            for (count = 0; count < mNumRealChannels; count++)
            {
                mRealChannel[count]->setPosition(0, FMOD_TIMEUNIT_PCM);
            }

            mLastPCM = 0;
            mDecodeOffset = 0;
            mPosition = position;
            mSamplesProcessed = mSamplesProcessedLast = 0;

            result = stream->flush();
        }

        for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count]->setPaused(paused);
        }

        FMOD_OS_CriticalSection_Leave(SystemI::gStreamFillCrit);
    }

    return result;
}

FMOD_RESULT ChannelStream::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    Stream * stream;
    bool getsubsoundtime = false;

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    stream = (Stream *)mSound;
    if (!stream)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
    {
        postype = FMOD_TIMEUNIT_MS;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
    {
        postype = FMOD_TIMEUNIT_PCM;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
    {
        postype = FMOD_TIMEUNIT_PCMBYTES;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        getsubsoundtime = true;
    }

    if (getsubsoundtime && !stream->mSubSoundList)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == FMOD_TIMEUNIT_MS || postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES ||
        postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        unsigned int pcmcurrent;
        int pcmleft;
        int currentsubsoundid;
        int currentsentenceid = 0;

        result = mRealChannel[0]->getPosition(&pcmcurrent, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            pcmcurrent = 0;
        }

        pcmleft = pcmcurrent - (mSamplesProcessedLast % stream->mSample->mLength);
        if (pcmleft < 0)
        {
            pcmleft += stream->mSample->mLength;
        }

        pcmcurrent = mPosition;

        while (pcmleft)
        {
            unsigned int len;
            unsigned int pcmtonext;
            unsigned int endpoint;

            if ((mMode & FMOD_LOOP_NORMAL) && mLoopCount)
            {
                endpoint = mLoopStart + mLoopLength - 1;
            }
            else
            {
                endpoint = mSound->mLength - 1;
            }

            len = pcmleft;
            pcmtonext = endpoint - pcmcurrent + 1;
            if (pcmtonext < len)
            {
                len = pcmtonext;
            }

            pcmcurrent += len;
            if (pcmcurrent > endpoint)
            {
                if ((stream->mMode & FMOD_LOOP_NORMAL) && mLoopCount)
                {
                    pcmcurrent = mLoopStart;
                }
                else
                {
                    pcmcurrent = mSound->mLength;
                }
            }

            if (!len)
            {
                break;
            }

            pcmleft -= len;
        }

        if (getsubsoundtime)
        {
            for (currentsubsoundid = 0; currentsubsoundid < mSound->mSubSoundListNum; currentsubsoundid++)
            {
                SoundI * sound = mSound->mSubSound[mSound->mSubSoundList[currentsubsoundid]];

                if (sound)
                {
                    if (pcmcurrent < sound->mLength)
                    {
                        break;
                    }

                    pcmcurrent -= sound->mLength;
                }

                currentsentenceid++;
            }
        }

        if (postype == FMOD_TIMEUNIT_SENTENCE)
        {
            *position = currentsentenceid;
        }
        else if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
        {
            *position = mSound->mSubSoundList[currentsentenceid];
        }
        else if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = pcmcurrent;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(pcmcurrent, position, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)(1000.0f * ((float)pcmcurrent / mSound->mDefaultFrequency));
        }
    }
    else
    {
        return stream->getPosition(position, postype);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    FMOD_RESULT result;

    result = ChannelReal::setLoopPoints(loopstart, looplength);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    result = mSound->setLoopPoints(loopstart, FMOD_TIMEUNIT_PCM, loopstart + looplength - 1, FMOD_TIMEUNIT_PCM);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::setLoopCount(int loopcount)
{
    FMOD_RESULT result;

    result = ChannelReal::setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    result = mSound->setLoopCount(loopcount);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::set3DAttributes()
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DAttributes();
    }

    return result;
}

FMOD_RESULT ChannelStream::set3DMinMaxDistance()
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DMinMaxDistance();
    }

    return result;
}

FMOD_RESULT ChannelStream::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        mRealChannel[count]->set3DConeSettings(insideconeangle, outsideconeangle, outsidevolume);
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelStream::set3DConeOrientation(FMOD_VECTOR * orientation)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DConeOrientation(orientation);
    }

    return result;
}

FMOD_RESULT ChannelStream::set3DOcclusion(float directocclusion, float reverbocclusion)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set3DOcclusion(directocclusion, reverbocclusion);
    }

    return result;
}

FMOD_RESULT ChannelStream::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setReverbProperties(prop);
    }

    return result;
}

FMOD_RESULT ChannelStream::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    for (count = 0; count < mNumRealChannels && count < 1; count++)
    {
        result = mRealChannel[count]->getReverbProperties(prop);
    }

    return result;
}

FMOD_RESULT ChannelStream::isPlaying(bool * isplaying)
{
    *isplaying = !mFinished;
    return FMOD_OK;
}

FMOD_RESULT ChannelStream::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    return mRealChannel[0]->getSpectrum(spectrumarray, numvalues, channeloffset, windowtype);
}

FMOD_RESULT ChannelStream::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
    return mRealChannel[0]->getWaveData(wavearray, numvalues, channeloffset);
}

FMOD_RESULT ChannelStream::getDSPHead(DSPI * * dsp)
{
    return mRealChannel[0]->getDSPHead(dsp);
}

// Inline but called out of line by stop() and emitted weak after the implicit members (0x805BC854):
// its body must follow stop(). Defining it here is a held hypothesis; a pool template instantiated at
// the end of the TU would give the same placement.
inline FMOD_RESULT ChannelStreamPool::free(ChannelStream * channel)
{
    channel->removeNode();
    channel->addAfter(&mFreeHead);
    return FMOD_OK;
}

} // namespace FMOD
