// NonMatching partial reconstruction of the G2MEAB MIDI codec (DLS-driven software synth).
// G2MEAB .text: 0x805D12C8..0x805D4E84 (28 native functions). No FMOD Ex 4.06 or Gormiti reference
// names these classes; see fmod_codec_midi.h.
// Implemented: descriptor, track readers, voice stop/volume/pitch/pan, channel and song tick update,
// reset, close, read, seek and the four callbacks. Empty placeholders: openInternal 0x805D3C20 and
// MIDITrack::process 0x805D3304. Not yet present: the DLS articulation/region helpers 0x805D136C,
// 0x805D13B8, 0x805D17EC, 0x805D2588 and the note-on handler 0x805D2748, which need the DLS types.
// Native readVarLen 0x805D30B4 has readByte 0x805D3148 (emitted after it) expanded inline, which the
// current profile does not reproduce. The __LINE__ values the native allocations pass (0x92F..0xCC2)
// are not reproduced.

#include "fmod_codec_midi.h"
#include "fmod.h"
#include "fmod_channelpool.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"
#include "fmod_types.h"

#include <math.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX midicodec;

// Envelope time for each 1/128 step of the DLS timecent range (data 0x806E3518).
float gTimeCentsTable[128] = // Guessed name
{
    0.977f, 1.062f, 1.155f, 1.256f, 1.366f, 1.486f, 1.616f, 1.757f,
    1.911f, 2.079f, 2.261f, 2.459f, 2.674f, 2.908f, 3.163f, 3.439f,
    3.741f, 4.068f, 4.424f, 4.812f, 5.233f, 5.691f, 6.19f, 6.732f,
    7.321f, 7.962f, 8.659f, 9.417f, 10.242f, 11.139f, 12.114f, 13.175f,
    14.328f, 15.583f, 16.947f, 18.431f, 20.045f, 21.8f, 23.709f, 25.785f,
    28.042f, 30.498f, 33.168f, 36.072f, 39.231f, 42.666f, 46.401f, 50.464f,
    54.883f, 59.688f, 64.915f, 70.598f, 76.78f, 83.503f, 90.814f, 98.766f,
    107.414f, 116.819f, 127.047f, 138.171f, 150.27f, 163.427f, 177.737f, 193.299f,
    210.224f, 228.631f, 248.65f, 270.421f, 294.099f, 319.85f, 347.856f, 378.314f,
    411.439f, 447.464f, 486.644f, 529.254f, 575.595f, 625.993f, 680.805f, 740.415f,
    805.245f, 875.752f, 952.432f, 1035.826f, 1126.522f, 1225.159f, 1332.433f, 1449.099f,
    1575.981f, 1713.972f, 1864.046f, 2027.26f, 2204.765f, 2397.812f, 2607.763f, 2836.096f,
    3084.422f, 3354.491f, 3648.207f, 3967.64f, 4315.043f, 4692.864f, 5103.767f, 5550.648f,
    6036.658f, 6565.222f, 7140.066f, 7765.244f, 8445.161f, 9184.612f, 9988.808f, 10863.418f,
    11814.609f, 12849.085f, 13974.139f, 15197.702f, 16528.398f, 17975.61f, 19549.537f, 21261.275f,
    23122.893f, 25147.512f, 27349.404f, 29744.092f, 32348.457f, 35180.86f, 38261.26f, 41611.383f
};

