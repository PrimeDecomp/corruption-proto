// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x8060CA08..0x8060D428 (13 native functions).
// Original basename from the Gormiti (later FMOD Ex, MWCC) debug information, which places MusicSong and
// the music tables/dummies in fmod_music.cpp. The music-channel callbacks and ChannelMusic of that
// version are absent here (no codec description slots, no hardware music channel in MusicSong).

// Reconstructed with the Gormiti debug information as reference; layouts are the G2MEAB ones (fmod_music.h).

#include "fmod_music.h"
#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_channelpool.h"
#include "fmod_codec.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include <string.h>

namespace FMOD {

MusicSample gDummySample;
MusicVirtualChannel gDummyVirtualChannel;
MusicChannel gDummyChannel;
MusicInstrument gDummyInstrument;

unsigned char gSineTable[32] =
{
    0, 24, 49, 74, 97, 120, 141, 161, 180, 197, 212, 224, 235, 244, 250, 253,
    255, 253, 250, 244, 235, 224, 212, 197, 180, 161, 141, 120, 97, 74, 49, 24
};

signed char gFineSineTable[256] =
{
    0, 2, 3, 5, 6, 8, 9, 11, 12, 14, 16, 17, 19, 20, 22, 23,
    24, 26, 27, 29, 30, 32, 33, 34, 36, 37, 38, 39, 41, 42, 43, 44,
    45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 56, 57, 58, 59,
    59, 60, 60, 61, 61, 62, 62, 62, 63, 63, 63, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 63, 63, 63, 62, 62, 62, 61, 61, 60, 60,
    59, 59, 58, 57, 56, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46,
    45, 44, 43, 42, 41, 39, 38, 37, 36, 34, 33, 32, 30, 29, 27, 26,
    24, 23, 22, 20, 19, 17, 16, 14, 12, 11, 9, 8, 6, 5, 3, 2,
    0, -2, -3, -5, -6, -8, -9, -11, -12, -14, -16, -17, -19, -20, -22, -23,
    -24, -26, -27, -29, -30, -32, -33, -34, -36, -37, -38, -39, -41, -42, -43, -44,
    -45, -46, -47, -48, -49, -50, -51, -52, -53, -54, -55, -56, -56, -57, -58, -59,
    -59, -60, -60, -61, -61, -62, -62, -62, -63, -63, -63, -64, -64, -64, -64, -64,
    -64, -64, -64, -64, -64, -64, -63, -63, -63, -62, -62, -62, -61, -61, -60, -60,
    -59, -59, -58, -57, -56, -56, -55, -54, -53, -52, -51, -50, -49, -48, -47, -46,
    -45, -44, -43, -42, -41, -39, -38, -37, -36, -34, -33, -32, -30, -29, -27, -26,
    -24, -23, -22, -20, -19, -17, -16, -14, -12, -11, -9, -8, -6, -5, -3, -2
};

unsigned int gPeriodTable[134] =
{
    27392, 25856, 24384, 23040, 21696, 20480, 19328, 18240, 17216, 16256, 15360, 14496,
    13696, 12928, 12192, 11520, 10848, 10240, 9664, 9120, 8608, 8128, 7680, 7248,
    6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4064, 3840, 3624,
    3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920, 1812,
    1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016, 960, 906,
    856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
    428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
    214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
    107, 101, 95, 90, 85, 80, 75, 71, 67, 63, 60, 56,
    53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30, 28,
    26, 25, 23, 22, 21, 20, 18, 17, 16, 15, 15, 14,
    0, 0
};

// IT period table, one octave below gPeriodTable (0x806EE380, read only by the IT pitch envelope 0x805C95FC).
unsigned int gITPeriodTable[144] = // Guessed name
{
    54784, 51712, 48768, 46080, 43392, 40960, 38656, 36480, 34432, 32512, 30720, 28992,
    27392, 25856, 24384, 23040, 21696, 20480, 19328, 18240, 17216, 16256, 15360, 14496,
    13696, 12928, 12192, 11520, 10848, 10240, 9664, 9120, 8608, 8128, 7680, 7248,
    6848, 6464, 6096, 5760, 5424, 5120, 4832, 4560, 4304, 4064, 3840, 3624,
    3424, 3232, 3048, 2880, 2712, 2560, 2416, 2280, 2152, 2032, 1920, 1812,
    1712, 1616, 1524, 1440, 1356, 1280, 1208, 1140, 1076, 1016, 960, 906,
    856, 808, 762, 720, 678, 640, 604, 570, 538, 508, 480, 453,
    428, 404, 381, 360, 339, 320, 302, 285, 269, 254, 240, 226,
    214, 202, 190, 180, 170, 160, 151, 143, 135, 127, 120, 113,
    107, 101, 95, 90, 85, 80, 75, 71, 67, 63, 60, 56,
    53, 50, 47, 45, 42, 40, 37, 35, 33, 31, 30, 28,
    26, 25, 23, 22, 21, 20, 18, 17, 16, 15, 15, 14
};

FMOD_RESULT MusicSong::play()
{
    FMOD_RESULT result;
    int count;

    result = stop();
    if (result != FMOD_OK)
    {
        return result;
    }

    mGlobalVolume = mDefaultGlobalVolume;
    mSpeed = mDefaultSpeed;
    mRow = 0;
    mOrder = 0;
    mNextOrder = 0;
    mNextRow = 0;
    mMixerSamplesLeft = 0;
    mTick = 0;
    mPatternDelay = 0;
    mPatternDelayTicks = 0;
    mPCMOffset = 0;
    mFinished = false;

    new (&mChannelGroup) ChannelGroupI();
    mChannelGroup.mDSPHead = mDSPHead;
    mChannelGroup.mVolume = 1.0f;

    if (mVisited)
    {
        memset(mVisited, 0, mNumOrders * 256);
    }

    setBPM(mDefaultBPM);

    if (mNumChannels && mMusicChannel)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            MusicChannel * cptr = mMusicChannel[count];

            memset(cptr, 0, sizeof(MusicChannel));
            cptr->mVirtualChannelHead.initNode();
            cptr->mGlobalVolume = mDefaultVolume[count];
            cptr->mPan = mDefaultPan[count];
        }
    }

    if (mNumVirtualChannels)
    {
        for (count = 0; count < mNumVirtualChannels; count++)
        {
            MusicVirtualChannel * vcptr = &mVirtualChannel[count];

            memset(vcptr, 0, sizeof(MusicVirtualChannel));
            vcptr->mChannel.init();
            vcptr->mIndex = count;
            vcptr->mChannel.mIndex = count;
            vcptr->mSong = this;
        }
    }

    mPlaying = true;
    return FMOD_OK;
}

