// G2MEAB prototype translation unit; every native function is reconstructed (NonMatching).
// .text: 0x805B8A3C..0x805BAB64 (21 native functions, including the weak FMOD_DSP_DESCRIPTION_EX destructor
// 0x805B8DC4 emitted after init and its -0x5C base adjustor 0x805BAB5C).
// Original basename from the 4.06 reference library object. The 4.06 setupDSPCodec is inlined into alloc()
// (0x805B9204..0x805B941C); the reverb, low-pass and moveChannelGroup members of 4.06 are absent.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelSoftware uses the G2MEAB
// layout (fmod_channel_software.h).

#include "fmod_channel_software.h"
#include "fmod.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_codec_fsb.h"
#include "fmod_codec_wav.h"
#include "fmod_codeci.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_connection.h"
#include "fmod_dsp_fft.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_wavetable.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_sample_software.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

ChannelSoftware::ChannelSoftware()
{
    mDSPWaveTable = 0;
    mDSPHead = 0;
    mDSPResampler = 0;
    mDSPCodec = 0;
    mDSP = 0;
}

FMOD_RESULT ChannelSoftware::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
    FMOD_RESULT result;
    FMOD_DSP_DESCRIPTION description;
    FMOD_DSP_DESCRIPTION_EX descriptionex;

    ChannelReal::init(index, system, output, dspmixtarget);

    memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION));
    FMOD_strcpy(description.name, "FMOD Channel DSPHead Unit");
    description.version = 0x00010100;

    result = mSystem->createDSP(&description, &mDSPHead);
    if (result != FMOD_OK)
    {
        return result;
    }

    memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION));
    FMOD_strcpy(description.name, "FMOD SubChannel DSPHead Unit");
    description.version = 0x00010100;

    result = mSystem->createDSP(&description, &mSubChannelDSPHead);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSubChannelDSPHeadTarget = mSubChannelDSPHead;

    result = mSubChannelDSPHeadTarget->setActive(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
    FMOD_strcpy(descriptionex.name, "FMOD WaveTable Unit");
    descriptionex.version = 0x00010100;
    descriptionex.channels = dspmixtarget->mDescription.channels;
    descriptionex.setparameter = DSPWaveTable::setParameterCallback;
    descriptionex.getparameter = DSPWaveTable::getParameterCallback;
    descriptionex.setposition = DSPWaveTable::setPositionCallback;
    descriptionex.mCategory = FMOD_DSP_CATEGORY_WAVETABLE;
    descriptionex.mFormat = dspmixtarget->mDescription.mFormat;

    result = mSystem->createDSP(&descriptionex, &mDSPWaveTable);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mDSPWaveTable->setUserData(this);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mDSPWaveTable->setTargetFrequency((int)dspmixtarget->mDefaultFrequency);
    if (result != FMOD_OK)
    {
        return result;
    }

    mMinFrequency = -mMaxFrequency;

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::close()
{
    FMOD_RESULT result;

    result = ChannelReal::close();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mDSPWaveTable)
    {
        mDSPWaveTable->release(true);
        mDSPWaveTable = 0;
    }

    if (mDSPHead)
    {
        mDSPHead->release(true);
        mDSPHead = 0;
    }

    if (mSubChannelDSPHead)
    {
        mSubChannelDSPHead->release(true);
        mSubChannelDSPHead = 0;
        mSubChannelDSPHeadTarget = 0;
    }

    if (mDSPResampler)
    {
        mDSPResampler->release(true);
        mDSPResampler = 0;
    }

    if (mDSPCodec)
    {
        mDSPCodec->release(true);
        mDSPCodec = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::alloc()
{
    FMOD_RESULT result;

    if (!(mMode & FMOD_CREATECOMPRESSEDSAMPLE))
    {
        DSPWaveTable * dspwave = (DSPWaveTable *)mDSPWaveTable;
        int subchannels;

        if (!dspwave)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        result = dspwave->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mParent->getRealChannel(0, &subchannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (subchannels > 1)
        {
            if (!mSubChannelIndex)
            {
                mSubChannelDSPHeadTarget = mSubChannelDSPHead;

                result = mDSPHead->disconnectFrom(0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mSubChannelDSPHeadTarget->disconnectFrom(0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mParent->mChannelGroup->mDSPHead->addInput(mDSPHead);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mDSPHead->addInput(mSubChannelDSPHeadTarget);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            else
            {
                ChannelSoftware * channel0;

                result = mParent->getRealChannel((ChannelReal * *)&channel0, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mSubChannelDSPHeadTarget = channel0->mSubChannelDSPHeadTarget;
            }

            result = mSubChannelDSPHeadTarget->addInput(mDSPWaveTable);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            mSubChannelDSPHeadTarget = mSubChannelDSPHead;

            result = mDSPHead->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mSubChannelDSPHeadTarget->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mDSPWaveTable->disconnectFrom(0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mParent->mChannelGroup->mDSPHead->addInput(mDSPHead);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mDSPHead->addInput(mDSPWaveTable);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = setLoopPoints(mSound->mLoopStart, mSound->mLoopLength);
        if (result != FMOD_OK)
        {
            return result;
        }

        mMinFrequency = -mMaxFrequency;

        dspwave->mChannel = this;
        dspwave->mSound = mSound;
        dspwave->mPosition.mValue = 0;
        dspwave->mDirection = DSPWAVETABLE_SPEEDDIR_FORWARDS;

        mDSPHead->mUnk63 = false;
        mDSPHead->setActive(false);
        mDSPWaveTable->mUnk63 = false;
        mDSPWaveTable->setActive(false);
    }
    else
    {
        // Inlined 4.06 setupDSPCodec: the pool unit is used both through the address-taken handle (reloaded from
        // the stack) and through a register copy.
        DSPI * dsp;
        DSPCodec * dspcodec;
        Codec * codec;
        SampleSoftware * sample = (SampleSoftware *)mSound;

        result = mSystem->allocDSPCodec(sample->mFormat, &dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        dspcodec = (DSPCodec *)dsp;
        codec = dspcodec->mCodec;

        codec->mPCMBufferLength = mSound->mCodec->mPCMBufferLength;
        codec->mPCMBufferLengthBytes = dsp->mDescription.channels * (mSound->mCodec->mPCMBufferLength * sizeof(float));

        memcpy(&codec->mWaveFormat, &mSound->mCodec->waveformat[mSound->mSubSoundIndex], sizeof(FMOD_CODEC_WAVEFORMAT));

        // The codec reads the sample memory through the file storage at +0x17C; its last word (+0x31C) receives
        // the sample buffer. The G2MEAB member there is unresolved.
        dspcodec->mFileMemory.init(mSound->mLengthBytes, 0);
        dspcodec->mFileMemory.mMem = sample->mBuffer;

        if (mSound->mType == FMOD_SOUND_TYPE_WAV && mSound->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            CodecWav * srcwav = (CodecWav *)mSound->mCodec;
            CodecWav * destwav = (CodecWav *)codec;

            destwav->mSamplesPerADPCMBlock = srcwav->mSamplesPerADPCMBlock;
            destwav->mReadBufferLength = srcwav->mReadBufferLength;
        }

        if (mSound->mType == FMOD_SOUND_TYPE_FSB && mSound->mFormat == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            CodecWav * srcwav = ((CodecFSB *)mSound->mCodec)->mADPCM;
            CodecWav * destwav = (CodecWav *)codec;

            destwav->mSamplesPerADPCMBlock = srcwav->mSamplesPerADPCMBlock;
            destwav->mReadBufferLength = srcwav->mReadBufferLength;
        }

        dsp->mDescription.channels = mSound->mChannels;

        result = dsp->setTargetFrequency((int)mParent->mChannelGroup->mDSPHead->mDefaultFrequency);
        if (result != FMOD_OK)
        {
            return result;
        }

        mMinFrequency = 0.0f;
        mDSPCodec = dspcodec;
        dspcodec->mChannel = this;

        result = mDSPHead->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPWaveTable->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mParent->mChannelGroup->mDSPHead->addInput(mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPHead->addInput(dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        mDSPHead->mUnk63 = false;
        mDSPHead->setActive(false);
        dsp->mUnk63 = false;
        dsp->setActive(true);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::alloc(DSPI * dsp)
{
    FMOD_RESULT result;

    {
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));
        FMOD_strcpy(descriptionex.name, "FMOD Resampler Unit");
        descriptionex.version = 0x00010100;
        descriptionex.channels = 0;
        descriptionex.mCategory = FMOD_DSP_CATEGORY_RESAMPLER;

        result = mSystem->createDSP(&descriptionex, &mDSPResampler);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPResampler->setUserData(this);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mDSPResampler->setTargetFrequency((int)mParent->mChannelGroup->mDSPHead->mDefaultFrequency);
        if (result != FMOD_OK)
        {
            return result;
        }

        mMinFrequency = 0.0f;
    }

    result = mDSPHead->disconnectFrom(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mDSPWaveTable->disconnectFrom(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mParent->mChannelGroup->mDSPHead->addInput(mDSPHead);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mDSPHead->addInput(mDSPResampler);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mDSPResampler->addInput(dsp);
    if (result != FMOD_OK)
    {
        return result;
    }

    mDSPHead->mUnk63 = false;
    mDSPHead->setActive(false);
    mDSPResampler->mUnk63 = false;
    mDSPResampler->setActive(false);
    dsp->mUnk63 = false;
    dsp->setActive(false);

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::start()
{
    if (!(mFlags & CHANNELREAL_FLAG_PAUSED))
    {
        mDSPHead->setActive(true);
    }

    if (mSound)
    {
        mDSPWaveTable->setActive(true);
    }

    if (mDSPResampler)
    {
        mDSPResampler->setActive(true);
    }

    if (mDSPCodec)
    {
        mDSPCodec->setActive(true);
    }

    if (mDSP)
    {
        mDSP->setActive(true);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::stop(bool force, bool updateflags)
{
    FMOD_RESULT result;
    int count;

    if ((mFlags & CHANNELREAL_FLAG_ENDDELAY) && !force)
    {
        return FMOD_OK;
    }

    if (mDSPHead)
    {
        mDSPHead->mUnk63 = true;
        mDSPHead->setActive(false);
    }

    if (mDSPCodec)
    {
        mDSPCodec->mUnk63 = true;
        mDSPCodec->setActive(false);
        mDSPCodec->freeFromPool();
        mDSPCodec = 0;
    }

    if (mDSPResampler)
    {
        mDSPResampler->mUnk63 = true;
        mDSPResampler->setActive(false);
        mDSPResampler->release(true);
        mDSPResampler = 0;
    }

    if (mDSPWaveTable)
    {
        mDSPWaveTable->mUnk63 = true;
        mDSPWaveTable->setActive(false);
    }

    if (mDSP)
    {
        DSPI * prev;

        result = mDSP->getOutput(0, &prev);
        if (result == FMOD_OK)
        {
            result = prev->disconnectFrom(mDSP);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (mDSPHead)
    {
        for (count = 1; count < mDSPHead->mNumInputs; count++)
        {
            DSPI * input;

            result = mDSPHead->getInput(count, &input);
            if (result != FMOD_OK)
            {
                return result;
            }

            input->mUnk63 = true;
            input->setActive(false);
        }
    }

    ChannelReal::stop(force, true);

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::setPaused(bool paused)
{
    FMOD_RESULT result;

    result = mDSPHead->setActive(!paused);
    if (result != FMOD_OK)
    {
        return result;
    }

    return ChannelReal::setPaused(paused);
}

FMOD_RESULT ChannelSoftware::setVolume(float volume)
{
    FMOD_RESULT result;
    DSPConnection * connection;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    result = mDSPHead->getOutput(0, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    volume *= mParent->mVolume3D;
    volume *= mParent->mConeVolume3D;
    volume *= mParent->mVolumeOcclusion;
    volume *= mParent->mChannelGroup->mRealVolume;

    return connection->setMix(volume);
}

FMOD_RESULT ChannelSoftware::setFrequency(float frequency)
{
    if (mDSPResampler || mDSPCodec)
    {
        DSPResampler * resampler = mDSPCodec ? mDSPCodec : (DSPResampler *)mDSPResampler;

        if (!resampler)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        frequency *= mParent->mPitch3D;
        frequency *= mParent->mChannelGroup->mRealPitch;

        return resampler->setFrequency(frequency);
    }
    else
    {
        DSPWaveTable * dspwave = (DSPWaveTable *)mDSPWaveTable;

        if (!dspwave)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        frequency *= mParent->mPitch3D;
        frequency *= mParent->mChannelGroup->mRealPitch;

        return dspwave->setFrequency(frequency);
    }
}

FMOD_RESULT ChannelSoftware::setPan(float pan, float fbpan)
{
    FMOD_RESULT result;
    int subchannels;

    result = mParent->getRealChannel(0, &subchannels);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (subchannels > 1)
    {
        DSPConnection * connection;

        result = mSubChannelDSPHeadTarget->getInput(mSubChannelIndex, &connection);
        if (result != FMOD_OK)
        {
            return result;
        }

        return connection->setPan(pan);
    }
    else
    {
        DSPConnection * connection;

        result = mDSPHead->getOutput(0, &connection);
        if (result != FMOD_OK)
        {
            return result;
        }

        return connection->setPan(pan);
    }
}

FMOD_RESULT ChannelSoftware::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    FMOD_RESULT result;
    DSPConnection * connection;
    int numchannels;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    result = mDSPHead->getOutput(0, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (numchannels == 1)
    {
        if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_QUAD)
        {
            float levels[4] = { frontleft, frontright, backleft, backright };

            return connection->setLevels(levels, 1);
        }
        else if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_SURROUND)
        {
            float levels[4] = { frontleft, frontright, center, (backleft + backright) * 0.5f };

            return connection->setLevels(levels, 1);
        }
        else
        {
            float levels[8] = { frontleft, frontright, center, lfe, backleft, backright, sideleft, sideright };

            return connection->setLevels(levels, 1);
        }
    }
    else if (numchannels == 2)
    {
        if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_QUAD)
        {
            float levels[4][2] =
            {
                { frontleft, 0 },
                { 0, frontright },
                { backleft, 0 },
                { 0, backright }
            };

            return connection->setLevels(&levels[0][0], 2);
        }
        else if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_SURROUND)
        {
            float levels[4][2] =
            {
                { frontleft, 0 },
                { 0, frontright },
                { 0.5f * center, 0.5f * center },
                { 0.5f * backleft, 0.5f * backright }
            };

            return connection->setLevels(&levels[0][0], 2);
        }
        else
        {
            float levels[8][2] =
            {
                { frontleft, 0 },
                { 0, frontright },
                { 0.5f * center, 0.5f * center },
                { 0.5f * lfe, 0.5f * lfe },
                { backleft, 0 },
                { 0, backright },
                { sideleft, 0 },
                { 0, sideright }
            };

            return connection->setLevels(&levels[0][0], 2);
        }
    }

    return FMOD_ERR_TOOMANYCHANNELS;
}

FMOD_RESULT ChannelSoftware::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
    FMOD_RESULT result;
    DSPConnection * connection;
    float clevels[2][8];
    int count;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    result = mDSPHead->getOutput(0, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = connection->getLevels(&clevels[0][0]);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < numlevels; count++)
    {
        clevels[speaker][count] = levels[count];
    }

    return connection->setLevels(&clevels[0][0], 8);
}

FMOD_RESULT ChannelSoftware::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    unsigned int pcm, endpoint;
    int channels;
    FMOD_SOUND_FORMAT format;
    float frequency;

    if (mSubChannelIndex > 0)
    {
        return FMOD_OK;
    }

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }

    if (mDSPCodec)
    {
        channels = mDSPCodec->mDescription.channels;
        format = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency = mDSPCodec->mDefaultFrequency;
    }
    else if (mSound)
    {
        channels = mSound->mChannels;
        format = mSound->mFormat;
        frequency = mSound->mDefaultFrequency;
    }
    else if (mDSPResampler)
    {
        channels = mDSPResampler->mDescription.channels;
        format = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency = mDSPResampler->mDefaultFrequency;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        pcm = position;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(position, &pcm, channels, format);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        pcm = (unsigned int)((float)position / 1000.0f * frequency);
    }

    if (mSound)
    {
        if (mMode & FMOD_LOOP_OFF)
        {
            endpoint = mSound->mLength - 1;
        }
        else
        {
            endpoint = mLoopStart + mLoopLength - 1;
        }
    }
    else
    {
        endpoint = (unsigned int)-1;
    }

    if (pcm > endpoint)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    return mDSPHead->setPosition(pcm);
}

FMOD_RESULT ChannelSoftware::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    int channels;
    FMOD_SOUND_FORMAT format;
    float frequency;

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }

    if (mDSPCodec)
    {
        DSPResampler * dspcodec = mDSPCodec;

        if (!dspcodec)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        channels = dspcodec->mDescription.channels;
        format = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency = mSound->mDefaultFrequency;

        mPosition = (unsigned int)(dspcodec->mPosition.mValue >> 32);
    }
    else if (mSound)
    {
        DSPWaveTable * dspwave = (DSPWaveTable *)mDSPWaveTable;

        if (!dspwave)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        channels = mSound->mChannels;
        format = mSound->mFormat;
        frequency = mSound->mDefaultFrequency;

        mPosition = (unsigned int)(dspwave->mPosition.mValue >> 32);
    }
    else if (mDSPResampler)
    {
        channels = mDSPResampler->mDescription.channels;
        format = FMOD_SOUND_FORMAT_PCMFLOAT;
        frequency = mDSPResampler->mDefaultFrequency;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        *position = mPosition;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getBytesFromSamples(mPosition, position, channels, format);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        *position = (unsigned int)(1000.0f * ((float)mPosition / frequency));
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::isPlaying(bool * isplaying)
{
    FMOD_RESULT result;
    int numinputs;

    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & CHANNELREAL_FLAG_ALLOCATED)
    {
        *isplaying = true;
    }
    else if (mDSPCodec)
    {
        *isplaying = mDSPCodec->mUnk63;
        *isplaying = !*isplaying;
    }
    else if (mDSPResampler)
    {
        result = mDSPResampler->getNumInputs(&numinputs);
        if (result != FMOD_OK)
        {
            *isplaying = false;
        }
        else
        {
            int count, finished = 0;

            for (count = 0; count < numinputs; count++)
            {
                DSPI * dsp;

                result = mDSPResampler->getInput(count, &dsp);
                if (result != FMOD_OK)
                {
                    finished = numinputs;
                    break;
                }

                if (dsp->mUnk63)
                {
                    finished++;
                }
            }

            if (finished == numinputs)
            {
                *isplaying = false;
            }
            else
            {
                *isplaying = true;
            }
        }
    }
    else if (!mSound && !mDSP)
    {
        *isplaying = false;
    }
    else
    {
        *isplaying = mDSPWaveTable->mUnk63;
        *isplaying = !*isplaying;
    }

    if (!*isplaying)
    {
        if (mEndDelay)
        {
            mFlags |= CHANNELREAL_FLAG_ENDDELAY;
            *isplaying = true;
            return FMOD_OK;
        }

        mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mFlags &= ~CHANNELREAL_FLAG_PLAYING;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelSoftware::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    static DSPFFT fft;
    FMOD_RESULT result;
    DSPFilter * dsphead;
    float * buffer;
    unsigned int position, length;
    int numchannels;
    unsigned int blocksize;

    dsphead = (DSPFilter *)mDSPHead;
    if (!dsphead)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    numvalues *= 2;

    if (numvalues != 128 && numvalues != 256 && numvalues != 512 && numvalues != 1024 &&
        numvalues != 2048 && numvalues != 4096 && numvalues != 8192 && numvalues != 16384)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering(16384);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numvalues > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mSystem->getDSPBufferSize(&blocksize, 0);

    position -= numvalues;
    if ((int)position < 0)
    {
        position += length;
    }

    return fft.getSpectrum(buffer, position, length, spectrumarray, numvalues, channeloffset, numchannels, windowtype);
}

FMOD_RESULT ChannelSoftware::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
    FMOD_RESULT result;
    DSPFilter * dsphead;
    float * buffer;
    unsigned int position, length;
    int numchannels, count;

    dsphead = (DSPFilter *)mDSPHead;
    if (!dsphead)
    {
        return FMOD_ERR_INITIALIZATION;
    }

    if (mSound)
    {
        numchannels = mSound->mChannels;
    }
    else if (mDSP)
    {
        numchannels = mDSP->mDescription.channels;
    }
    else
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (channeloffset >= numchannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = dsphead->startBuffering(16384);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getHistoryBuffer(&buffer, &position, &length);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numvalues > (int)length)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    position -= numvalues;
    if ((int)position < 0)
    {
        position += length;
    }

    for (count = 0; count < numvalues; count++)
    {
        wavearray[count] = buffer[position * numchannels + channeloffset];

        position++;
        if (position >= length)
        {
            position = 0;
        }
    }

    return result;
}

FMOD_RESULT ChannelSoftware::getDSPHead(DSPI * * dsp)
{
    *dsp = mDSPHead;

    return FMOD_OK;
}

} // namespace FMOD