// Linear gain for each half decibel of attenuation, -96 dB at index 0 to 0 dB at index 192 (data 0x806E3718).
float gAttenuationTable[193] = // Guessed name
{
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 0.0001f, 0.0001f, 0.0001f, 0.0001f,
    0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f,
    0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f, 0.0001f,
    0.0002f, 0.0002f, 0.0002f, 0.0002f, 0.0002f, 0.0002f, 0.0002f, 0.0002f,
    0.0003f, 0.0003f, 0.0003f, 0.0003f, 0.0003f, 0.0003f, 0.0004f, 0.0004f,
    0.0004f, 0.0004f, 0.0004f, 0.0005f, 0.0005f, 0.0005f, 0.0006f, 0.0006f,
    0.0006f, 0.0007f, 0.0007f, 0.0007f, 0.0008f, 0.0008f, 0.0009f, 0.0009f,
    0.001f, 0.0011f, 0.0011f, 0.0012f, 0.0013f, 0.0013f, 0.0014f, 0.0015f,
    0.0016f, 0.0017f, 0.0018f, 0.0019f, 0.002f, 0.0021f, 0.0022f, 0.0024f,
    0.0025f, 0.0027f, 0.0028f, 0.003f, 0.0032f, 0.0033f, 0.0035f, 0.0038f,
    0.004f, 0.0042f, 0.0045f, 0.0047f, 0.005f, 0.0053f, 0.0056f, 0.006f,
    0.0063f, 0.0067f, 0.0071f, 0.0075f, 0.0079f, 0.0084f, 0.0089f, 0.0094f,
    0.01f, 0.0106f, 0.0112f, 0.0119f, 0.0126f, 0.0133f, 0.0141f, 0.015f,
    0.0158f, 0.0168f, 0.0178f, 0.0188f, 0.02f, 0.0211f, 0.0224f, 0.0237f,
    0.0251f, 0.0266f, 0.0282f, 0.0299f, 0.0316f, 0.0335f, 0.0355f, 0.0376f,
    0.0398f, 0.0422f, 0.0447f, 0.0473f, 0.0501f, 0.0531f, 0.0562f, 0.0596f,
    0.0631f, 0.0668f, 0.0708f, 0.075f, 0.0794f, 0.0841f, 0.0891f, 0.0944f,
    0.1f, 0.1059f, 0.1122f, 0.1189f, 0.1259f, 0.1334f, 0.1413f, 0.1496f,
    0.1585f, 0.1679f, 0.1778f, 0.1884f, 0.1995f, 0.2113f, 0.2239f, 0.2371f,
    0.2512f, 0.2661f, 0.2818f, 0.2985f, 0.3162f, 0.335f, 0.3548f, 0.3758f,
    0.3981f, 0.4217f, 0.4467f, 0.4732f, 0.5012f, 0.5309f, 0.5623f, 0.5957f,
    0.631f, 0.6683f, 0.7079f, 0.7499f, 0.7943f, 0.8414f, 0.8913f, 0.9441f,
    1.0f
};