FMOD_RESULT MusicSong::spawnNewVirtualChannel(MusicChannel * cptr, MusicSample * sptr, MusicVirtualChannel * * newvcptr)
{
    MusicVirtualChannel * vcptr = 0;
    int count;

    for (count = 0; count < mNumVirtualChannels; count++)
    {
        if (!mVirtualChannel[count].mAllocated)
        {
            vcptr = &mVirtualChannel[count];
            vcptr->mAllocated = true;
            break;
        }
    }

    if (!vcptr)
    {
        return FMOD_ERR_INTERNAL;
    }

    vcptr->addAfter(&cptr->mVirtualChannelHead);

    vcptr->mBackground = false;

    vcptr->mEnvVolume.mTick = 0;
    vcptr->mEnvVolume.mPosition = 0;
    vcptr->mEnvVolume.mValue = 64;
    vcptr->mEnvVolume.mFraction = 64 << 16;
    vcptr->mEnvVolume.mDelta = 0;
    vcptr->mEnvVolume.mStopped = false;

    vcptr->mEnvPan.mTick = 0;
    vcptr->mEnvPan.mPosition = 0;
    vcptr->mEnvPan.mValue = 128;
    vcptr->mEnvPan.mFraction = 128 << 16;
    vcptr->mEnvPan.mDelta = 0;
    vcptr->mEnvPan.mStopped = false;

    vcptr->mEnvPitchTick = 0;
    vcptr->mEnvPitchPos = 0;
    vcptr->mEnvPitchFrac = 0;
    vcptr->mEnvPitch = 0;
    vcptr->mEnvPitchDelta = 0;
    vcptr->mEnvPitchStopped = false;

    vcptr->mFadeOutVolume = 1024;

    if (newvcptr)
    {
        *newvcptr = vcptr;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicVirtualChannel::cleanUp()
{
    bool playing = false;

    mChannel.isPlaying(&playing);
    if (!playing)
    {
        if (mSong->mLowPass)
        {
            mSong->mLowPass[mChannel.mIndex]->remove();
        }

        mNoteControl = 0;
        removeNode();
        mAllocated = false;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::setBPM(int bpm)
{
    float hz;

    if (bpm < 1)
    {
        bpm = 1;
    }

    mBPM = bpm;

    hz = 2.0f * (float)bpm / 5.0f * mMasterSpeed;
    if (hz >= 0.01f)
    {
        mMixerSamplesPerTick = (int)((float)mWaveFormat.frequency / hz);
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::stop()
{
    int count;

    mPlaying = false;
    mFinished = true;

    if (mMusicChannel)
    {
        for (count = 0; count < mNumChannels; count++)
        {
            MusicChannel * cptr = mMusicChannel[count];

            if (cptr && cptr->mVirtualChannelHead.getNext())
            {
                while (!cptr->mVirtualChannelHead.isEmpty())
                {
                    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();

                    vcptr->mChannel.stopEx(false, false, true, true, false);
                    vcptr->mChannel.mRealChannel[0] = 0;

                    if (mLowPass)
                    {
                        mLowPass[vcptr->mChannel.mIndex]->remove();
                    }

                    vcptr->cleanUp();
                }
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::playSound(MusicSample * sample, MusicVirtualChannel * vcptr, bool addfilter, _SNDMIXPLUGIN * plugin)
{
    FMOD_RESULT result;
    ChannelI * channel = &vcptr->mChannel;
    ChannelReal * realchannel = channel->mRealChannel[0];

    result = mChannelPool->allocateChannel(&realchannel, vcptr->mFlip ? vcptr->mIndex + mNumVirtualChannels : vcptr->mIndex, 1, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    vcptr->mFlip = !vcptr->mFlip;

    if (channel->mRealChannel[0])
    {
        vcptr->mChannel.setVolume(0.0f);
    }

    channel->mRealChannel[0] = realchannel;

    if (plugin)
    {
        channel->mChannelGroup = &plugin->mChannelGroup;
    }
    else
    {
        channel->mChannelGroup = &mChannelGroup;
    }

    result = channel->play(sample->mSound, true, true);
    if (result != FMOD_OK)
    {
        channel->stopEx(false, false, true, true, false);
        return result;
    }

    if (vcptr->mSampleOffset)
    {
        channel->setPosition(vcptr->mSampleOffset, FMOD_TIMEUNIT_PCM);
        vcptr->mSampleOffset = 0;
    }

    if (mLowPass)
    {
        mLowPass[channel->mIndex]->remove();

        if (addfilter)
        {
            channel->addDSP(mLowPass[channel->mIndex]);
        }
    }

    channel->setPaused(false);

    return FMOD_OK;
}

FMOD_RESULT MusicSong::fineTune2Hz(unsigned char finetune, unsigned int * hz)
{
    if (!hz)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    switch (finetune)
    {
        case 0:
            *hz = 8363;
            break;
        case 1:
            *hz = 8413;
            break;
        case 2:
            *hz = 8463;
            break;
        case 3:
            *hz = 8529;
            break;
        case 4:
            *hz = 8581;
            break;
        case 5:
            *hz = 8651;
            break;
        case 6:
            *hz = 8723;
            break;
        case 7:
            *hz = 8757;
            break;
        case 8:
            *hz = 7895;
            break;
        case 9:
            *hz = 7941;
            break;
        case 10:
            *hz = 7985;
            break;
        case 11:
            *hz = 8046;
            break;
        case 12:
            *hz = 8107;
            break;
        case 13:
            *hz = 8169;
            break;
        case 14:
            *hz = 8232;
            break;
        case 15:
            *hz = 8280;
            break;
        default:
            *hz = 8363;
            break;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::getLengthInternal(unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
    if (lengthtype == FMOD_TIMEUNIT_MODORDER)
    {
        *length = mNumOrders;
        return FMOD_OK;
    }

    if (lengthtype == FMOD_TIMEUNIT_MODPATTERN)
    {
        *length = mNumPatterns;
        return FMOD_OK;
    }

    if (lengthtype == FMOD_TIMEUNIT_MODROW)
    {
        *length = mPattern[mOrderList[mOrder]].mRows;
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::getPositionInternal(unsigned int * position, FMOD_TIMEUNIT postype)
{
    if (postype == FMOD_TIMEUNIT_MODORDER)
    {
        *position = mOrder;
        return FMOD_OK;
    }

    if (postype == FMOD_TIMEUNIT_MODPATTERN)
    {
        *position = mOrderList[mOrder];
        return FMOD_OK;
    }

    if (postype == FMOD_TIMEUNIT_MODROW)
    {
        *position = mRow;
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicSong::getLengthCallback(FMOD_CODEC_STATE * codec_state, unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
    MusicSong * cmusic = (MusicSong *)codec_state;

    return cmusic->getLengthInternal(length, lengthtype);
}

FMOD_RESULT MusicSong::getPositionCallback(FMOD_CODEC_STATE * codec_state, unsigned int * position, FMOD_TIMEUNIT postype)
{
    MusicSong * cmusic = (MusicSong *)codec_state;

    return cmusic->getPositionInternal(position, postype);
}

} // namespace FMOD