FMOD_CODEC_DESCRIPTION_EX * CodecMIDI::getDescriptionEx()
{
    memset(&midicodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    midicodec.name = "FMOD MIDI Codec";
    midicodec.version = 0x00010100;
    midicodec.timeunits = FMOD_TIMEUNIT_PCM;
    midicodec.defaultasstream = 1;
    midicodec.open = &CodecMIDI::openCallback;
    midicodec.close = &CodecMIDI::closeCallback;
    midicodec.read = &CodecMIDI::readCallback;
    midicodec.setposition = &CodecMIDI::setPositionCallback;

    midicodec.mType = FMOD_SOUND_TYPE_MIDI;
    midicodec.mSize = sizeof(CodecMIDI);

    return &midicodec;
}

float MIDIVoice::timeCentsToTime(int timecents)
{
    float index;

    if (timecents == (int)0x80000000)
    {
        return 0.0f;
    }

    index = (float)timecents / 78643200.0f;
    index += 10.0f;
    index *= 8.533334f;
    if (index < 0.0f)
    {
        index = 0.0f;
    }
    if (index >= 128.0f)
    {
        index = 127.0f;
    }

    return gTimeCentsTable[(int)index];
}

FMOD_RESULT MIDIVoice::stop()
{
    mChannel.stopEx(false, false, true, true, false);

    mUnk154 = 0;
    mKeyOff = false;
    mUnk1D4 = -1;

    removeNode();
    addAfter(&mMIDI->mFreeVoiceHead);

    return FMOD_OK;
}

FMOD_RESULT MIDIVoice::updateVolume()
{
    float envvolume = 1.0f;
    float velocity, volume, expression;
    float lfovolume = 1.0f;
    float total;

    if (mVolumeEnvelope.mActive)
    {
        MIDIEnvelopeStage * stage;
        float level;

        if (mKeyOff && !mMIDIChannel->mSustainPedal && mVolumeEnvelope.mState != 2)
        {
            stage = &mVolumeEnvelope.mStage[mVolumeEnvelope.mState];
            level = stage->mStart + mVolumeEnvelope.mTime * ((stage->mEnd - stage->mStart) / stage->mTime);

            if (mVolumeEnvelope.mState == 0)
            {
                level = 20.0f * (float)log10(1.0f - level / -96.0f);
            }
            if (mVolumeEnvelope.mState == 1 && level < mVolumeEnvelope.mSustain)
            {
                level = mVolumeEnvelope.mSustain;
            }

            mVolumeEnvelope.mState = 2;

            stage = &mVolumeEnvelope.mStage[mVolumeEnvelope.mState];
            mVolumeEnvelope.mTime = (level - stage->mStart) / ((stage->mEnd - stage->mStart) / stage->mTime);
        }

        while (mVolumeEnvelope.mTime >= mVolumeEnvelope.mStage[mVolumeEnvelope.mState].mTime && mVolumeEnvelope.mState < 3)
        {
            if (mVolumeEnvelope.mState == 1 && (!mKeyOff || mMIDIChannel->mSustainPedal))
            {
                mVolumeEnvelope.mTime = mVolumeEnvelope.mStage[mVolumeEnvelope.mState].mTime;
                break;
            }

            mVolumeEnvelope.mTime -= mVolumeEnvelope.mStage[mVolumeEnvelope.mState].mTime;
            mVolumeEnvelope.mState++;
        }

        if (mVolumeEnvelope.mState >= 3)
        {
            return stop();
        }

        stage = &mVolumeEnvelope.mStage[mVolumeEnvelope.mState];
        if (stage->mTime > 0.0f)
        {
            level = stage->mStart + mVolumeEnvelope.mTime * ((stage->mEnd - stage->mStart) / stage->mTime);
        }
        else
        {
            level = stage->mStart;
        }

        if (mVolumeEnvelope.mState == 1 && level < mVolumeEnvelope.mSustain)
        {
            level = mVolumeEnvelope.mSustain;
        }

        if (mVolumeEnvelope.mState == 0)
        {
            envvolume = 1.0f - level / -96.0f;
        }
        else
        {
            envvolume = gAttenuationTable[192 - (int)(-2.0f * level)];
        }
    }

    velocity = (float)(mVelocity * mVelocity) / 16129.0f;
    volume = (float)(mMIDIChannel->mVolume * mMIDIChannel->mVolume) / 16129.0f;
    expression = (float)(mMIDIChannel->mExpression * mMIDIChannel->mExpression) / 16129.0f;

    if (mLFOTime >= mLFODelay)
    {
        lfovolume = (float)sin(mLFOFrequency * (6.2831855f * ((mLFOTime - mLFODelay) / 1000.0f)));
        lfovolume = 1.0f + mLFOVolumeDepth * lfovolume;
        if (lfovolume < 0.0f)
        {
            lfovolume = 0.0f;
        }
        if (lfovolume > 1.0f)
        {
            lfovolume = 1.0f;
        }
    }

    total = envvolume * velocity * volume * expression * mGain * lfovolume;

    if (mVolumeEnvelope.mState == 2 && total < 0.0009765625f)
    {
        return stop();
    }

    mChannel.setVolume(total);

    return FMOD_OK;
}

FMOD_RESULT MIDIVoice::updatePitch()
{
    float envpitch = 0.0f;
    float bend, keypitch, finetune, root, lfopitch, frequency, deffrequency;

    if (mPitchEnvelope.mActive)
    {
        MIDIEnvelopeStage * stage;
        float level;

        if (mKeyOff && !mMIDIChannel->mSustainPedal && mPitchEnvelope.mState != 2)
        {
            stage = &mPitchEnvelope.mStage[mPitchEnvelope.mState];
            level = stage->mStart + mPitchEnvelope.mTime * ((stage->mEnd - stage->mStart) / stage->mTime);

            if (mPitchEnvelope.mState == 1 && level < mPitchEnvelope.mSustain)
            {
                level = mPitchEnvelope.mSustain;
            }

            mPitchEnvelope.mState = 2;

            stage = &mPitchEnvelope.mStage[mPitchEnvelope.mState];
            mPitchEnvelope.mTime = (level - stage->mStart) / ((stage->mEnd - stage->mStart) / stage->mTime);
        }

        while (mPitchEnvelope.mTime >= mPitchEnvelope.mStage[mPitchEnvelope.mState].mTime && mPitchEnvelope.mState < 3)
        {
            if (mPitchEnvelope.mState == 1 && mPitchEnvelope.mSustain > 0.0f && (!mKeyOff || mMIDIChannel->mSustainPedal))
            {
                mPitchEnvelope.mTime = mPitchEnvelope.mStage[mPitchEnvelope.mState].mTime;
                break;
            }

            mPitchEnvelope.mTime -= mPitchEnvelope.mStage[mPitchEnvelope.mState].mTime;
            mPitchEnvelope.mState++;
        }

        if (mPitchEnvelope.mState >= 3)
        {
            envpitch = 0.0f;
        }
        else
        {
            stage = &mPitchEnvelope.mStage[mPitchEnvelope.mState];
            if (stage->mTime)
            {
                envpitch = stage->mStart + mPitchEnvelope.mTime * ((stage->mEnd - stage->mStart) / stage->mTime);
            }
            else
            {
                envpitch = stage->mStart;
            }

            if (mPitchEnvelope.mState == 1 && envpitch < mPitchEnvelope.mSustain)
            {
                envpitch = mPitchEnvelope.mSustain;
            }

            envpitch *= mPitchEnvelope.mDepth;
        }
    }

    bend = 100.0f * ((float)mMIDIChannel->mPitchBendRange / 256.0f) * ((float)mMIDIChannel->mPitchBend / 8192.0f);
    keypitch = (float)mScaleTuning * (float)mKey / 128.0f;
    finetune = (float)mFineTune;
    root = 100.0f * (float)mUnityNote;

    if (mLFOTime >= mLFODelay)
    {
        lfopitch = mLFOPitchDepth * (float)sin(mLFOFrequency * (6.2831855f * ((mLFOTime - mLFODelay) / 1000.0f)));
    }
    else
    {
        lfopitch = 0.0f;
    }

    frequency = (float)pow(2.0, (envpitch + bend + keypitch + finetune - root + lfopitch) / 1200.0f);

    mSound->getDefaults(&deffrequency, 0, 0, 0);

    mChannel.setFrequency(frequency * deffrequency);

    return FMOD_OK;
}

FMOD_RESULT MIDIVoice::updatePan()
{
    mChannel.setPan((float)mMIDIChannel->mPan / 64.0f - 1.0f, true);

    return FMOD_OK;
}

FMOD_RESULT MIDIChannel::update()
{
    MIDIVoice * voice = (MIDIVoice *)mVoiceHead.getNext();

    while (voice != &mVoiceHead)
    {
        MIDIVoice * next = (MIDIVoice *)voice->getNext();

        if (voice->mUnk154)
        {
            voice->updateVolume();
            voice->mVolumeEnvelope.mTime += mTrack->mMIDI->mTickLength;
            voice->updatePitch();
            voice->mPitchEnvelope.mTime += mTrack->mMIDI->mTickLength;
            voice->updatePan();
            voice->mLFOTime += mTrack->mMIDI->mTickLength;
        }

        voice = next;
    }

    return FMOD_OK;
}

FMOD_RESULT MIDITrack::readVarLen(unsigned int * value)
{
    unsigned char byte;
    unsigned int result;

    if (readByte(&byte) != FMOD_OK)
    {
        return FMOD_ERR_FILE_EOF;
    }

    result = byte;
    if (result & 0x80)
    {
        result &= 0x7F;
        do
        {
            if (readByte(&byte) != FMOD_OK)
            {
                return FMOD_ERR_FILE_EOF;
            }

            result = (result << 7) + (byte & 0x7F);
        } while (byte & 0x80);
    }

    *value = result;

    return FMOD_OK;
}

FMOD_RESULT MIDITrack::readByte(unsigned char * value)
{
    if (mPosition >= mLength)
    {
        mFinished = true;
        return FMOD_ERR_FILE_EOF;
    }

    *value = mData[mPosition++];

    return FMOD_OK;
}

FMOD_RESULT MIDITrack::read(void * buffer, unsigned int length)
{
    if (mPosition >= mLength)
    {
        mFinished = true;
        return FMOD_ERR_FILE_EOF;
    }

    if (mPosition + length > mLength)
    {
        length = mLength - mPosition;
    }

    if (buffer)
    {
        memcpy(buffer, mData + mPosition, length);
    }

    mPosition += length;

    return FMOD_OK;
}

FMOD_RESULT MIDITrack::readMetaData(const char * name, unsigned int length, bool store)
{
    FMOD_RESULT result;
    char * data;

    if (!store)
    {
        return read(0, length);
    }

    data = (char *)FMOD_Memory_Calloc(length);
    if (!data)
    {
        return FMOD_ERR_MEMORY;
    }

    result = read(data, length);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mMIDI->metaData(FMOD_TAGTYPE_MIDI, name, data, length, FMOD_TAGDATATYPE_STRING, false);

    FMOD_Memory_Free(data);

    return result;
}

FMOD_RESULT MIDITrack::process(bool audible)
{
    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::update(bool audible)
{
    int count;

    for (count = 0; count < mNumTracks; count++)
    {
        mTrack[count].process(false);
    }

    for (count = 0; count < 16; count++)
    {
        mMIDIChannel[count].update();
    }

    mUnk2950 += mUnk2928;
    mPCMOffset += mMixerSamplesPerTick;

    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::play()
{
    int count;

    for (count = 0; count < mNumTracks; count++)
    {
        mTrack[count].mPosition = 0;
        mTrack[count].mUnk18 = 0.0f;
        mTrack[count].mFinished = false;
        mTrack[count].mUnk1E = 0;
        mTrack[count].mUnk14 = 0;
    }

    mFreeVoiceHead.initNode();

    for (count = 0; count < mNumVoices; count++)
    {
        mVoice[count].initNode();
        mVoice[count].stop();
    }

    for (count = 0; count < 16; count++)
    {
        mMIDIChannel[count].mVoiceHead.initNode();
        mMIDIChannel[count].mUnk228 = count + 1;
        mMIDIChannel[count].mTrack = 0;
        mMIDIChannel[count].mPan = 64;
        mMIDIChannel[count].mVolume = 100;
        mMIDIChannel[count].mExpression = 127;
        mMIDIChannel[count].mUnk230 = 0;
        mMIDIChannel[count].mUnk234 = 0;
        mMIDIChannel[count].mPitchBendRange = 0x200;
        mMIDIChannel[count].mUnk238 = -1;
    }

    mUnk2950 = 0.0f;
    mPCMOffset = 0;

    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::closeInternal()
{
    if (mTrack)
    {
        int count;

        for (count = 0; count < mNumTracks; count++)
        {
            if (mTrack[count].mData)
            {
                FMOD_Memory_Free(mTrack[count].mData);
            }
        }

        FMOD_Memory_Free(mTrack);
    }

    if (mVoice)
    {
        FMOD_Memory_Free(mVoice);
        mVoice = 0;
    }

    if (mChannelPool)
    {
        mChannelPool->release();
        mChannelPool = 0;
    }

    if (mChannelSoftware)
    {
        FMOD_Memory_Free(mChannelSoftware);
        mChannelSoftware = 0;
    }

    if (mDSPHead)
    {
        mDSPHead->release(true);
        mDSPHead = 0;
    }

    if (mDLS)
    {
        mDLS->release();
        mDLS = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int numsamples;
    int numchannels;
    LocalCriticalSection criticalsection(mSystem->mDSPCrit);
    unsigned int mixedsofar;
    unsigned int mixedleft;
    unsigned int samplestomix;
    char * destptr;

    numchannels = mWaveFormat.channels;

    SoundI::getSamplesFromBytes(sizebytes, &numsamples, numchannels, mWaveFormat.format);

    mixedsofar = 0;
    mixedleft = mMixerSamplesLeft;
    destptr = (char *)buffer;

    while (mixedsofar < numsamples)
    {
        unsigned int read, bytes;
        void * buff = destptr;

        if (!mixedleft)
        {
            result = update(true);
            if (result != FMOD_OK)
            {
                return result;
            }

            samplestomix = mMixerSamplesPerTick;
            mixedleft = samplestomix;
        }
        else
        {
            samplestomix = mixedleft;
        }

        if (mixedsofar + samplestomix > numsamples)
        {
            samplestomix = numsamples - mixedsofar;
        }

        read = samplestomix;

        criticalsection.enter();
        if (buffer)
        {
            result = mDSPHead->execute(buff, &buff, &read, numchannels, &numchannels, FMOD_SPEAKERMODE_STEREO);
            if (result != FMOD_OK)
            {
                return result;
            }

            mDSPHead->resetVisited();
        }
        criticalsection.leave();

        SoundI::getBytesFromSamples(read, &bytes, numchannels, mWaveFormat.format);

        if (destptr != buff && buffer)
        {
            memcpy(destptr, buff, bytes);
        }

        mixedsofar += read;
        destptr += bytes;
        mixedleft -= read;
    }

    mMixerSamplesLeft = mixedleft;

    if (bytesread)
    {
        *bytesread = sizebytes;
    }

    return result;
}

FMOD_RESULT CodecMIDI::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    if (position == mPCMOffset)
    {
        return FMOD_OK;
    }

    if (position < mPCMOffset)
    {
        play();
    }

    while (mPCMOffset < position)
    {
        update(true);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMIDI::openCallback(FMOD_CODEC_STATE * codec_state, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecMIDI * cmidi = (CodecMIDI *)codec_state;

    return cmidi->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecMIDI::closeCallback(FMOD_CODEC_STATE * codec_state)
{
    CodecMIDI * cmidi = (CodecMIDI *)codec_state;

    return cmidi->closeInternal();
}

FMOD_RESULT CodecMIDI::readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecMIDI * cmidi = (CodecMIDI *)codec_state;

    return cmidi->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecMIDI::setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecMIDI * cmidi = (CodecMIDI *)codec_state;

    return cmidi->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
