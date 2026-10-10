// G2MEAB prototype translation unit; complete reconstruction (all native functions have bodies).
// G2MEAB .text: 0x805E3420..0x805E733C (19 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: S3M descriptor80753494 registration3420, format11/state1CAC, binds7218/7244/7270/729C
// to5114/6AE4/6CF4/7148. Allocation/free in open5114 and close6AE4 directly name
// fmod_codec_s3m.cpp. Duration34D8, volume/portamento/vibrato/tremolo helpers3544..38EC, row3A20,
// tick45F8 and advance4FC0 are the complete local effect group used by read/seek. Initializer72C8
// registers same descriptor and ends733C, Tag Reader registration. Preserve every retained
// callback, emitted helper and initializer; complete inventory and inlining uncertainty are
// recorded externally.

// Reconstructed with the XM codec as template (no reference debug information covers S3M). The class and
// helper names are descriptive guesses; see fmod_codec_s3m.h.

#include "fmod_codec_s3m.h"
#include "fmod.h"
#include "fmod_channel_software.h"
#include "fmod_channelpool.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_music.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX s3mcodec; // Guessed name

FMOD_CODEC_DESCRIPTION_EX * CodecS3M::getDescriptionEx()
{
    memset(&s3mcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    s3mcodec.name = "FMOD S3M Codec";
    s3mcodec.version = 0x00010100;
    s3mcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_MODORDER | FMOD_TIMEUNIT_MODROW | FMOD_TIMEUNIT_MODPATTERN;
    s3mcodec.defaultasstream = 1;
    s3mcodec.open = &CodecS3M::openCallback;
    s3mcodec.close = &CodecS3M::closeCallback;
    s3mcodec.read = &CodecS3M::readCallback;
    s3mcodec.getlength = &MusicSong::getLengthCallback;
    s3mcodec.setposition = &CodecS3M::setPositionCallback;
    s3mcodec.getposition = &MusicSong::getPositionCallback;

    s3mcodec.mType = FMOD_SOUND_TYPE_S3M;
    s3mcodec.mSize = sizeof(CodecS3M);

    return &s3mcodec;
}

FMOD_RESULT CodecS3M::calculateLength()
{
    mWaveFormat.lengthpcm = 0;

    play();

    while (!mFinished)
    {
        update(false);

        mWaveFormat.lengthpcm += mMixerSamplesPerTick;
    }

    stop();

    return FMOD_OK;
}

FMOD_RESULT MusicChannelS3M::volumeSlide()
{
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    if (!(mVolumeSlide & 0xF))
    {
        vcptr->mVolume += (mVolumeSlide >> 4);
    }
    if (!(mVolumeSlide >> 4))
    {
        vcptr->mVolume -= (mVolumeSlide & 0xF);
    }

    if (vcptr->mVolume > 64)
    {
        vcptr->mVolume = 64;
    }
    if (vcptr->mVolume < 0)
    {
        vcptr->mVolume = 0;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelS3M::portamento()
{
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    if (vcptr->mFrequency < mPortaTarget)
    {
        vcptr->mFrequency += mPortaSpeed << 2;
        if (vcptr->mFrequency > mPortaTarget)
        {
            vcptr->mFrequency = mPortaTarget;
        }
    }
    if (vcptr->mFrequency > mPortaTarget)
    {
        vcptr->mFrequency -= mPortaSpeed << 2;
        if (vcptr->mFrequency < mPortaTarget)
        {
            vcptr->mFrequency = mPortaTarget;
        }
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelS3M::vibrato()
{
    int delta;
    unsigned char temp;
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    temp = (mVibPos & 31);

    switch (mWaveControl & 3)
    {
        case 0:
        {
            delta = gSineTable[temp];
            break;
        }
        case 1:
        {
            temp <<= 3;
            if (mVibPos < 0)
            {
                temp = 255 - temp;
            }
            delta = temp;
            break;
        }
        case 2:
        {
            delta = 255;
            break;
        }
        case 3:
        {
            delta = rand() & 255;
            break;
        }
        default:
        {
            delta = 0;
            break;
        }
    }

    delta *= mVibDepth;
    delta >>= 7;
    delta <<= 2;

    if (mVibPos >= 0)
    {
        vcptr->mFrequencyDelta = delta;
    }
    else
    {
        vcptr->mFrequencyDelta = -delta;
    }

    mVibPos += mVibSpeed;
    if (mVibPos > 31)
    {
        mVibPos -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelS3M::tremolo()
{
    int delta;
    unsigned char temp;
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    temp = (mTremoloPosition & 31);

    switch ((mWaveControl >> 4) & 3)
    {
        case 0:
        {
            delta = gSineTable[temp];
            break;
        }
        case 1:
        {
            temp <<= 3;
            if (mTremoloPosition < 0)
            {
                temp = 255 - temp;
            }
            delta = temp;
            break;
        }
        case 2:
        {
            delta = 255;
            break;
        }
        case 3:
        {
            delta = rand() & 255;
            break;
        }
        default:
        {
            delta = 0;
            break;
        }
    }

    delta *= mTremoloDepth;
    delta >>= 6;

    if (mTremoloPosition >= 0)
    {
        if (vcptr->mVolume + delta > 64)
        {
            delta = 64 - vcptr->mVolume;
        }
        vcptr->mVolumeDelta = delta;
    }
    else
    {
        if ((short)vcptr->mVolume - delta < 0)
        {
            delta = vcptr->mVolume;
        }
        vcptr->mVolumeDelta = -delta;
    }

    mTremoloPosition += mTremoloDepth;
    if (mTremoloPosition > 31)
    {
        mTremoloPosition -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelS3M::fineVibrato()
{
    int delta;
    unsigned char temp;
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    temp = (mVibPos & 31);

    switch (mWaveControl & 3)
    {
        case 0:
        {
            delta = gSineTable[temp];
            break;
        }
        case 1:
        {
            temp <<= 3;
            if (mVibPos < 0)
            {
                temp = 255 - temp;
            }
            delta = temp;
            break;
        }
        case 2:
        {
            delta = 255;
            break;
        }
        case 3:
        {
            delta = rand() & 255;
            break;
        }
        default:
        {
            delta = 0;
            break;
        }
    }

    delta *= mVibDepth;
    delta >>= 7;

    if (mVibPos >= 0)
    {
        vcptr->mFrequencyDelta = delta;
    }
    else
    {
        vcptr->mFrequencyDelta = -delta;
    }

    mVibPos += mVibSpeed;
    if (mVibPos > 31)
    {
        mVibPos -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT CodecS3M::updateNote(bool audible)
{
    MusicNote * current;
    bool jumpflag = false;
    int count;

    current = mPattern[mOrderList[mOrder]].mData + (mRow * mNumChannels);
    if (!current)
    {
        return FMOD_OK;
    }

    if (mVisited)
    {
        if (mVisited[(mOrder * 256) + mRow])
        {
            mFinished = true;
            return FMOD_OK;
        }
        mVisited[(mOrder * 256) + mRow] = true;
    }

    for (count = 0; count < mNumChannels; count++, current++)
    {
        MusicChannelS3M * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicSample * sptr;
        unsigned char paramx, paramy;
        int oldvolume, oldfreq;

        paramx = current->mEffectParam >> 4;
        paramy = current->mEffectParam & 0xF;

        cptr = (MusicChannelS3M *)mMusicChannel[count];

        if (cptr->mVirtualChannelHead.isEmpty())
        {
            vcptr = &gDummyVirtualChannel;
            vcptr->mSample = &gDummySample;
        }
        else
        {
            vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();
        }

        if (current->mNumber)
        {
            cptr->mInstrument = current->mNumber - 1;
        }

        if (current->mNote && current->mNote != 0xFF)
        {
            cptr->mNote = current->mNote - 1;
        }

        if (cptr->mInstrument < mNumSamples)
        {
            sptr = &mSample[cptr->mInstrument];
        }
        else
        {
            sptr = &gDummySample;
        }

        oldvolume = vcptr->mVolume;
        oldfreq = vcptr->mFrequency;

        if (cptr->mRecentEffect == FMUSIC_S3M_TREMOLO && current->mEffect != FMUSIC_S3M_TREMOLO)
        {
            vcptr->mVolume = oldvolume + vcptr->mVolumeDelta;
        }
        cptr->mRecentEffect = current->mEffect;

        vcptr->mVolumeDelta = 0;
        vcptr->mNoteControl = 0;

        if (current->mNote && current->mNote != 0xFF)
        {
            if (vcptr == &gDummyVirtualChannel)
            {
                if (spawnNewVirtualChannel(cptr, sptr, &vcptr) != FMOD_OK)
                {
                    vcptr = &gDummyVirtualChannel;
                    vcptr->mSample = &gDummySample;
                }
            }

            cptr->mNote = current->mNote - 1;

            if (sptr->mMiddleC)
            {
                cptr->mPeriod = gPeriodTable[cptr->mNote] * 8363 / sptr->mMiddleC;
            }
            else
            {
                cptr->mPeriod = gPeriodTable[cptr->mNote];
            }

            vcptr->mPan = mDefaultPan[count];

            if (current->mEffect != FMUSIC_S3M_PORTATO && current->mEffect != FMUSIC_S3M_PORTATOVOLSLIDE)
            {
                vcptr->mFrequency = cptr->mPeriod;
            }

            vcptr->mNoteControl = FMUSIC_TRIGGER;
        }

        if (current->mNumber)
        {
            vcptr->mVolume = sptr->mDefaultVolume;
            cptr->mTremorPosition = 0;

            if ((cptr->mWaveControl & 0xF) < 4)
            {
                cptr->mVibPos = 0;
            }
            if ((cptr->mWaveControl >> 4) < 4)
            {
                cptr->mTremoloPosition = 0;
            }
        }

        vcptr->mFrequencyDelta = 0;
        vcptr->mNoteControl |= (FMUSIC_FREQ | FMUSIC_VOLUME | FMUSIC_PAN);

        if (current->mVolume)
        {
            vcptr->mVolume = current->mVolume - 1;
        }

        if (current->mNote == 0xFF)
        {
            vcptr->mVolume = 0;
        }

        switch (current->mEffect)
        {
            case FMUSIC_S3M_SETSPEED:
            {
                if (current->mEffectParam)
                {
                    mSpeed = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_S3M_PATTERNJUMP:
            {
                mNextOrder = current->mEffectParam;
                mNextRow = 0;

                if (mNextOrder >= mNumOrders)
                {
                    mNextOrder = 0;
                }

                jumpflag = true;
                break;
            }
            case FMUSIC_S3M_PATTERNBREAK:
            {
                mNextRow = (paramx * 10) + paramy;
                if (mNextRow > 63)
                {
                    mNextRow = 0;
                }

                if (!jumpflag)
                {
                    mNextOrder = mOrder + 1;
                }

                if (mNextOrder >= mNumOrders)
                {
                    mNextOrder = 0;
                }
                break;
            }
            case FMUSIC_S3M_VOLUMESLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }

                if ((cptr->mVolumeSlide & 0xF) == 0xF)
                {
                    vcptr->mVolume += (cptr->mVolumeSlide >> 4);
                }
                else if ((cptr->mVolumeSlide >> 4) == 0xF)
                {
                    vcptr->mVolume -= (cptr->mVolumeSlide & 0xF);
                }

                if (mMusicFlags == 1)
                {
                    if (!(cptr->mVolumeSlide & 0xF))
                    {
                        vcptr->mVolume += (cptr->mVolumeSlide >> 4);
                    }
                    if (!(cptr->mVolumeSlide >> 4))
                    {
                        vcptr->mVolume -= (cptr->mVolumeSlide & 0xF);
                    }
                }

                if (vcptr->mVolume > 64)
                {
                    vcptr->mVolume = 64;
                }
                if (vcptr->mVolume < 0)
                {
                    vcptr->mVolume = 0;
                }
                break;
            }
            case FMUSIC_S3M_PORTADOWN:
            {
                if (current->mEffectParam)
                {
                    cptr->mPortaUpDown = current->mEffectParam;
                }

                if ((cptr->mPortaUpDown >> 4) == 0xF)
                {
                    vcptr->mFrequency += ((cptr->mPortaUpDown & 0xF) << 2);
                }
                if ((cptr->mPortaUpDown >> 4) == 0xE)
                {
                    vcptr->mFrequency += (cptr->mPortaUpDown & 0xF);
                }
                break;
            }
            case FMUSIC_S3M_PORTAUP:
            {
                if (current->mEffectParam)
                {
                    cptr->mPortaUpDown = current->mEffectParam;
                }

                if ((cptr->mPortaUpDown >> 4) == 0xF)
                {
                    vcptr->mFrequency -= ((cptr->mPortaUpDown & 0xF) << 2);
                }
                if ((cptr->mPortaUpDown >> 4) == 0xE)
                {
                    vcptr->mFrequency -= (cptr->mPortaUpDown & 0xF);
                }
                break;
            }
            case FMUSIC_S3M_PORTATO:
            {
                if (current->mEffectParam)
                {
                    cptr->mPortaSpeed = current->mEffectParam;
                }
                cptr->mPortaTarget = cptr->mPeriod;
                vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                vcptr->mNoteControl &= ~FMUSIC_FREQ;
                break;
            }
            case FMUSIC_S3M_VIBRATO:
            {
                if (paramx)
                {
                    cptr->mVibSpeed = paramx;
                }
                if (paramy)
                {
                    cptr->mVibDepth = paramy;
                }
                break;
            }
            case FMUSIC_S3M_TREMOR:
            {
                if (current->mEffectParam)
                {
                    cptr->mTremorOn = (paramx + 1);
                    cptr->mTremorOff = (paramy + 1);
                }

                if (cptr->mTremorPosition >= cptr->mTremorOn)
                {
                    vcptr->mVolumeDelta = -vcptr->mVolume;
                }

                cptr->mTremorPosition++;
                if (cptr->mTremorPosition >= (cptr->mTremorOn + cptr->mTremorOff))
                {
                    cptr->mTremorPosition = 0;
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_S3M_PORTATOVOLSLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }
                cptr->mPortaTarget = cptr->mPeriod;
                vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                vcptr->mNoteControl &= ~FMUSIC_FREQ;
                break;
            }
            case FMUSIC_S3M_VIBRATOVOLSLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_S3M_ARPEGGIO:
            {
                if (current->mEffectParam)
                {
                    cptr->mArpeggio = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_S3M_SETSAMPLEOFFSET:
            {
                unsigned int offset;

                offset = current->mEffectParam << 8;

                if (offset >= sptr->mLoopStart + sptr->mLoopLength)
                {
                    vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                    vcptr->mNoteControl |= FMUSIC_STOP;
                }
                else
                {
                    vcptr->mSampleOffset = offset;
                }
                break;
            }
            case FMUSIC_S3M_RETRIGVOLSLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mRetrigX = paramx;
                    cptr->mRetrigY = paramy;
                }
                break;
            }
            case FMUSIC_S3M_TREMOLO:
            {
                if (paramx)
                {
                    cptr->mTremoloDepth = paramx;
                }
                if (paramy)
                {
                    cptr->mTremoloDepth = paramy;
                }
                break;
            }
            case FMUSIC_S3M_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_S3M_SETFINETUNE:
                    {
                        fineTune2Hz(paramy, &sptr->mMiddleC);
                        break;
                    }
                    case FMUSIC_S3M_SETVIBRATOWAVE:
                    {
                        cptr->mWaveControl &= 0xF0;
                        cptr->mWaveControl |= paramy;
                        break;
                    }
                    case FMUSIC_S3M_SETTREMOLOWAVE:
                    {
                        cptr->mWaveControl &= 0xF;
                        cptr->mWaveControl |= (paramy << 4);
                        break;
                    }
                    case FMUSIC_S3M_SETPANPOSITION16:
                    {
                        vcptr->mPan = paramy << 4;
                        vcptr->mNoteControl |= FMUSIC_PAN;
                        break;
                    }
                    case FMUSIC_S3M_STEREOCONTROL:
                    {
                        if (paramy > 7)
                        {
                            paramy -= 8;
                        }
                        else
                        {
                            paramy += 8;
                        }

                        vcptr->mPan = paramy << 4;
                        vcptr->mNoteControl |= FMUSIC_PAN;
                        break;
                    }
                    case FMUSIC_S3M_PATTERNLOOP:
                    {
                        if (!paramy)
                        {
                            cptr->mPatternLoopRow = mRow;
                        }
                        else
                        {
                            if (!cptr->mPatternLoopNumber)
                            {
                                cptr->mPatternLoopNumber = paramy;
                            }
                            else
                            {
                                cptr->mPatternLoopNumber--;
                            }

                            if (cptr->mPatternLoopNumber)
                            {
                                mNextRow = cptr->mPatternLoopRow;

                                if (mVisited)
                                {
                                    int count2;

                                    for (count2 = cptr->mPatternLoopRow; count2 <= mRow; count2++)
                                    {
                                        mVisited[(mOrder * 256) + count2] = false;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case FMUSIC_S3M_NOTEDELAY:
                    {
                        vcptr->mVolume = oldvolume;
                        vcptr->mFrequency = oldfreq;
                        vcptr->mNoteControl &= ~FMUSIC_FREQ;
                        vcptr->mNoteControl &= ~FMUSIC_PAN;
                        vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                        vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                        break;
                    }
                    case FMUSIC_S3M_PATTERNDELAY:
                    {
                        mPatternDelay = paramy;
                        mPatternDelay *= mSpeed;
                        break;
                    }
                }
                break;
            }
            case FMUSIC_S3M_FINEVIBRATO:
            {
                if (paramx)
                {
                    cptr->mVibSpeed = paramx;
                }
                if (paramy)
                {
                    cptr->mVibDepth = paramy;
                }
                break;
            }
            case FMUSIC_S3M_GLOBALVOLUME:
            {
                mGlobalVolume = current->mEffectParam;
                if (mGlobalVolume > 64)
                {
                    mGlobalVolume = 64;
                }
                break;
            }
            case FMUSIC_S3M_SETTEMPO:
            {
                if (current->mEffectParam >= 0x20)
                {
                    setBPM(current->mEffectParam);
                }
                break;
            }
            case FMUSIC_S3M_SETPAN:
            {
                vcptr->mPan = current->mEffectParam << 1;
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case FMUSIC_S3M_Z: // no-op case: the jump table 0x806EB790 runs to 0x1A
            {
                break;
            }
        }

        if (audible)
        {
            vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();

            if (vcptr->mFrequency + vcptr->mFrequencyDelta == 0)
            {
                vcptr->mNoteControl &= ~FMUSIC_FREQ;
            }

            if (vcptr->mNoteControl & FMUSIC_TRIGGER)
            {
                playSound(sptr, vcptr, false, 0);
            }

            if (vcptr->mNoteControl & FMUSIC_VOLUME)
            {
                vcptr->mChannel.setVolume(0.5f * ((float)(mGlobalVolume * (vcptr->mVolume + vcptr->mVolumeDelta)) / 4096.0f));
            }

            if (vcptr->mNoteControl & FMUSIC_PAN)
            {
                vcptr->mChannel.setPan(((float)vcptr->mPan - 128.0f) * mPanSeparation / 128.0f, true);
            }

            if (vcptr->mNoteControl & FMUSIC_FREQ)
            {
                int finalfreq;

                finalfreq = vcptr->mFrequency + vcptr->mFrequencyDelta;
                if (finalfreq < 1)
                {
                    finalfreq = 1;
                }

                vcptr->mChannel.setFrequency((float)(14317056 / finalfreq));
            }

            if (vcptr->mNoteControl & FMUSIC_STOP)
            {
                vcptr->mChannel.stopEx(false, false, true, true, false);
                vcptr->mSampleOffset = 0;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecS3M::updateEffects()
{
    MusicNote * current;
    int count;

    current = mPattern[mOrderList[mOrder]].mData + (mRow * mNumChannels);
    if (!current)
    {
        return FMOD_OK;
    }

    for (count = 0; count < mNumChannels; count++, current++)
    {
        MusicChannelS3M * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicSample * sptr;
        unsigned char effect, paramx, paramy;

        cptr = (MusicChannelS3M *)mMusicChannel[count];

        if (cptr->mInstrument < mNumSamples)
        {
            sptr = &mSample[cptr->mInstrument];
        }
        else
        {
            sptr = &gDummySample;
        }

        if (cptr->mVirtualChannelHead.isEmpty())
        {
            vcptr = &gDummyVirtualChannel;
        }
        else
        {
            vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();
        }

        effect = current->mEffect;
        paramx = current->mEffectParam >> 4;
        paramy = current->mEffectParam & 0xF;

        vcptr->mVolumeDelta = 0;
        vcptr->mFrequencyDelta = 0;
        vcptr->mNoteControl = 0;

        switch (effect)
        {
            case FMUSIC_S3M_VOLUMESLIDE:
            {
                cptr->volumeSlide();
                break;
            }
            case FMUSIC_S3M_PORTADOWN:
            {
                if (cptr->mPortaUpDown < 0xE0)
                {
                    vcptr->mFrequency += (cptr->mPortaUpDown << 2);
                }
                vcptr->mNoteControl |= FMUSIC_FREQ;
                break;
            }
            case FMUSIC_S3M_PORTAUP:
            {
                if (cptr->mPortaUpDown < 0xE0)
                {
                    vcptr->mFrequency -= (cptr->mPortaUpDown << 2);

                    if (vcptr->mFrequency < 1)
                    {
                        vcptr->mNoteControl |= FMUSIC_STOP;
                    }
                    else
                    {
                        vcptr->mNoteControl |= FMUSIC_FREQ;
                    }
                }
                break;
            }
            case FMUSIC_S3M_PORTATO:
            {
                cptr->portamento();
                break;
            }
            case FMUSIC_S3M_VIBRATO:
            {
                cptr->vibrato();
                break;
            }
            case FMUSIC_S3M_TREMOR:
            {
                if (cptr->mTremorPosition >= cptr->mTremorOn)
                {
                    vcptr->mVolumeDelta = -vcptr->mVolume;
                }

                cptr->mTremorPosition++;
                if (cptr->mTremorPosition >= (cptr->mTremorOn + cptr->mTremorOff))
                {
                    cptr->mTremorPosition = 0;
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_S3M_ARPEGGIO:
            {
                if (cptr->mArpeggio)
                {
                    paramx = cptr->mArpeggio >> 4;
                    paramy = cptr->mArpeggio & 0xF;

                    switch (mTick % 3)
                    {
                        case 1:
                        {
                            if (sptr->mMiddleC)
                            {
                                vcptr->mFrequencyDelta = (gPeriodTable[cptr->mNote + paramx] * 8363 / sptr->mMiddleC) - (gPeriodTable[cptr->mNote] * 8363 / sptr->mMiddleC);
                            }
                            else
                            {
                                vcptr->mFrequencyDelta = gPeriodTable[cptr->mNote + paramx] - gPeriodTable[cptr->mNote];
                            }
                            break;
                        }
                        case 2:
                        {
                            if (sptr->mMiddleC)
                            {
                                vcptr->mFrequencyDelta = (gPeriodTable[cptr->mNote + paramy] * 8363 / sptr->mMiddleC) - (gPeriodTable[cptr->mNote] * 8363 / sptr->mMiddleC);
                            }
                            else
                            {
                                vcptr->mFrequencyDelta = gPeriodTable[cptr->mNote + paramy] - gPeriodTable[cptr->mNote];
                            }
                            break;
                        }
                    }
                    vcptr->mNoteControl |= FMUSIC_FREQ;
                }
                break;
            }
            case FMUSIC_S3M_VIBRATOVOLSLIDE:
            {
                cptr->vibrato();
                cptr->volumeSlide();
                break;
            }
            case FMUSIC_S3M_PORTATOVOLSLIDE:
            {
                cptr->portamento();
                cptr->volumeSlide();
                break;
            }
            case FMUSIC_S3M_RETRIGVOLSLIDE:
            {
                if (!cptr->mRetrigY)
                {
                    break;
                }

                if (!(mTick % cptr->mRetrigY))
                {
                    if (cptr->mRetrigX)
                    {
                        switch (cptr->mRetrigX)
                        {
                            case 1:
                            {
                                vcptr->mVolume -= 1;
                                break;
                            }
                            case 2:
                            {
                                vcptr->mVolume -= 2;
                                break;
                            }
                            case 3:
                            {
                                vcptr->mVolume -= 4;
                                break;
                            }
                            case 4:
                            {
                                vcptr->mVolume -= 8;
                                break;
                            }
                            case 5:
                            {
                                vcptr->mVolume -= 16;
                                break;
                            }
                            case 6:
                            {
                                vcptr->mVolume = 0; // stores 0 (0x805E4A88); FMOD 3 scales by 2/3 here
                                break;
                            }
                            case 7:
                            {
                                vcptr->mVolume >>= 1;
                                break;
                            }
                            case 9:
                            {
                                vcptr->mVolume += 1;
                                break;
                            }
                            case 0xA:
                            {
                                vcptr->mVolume += 2;
                                break;
                            }
                            case 0xB:
                            {
                                vcptr->mVolume += 4;
                                break;
                            }
                            case 0xC:
                            {
                                vcptr->mVolume += 8;
                                break;
                            }
                            case 0xD:
                            {
                                vcptr->mVolume += 16;
                                break;
                            }
                            case 0xF:
                            {
                                vcptr->mVolume <<= 1;
                                break;
                            }
                        }

                        if (vcptr->mVolume > 64)
                        {
                            vcptr->mVolume = 64;
                        }
                        if (vcptr->mVolume < 0)
                        {
                            vcptr->mVolume = 0;
                        }
                    }

                    vcptr->mPan = mDefaultPan[count];
                    vcptr->mFrequency = cptr->mPeriod;
                    vcptr->mFrequencyDelta = 0;
                    vcptr->mNoteControl |= FMUSIC_VOLUME;
                    vcptr->mNoteControl |= FMUSIC_PAN;
                    vcptr->mNoteControl |= FMUSIC_FREQ;
                    vcptr->mNoteControl |= FMUSIC_TRIGGER;
                }
                break;
            }
            case FMUSIC_S3M_TREMOLO:
            {
                cptr->tremolo();
                break;
            }
            case FMUSIC_S3M_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_S3M_NOTECUT:
                    {
                        if (mTick == paramy)
                        {
                            vcptr->mVolume = 0;
                            vcptr->mNoteControl |= FMUSIC_VOLUME;
                        }
                        break;
                    }
                    case FMUSIC_S3M_NOTEDELAY:
                    {
                        if (mTick == paramy)
                        {
                            if (vcptr == &gDummyVirtualChannel)
                            {
                                if (spawnNewVirtualChannel(cptr, sptr, &vcptr) != FMOD_OK)
                                {
                                    vcptr = &gDummyVirtualChannel;
                                    vcptr->mSample = &gDummySample;
                                }
                            }

                            if (current->mNumber)
                            {
                                vcptr->mVolume = sptr->mDefaultVolume;

                                if ((cptr->mWaveControl & 0xF) < 4)
                                {
                                    cptr->mVibPos = 0;
                                }
                                if ((cptr->mWaveControl >> 4) < 4)
                                {
                                    cptr->mTremoloPosition = 0;
                                }

                                cptr->mTremorPosition = 0;
                                vcptr->mNoteControl |= FMUSIC_VOLUME;
                            }

                            vcptr->mPan = mDefaultPan[count];
                            vcptr->mFrequency = cptr->mPeriod;
                            vcptr->mFrequencyDelta = 0;
                            vcptr->mNoteControl |= FMUSIC_FREQ;
                            vcptr->mNoteControl |= FMUSIC_PAN;

                            if (current->mVolume)
                            {
                                vcptr->mVolume = current->mVolume - 1;
                                vcptr->mNoteControl |= FMUSIC_VOLUME;
                            }

                            vcptr->mNoteControl |= FMUSIC_TRIGGER;
                        }
                        else
                        {
                            vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                            vcptr->mNoteControl &= ~FMUSIC_FREQ;
                            vcptr->mNoteControl &= ~FMUSIC_PAN;
                            vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                        }
                        break;
                    }
                }
                break;
            }
            case FMUSIC_S3M_FINEVIBRATO:
            {
                cptr->fineVibrato();
                break;
            }
        }

        vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();

        if (vcptr->mFrequency + vcptr->mFrequencyDelta == 0)
        {
            vcptr->mNoteControl &= ~FMUSIC_FREQ;
        }

        if (vcptr->mNoteControl & FMUSIC_TRIGGER)
        {
            playSound(sptr, vcptr, false, 0);
        }

        if (vcptr->mNoteControl & FMUSIC_VOLUME)
        {
            vcptr->mChannel.setVolume(0.5f * ((float)(mGlobalVolume * (vcptr->mVolume + vcptr->mVolumeDelta)) / 4096.0f));
        }

        if (vcptr->mNoteControl & FMUSIC_PAN)
        {
            vcptr->mChannel.setPan(((float)vcptr->mPan - 128.0f) * mPanSeparation / 128.0f, true);
        }

        if (vcptr->mNoteControl & FMUSIC_FREQ)
        {
            int finalfreq;

            finalfreq = vcptr->mFrequency + vcptr->mFrequencyDelta;
            if (finalfreq < 1)
            {
                finalfreq = 1;
            }

            vcptr->mChannel.setFrequency((float)(14317056 / finalfreq));
        }

        if (vcptr->mNoteControl & FMUSIC_STOP)
        {
            vcptr->mChannel.stopEx(false, false, true, true, false);
            vcptr->mSampleOffset = 0;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecS3M::update(bool audible)
{
    if (!mTick)
    {
        if (mFinished && !mLooping)
        {
            stop();
        }
        else
        {
            if (mNextOrder >= 0)
            {
                mOrder = mNextOrder;
                if (mNextOrder >= 0)
                {
                    mOrder = mNextOrder;
                }
                mNextOrder = -1;
            }
            if (mNextRow >= 0)
            {
                mRow = mNextRow;
                if (mNextRow >= 0)
                {
                    mRow = mNextRow;
                }
                mNextRow = -1;
            }

            updateNote(audible);

            if (mNextRow == -1)
            {
                mNextRow = mRow + 1;
                if (mNextRow >= 64)
                {
                    mNextOrder = mOrder + 1;
                    if (mNextOrder >= mNumOrders)
                    {
                        mNextOrder = mRestart;
                    }
                    mNextRow = 0;
                }
            }
        }
    }
    else if (audible)
    {
        updateEffects();
    }

    mTick++;
    if (mTick >= mSpeed + mPatternDelay)
    {
        mPatternDelay = 0;
        mTick = 0;
    }

    mPCMOffset += mMixerSamplesPerTick;

    return FMOD_OK;
}

FMOD_RESULT CodecS3M::openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    // Guessed local names. The frame keeps every array (0x30..0xEE0) and scalar (0x8..0x2C) in this order.
    FMOD_RESULT result;
    char tag[4];
    MusicNote dummynote;
    unsigned char remap[32];
    char name[28];
    FMOD_CREATESOUNDEXINFO exinfo;
    unsigned short parapointer[356];
    short leftbuffer16[512];
    short rightbuffer16[512];
    unsigned int sampleoffset[99];
    unsigned char leftbuffer8[512];
    unsigned char rightbuffer8[512];
    unsigned int length;
    void * ptr1, * ptr2;
    unsigned int len1, len2, lenbytes;
    unsigned short wordval;
    unsigned short numorders;
    unsigned short numpatterns;
    unsigned char defaultpan;
    unsigned char mastervolume;
    unsigned char channelsetting;
    unsigned char pan;
    unsigned char sampleflags;
    unsigned char memseghi;
    unsigned char flag;
    int count;

    if (!mFile->mSeekable)
    {
        return FMOD_ERR_FORMAT;
    }

    init(FMOD_SOUND_TYPE_S3M);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getSize(&mWaveFormat.lengthbytes);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(0x2C, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(tag, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(tag, "SCRM", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    for (count = 0; count < 64; count++)
    {
        mMusicChannel[count] = 0;
    }

    mPattern = 0;
    mPanSeparation = 0.8f;
    mMasterSpeed = 1.0f;
    mDefaultSpeed = 6;
    mDefaultBPM = 125;
    mDefaultGlobalVolume = 64;
    mNumPatterns = 0;
    mRestart = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(mSongName, 1, 28, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte((unsigned char *)0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(0x20, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&numorders);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mNumSamples);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&numpatterns);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&wordval);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (wordval & 0x40)
    {
        mMusicFlags = 1;
    }
    else
    {
        mMusicFlags = 0;
    }

    result = mFile->getWord(&wordval);
    if (result != FMOD_OK)
    {
        return result;
    }

    if ((wordval & 0xFFF) == 0x12C)
    {
        mMusicFlags = 1;
    }

    result = mFile->seek(0x2C, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(tag, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(tag, "SCRM", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->getByte(&mGlobalVolume);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte(&mDefaultSpeed);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte(&mDefaultBPM);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte(&mastervolume);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte((unsigned char *)0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte(&defaultpan);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(0x40, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mNumChannels = 0;
    memset(remap, 0xFF, 32);

    for (count = 0; count < 32; count++)
    {
        result = mFile->getByte(&channelsetting);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (channelsetting < 16)
        {
            remap[count] = mNumChannels;

            if (channelsetting <= 7)
            {
                mDefaultPan[mNumChannels] = 0;
            }
            else
            {
                mDefaultPan[mNumChannels] = 255;
            }

            mNumChannels++;
        }
    }

    result = metaData(FMOD_TAGTYPE_FMOD, "Number of channels", &mNumChannels, sizeof(mNumChannels), FMOD_TAGDATATYPE_INT, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumChannels; count++)
    {
        mMusicChannel[count] = FMOD_Object_Calloc(MusicChannelS3M);
        if (!mMusicChannel[count])
        {
            return FMOD_ERR_MEMORY;
        }
    }

    result = mFile->seek(0x60, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(mOrderList, 1, numorders, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mNumOrders = 0;
    mNumPatterns = 0;

    for (count = 0; count < numorders; count++)
    {
        mOrderList[mNumOrders] = mOrderList[count];

        if (mOrderList[count] < 0xFE)
        {
            mNumOrders++;

            if (mOrderList[count] > mNumPatterns)
            {
                mNumPatterns = mOrderList[count];
            }
        }
    }
    mNumPatterns++;

    result = mFile->read(parapointer, 2, mNumSamples + numpatterns, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (defaultpan == 0xFC)
    {
        for (count = 0; count < 32; count++)
        {
            result = mFile->getByte(&pan);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (pan & 0x10)
            {
                mDefaultPan[remap[count]] = (pan & 0xF) << 4;
            }
        }
    }

    if (!(mastervolume & 0x80))
    {
        for (count = 0; count < 32; count++)
        {
            mDefaultPan[count] = 128;
        }
    }

    for (count = 0; count < mNumSamples; count++)
    {
        unsigned int mode;
        FMOD_SOUND_FORMAT format;
        int channels;

        channels = 1;

        memset(&mSample[count], 0, sizeof(MusicSample));

        result = mFile->seek(parapointer[count] << 4, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->seek(13, 1);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getByte(&memseghi);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getWord(&wordval);
        if (result != FMOD_OK)
        {
            return result;
        }

        sampleoffset[count] = (memseghi << 16) + wordval;

        result = mFile->read(&length, 4, 1, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(&mSample[count].mLoopStart, 4, 1, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(&mSample[count].mLoopLength, 4, 1, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        mSample[count].mLoopLength -= mSample[count].mLoopStart;

        result = mFile->getByte(&mSample[count].mDefaultVolume);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getWord((unsigned short *)0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getByte(&sampleflags);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getWord(&mSample[count].mMiddleC);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->seek(14, 1);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(name, 28, 1, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->seek(4, 1);
        if (result != FMOD_OK)
        {
            return result;
        }

        mode = FMOD_SOFTWARE | FMOD_2D;

        if ((sampleflags & 1) && mSample[count].mLoopLength > 2)
        {
            mode |= FMOD_LOOP_NORMAL;
        }
        else
        {
            mode |= FMOD_LOOP_OFF;
            mSample[count].mLoopStart = 0;
            mSample[count].mLoopLength = length;
        }

        format = FMOD_SOUND_FORMAT_PCM8;

        if (sampleflags & 4)
        {
            format = FMOD_SOUND_FORMAT_PCM16;
            length *= 2;
        }

        if (sampleflags & 2)
        {
            channels = 2;
            length *= 2;
        }

        if (length)
        {
            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.length = length;
            exinfo.numchannels = channels;
            exinfo.defaultfrequency = mSample[count].mMiddleC;
            exinfo.format = format;

            result = mSystem->createSound(0, mode | FMOD_OPENUSER, &exinfo, &mSample[count].mSound);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (mode & FMOD_LOOP_NORMAL)
            {
                result = mSample[count].mSound->setLoopPoints(mSample[count].mLoopStart, FMOD_TIMEUNIT_PCM, mSample[count].mLoopStart + mSample[count].mLoopLength - 1, FMOD_TIMEUNIT_PCM);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
    }

    mNumPatternsMem = (mNumPatterns > numpatterns) ? mNumPatterns : numpatterns;
    mPattern = (MusicPattern *)FMOD_Memory_Calloc(mNumPatternsMem * sizeof(MusicPattern));
    if (!mPattern)
    {
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < numpatterns; count++)
    {
        MusicPattern * pptr;
        MusicNote * nptr;
        int row;

        result = mFile->seek((parapointer[mNumSamples + count] << 4) + 2, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        pptr = &mPattern[count];
        pptr->mRows = 64;
        pptr->mData = (MusicNote *)FMOD_Memory_Calloc(mNumChannels * pptr->mRows * sizeof(MusicNote));
        if (!pptr->mData)
        {
            return FMOD_ERR_MEMORY;
        }

        row = 0;
        while (row < 64)
        {
            result = mFile->getByte(&flag);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (flag)
            {
                unsigned char channel = remap[flag & 31];

                if (channel < mNumChannels)
                {
                    nptr = pptr->mData + (mNumChannels * row) + channel;
                }
                else
                {
                    nptr = &dummynote;
                }

                if (flag & 0x20)
                {
                    result = mFile->getByte(&wordval);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    switch (wordval)
                    {
                        case 255:
                        {
                            nptr->mNote = 0;
                            break;
                        }
                        case 254:
                        {
                            nptr->mNote = 255;
                            break;
                        }
                        default:
                        {
                            nptr->mNote = ((wordval >> 4) * 12) + (wordval & 0xF) + 1;
                            break;
                        }
                    }

                    result = mFile->getByte(&nptr->mNumber);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }

                if (flag & 0x40)
                {
                    result = mFile->getByte(&nptr->mVolume);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                    nptr->mVolume++;
                }

                if (flag & 0x80)
                {
                    result = mFile->getByte(&nptr->mEffect);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    result = mFile->getByte(&nptr->mEffectParam);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }
            else
            {
                row++;
            }
        }
    }

    // Patterns the order list references beyond the stored ones get empty rows.
    if (mNumPatterns > numpatterns)
    {
        for (count = numpatterns; count < mNumPatterns; count++)
        {
            MusicPattern * pptr = &mPattern[count];

            pptr->mRows = 64;
            pptr->mData = (MusicNote *)FMOD_Memory_Calloc(mNumChannels * pptr->mRows * sizeof(MusicNote));
            if (!pptr->mData)
            {
                return FMOD_ERR_MEMORY;
            }
        }
    }

    for (count = 0; count < mNumSamples; count++)
    {
        result = mFile->seek(sampleoffset[count] << 4, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mSample[count].mSound)
        {
            result = mSample[count].mSound->getLength(&lenbytes, FMOD_TIMEUNIT_PCMBYTES);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mSample[count].mSound->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (ptr1 && len1)
            {
                unsigned int count2, offset, remaining, block;

                if (mSample[count].mSound->mFormat == FMOD_SOUND_FORMAT_PCM16)
                {
                    if (mSample[count].mSound->mChannels == 1)
                    {
                        result = mFile->read(ptr1, 2, len1 >> 1, 0);
                    }
                    else
                    {
                        // Stereo samples are stored as a left block then a right block: interleave them.
                        offset = 0;
                        remaining = len1 >> 2;
                        while (remaining)
                        {
                            block = remaining > 512 ? 512 : remaining;

                            result = mFile->read(leftbuffer16, 2, block, 0);

                            for (count2 = 0; count2 < block; count2++)
                            {
                                ((short *)ptr1)[(offset + count2) * 2] = leftbuffer16[count2];
                            }

                            remaining -= block;
                            offset += block;
                        }

                        offset = 0;
                        remaining = len1 >> 2;
                        while (remaining)
                        {
                            block = remaining > 512 ? 512 : remaining;

                            result = mFile->read(rightbuffer16, 2, block, 0);

                            for (count2 = 0; count2 < block; count2++)
                            {
                                ((short *)ptr1)[(offset + count2) * 2 + 1] = rightbuffer16[count2];
                            }

                            remaining -= block;
                            offset += block;
                        }
                    }

                    for (count2 = 0; count2 < len1 >> 1; count2++)
                    {
                        ((unsigned short *)ptr1)[count2] ^= 0x8000;
                    }
                }
                else
                {
                    if (mSample[count].mSound->mChannels == 1)
                    {
                        result = mFile->read(ptr1, 1, len1, 0);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                    else
                    {
                        offset = 0;
                        remaining = len1 >> 2;
                        while (remaining)
                        {
                            block = remaining > 512 ? 512 : remaining;

                            result = mFile->read(leftbuffer8, 1, block, 0);

                            for (count2 = 0; count2 < block; count2++)
                            {
                                ((unsigned char *)ptr1)[(offset + count2) * 2] = leftbuffer8[count2];
                            }

                            remaining -= block;
                            offset += block;
                        }

                        offset = 0;
                        remaining = len1 >> 2;
                        while (remaining)
                        {
                            block = remaining > 512 ? 512 : remaining;

                            result = mFile->read(rightbuffer8, 1, block, 0);

                            for (count2 = 0; count2 < block; count2++)
                            {
                                ((unsigned char *)ptr1)[(offset + count2) * 2 + 1] = rightbuffer8[count2];
                            }

                            remaining -= block;
                            offset += block;
                        }
                    }

                    for (count2 = 0; count2 < len1; count2++)
                    {
                        ((unsigned char *)ptr1)[count2] ^= 0x80;
                    }
                }

                if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
                {
                    return result;
                }
            }

            result = mSample[count].mSound->unlock(ptr1, ptr2, len1, len2);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (userexinfo && userexinfo->format)
    {
        mWaveFormat.format = userexinfo->format;
    }
    else if (usermode & FMOD_SOFTWARE)
    {
        mWaveFormat.format = FMOD_SOUND_FORMAT_PCMFLOAT;
    }
    else
    {
        mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
    }

    mWaveFormat.channels = 2;
    FMOD_strncpy(mWaveFormat.name, mSongName, 256);

    result = mSystem->getSoftwareFormat(&mWaveFormat.frequency, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSrcDataOffset = 0;

    SoundI::getBytesFromSamples(1, (unsigned int *)&mWaveFormat.blockalign, mWaveFormat.channels, mWaveFormat.format);

    {
        FMOD_DSP_DESCRIPTION_EX description;

        memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

        FMOD_strcpy(description.name, "FMOD S3M Target Unit");
        description.version = 0x00010100;
        description.channels = mWaveFormat.channels;
        description.mFormat = mWaveFormat.format;
        description.mCategory = FMOD_DSP_CATEGORY_SOUNDCARD;

        result = mSystem->createDSP(&description, &mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }

        mDSPHead->mDefaultFrequency = (float)mWaveFormat.frequency;
    }

    mNumVirtualChannels = mNumChannels;
    mVirtualChannel = (MusicVirtualChannel *)FMOD_Memory_Calloc(mNumVirtualChannels * sizeof(MusicVirtualChannel));
    if (!mVirtualChannel)
    {
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < mNumVirtualChannels; count++)
    {
        new (&mVirtualChannel[count]) MusicVirtualChannel;
    }

    {
        int numrealchannels = mNumVirtualChannels * 2; // Guessed name

        mChannelPool = FMOD_Object_Calloc(ChannelPool);
        if (!mChannelPool)
        {
            return FMOD_ERR_MEMORY;
        }

        result = mChannelPool->init(mSystem, 0, numrealchannels);
        if (result != FMOD_OK)
        {
            return result;
        }

        mChannelSoftware = (ChannelSoftware *)FMOD_Memory_Calloc(numrealchannels * sizeof(ChannelSoftware));
        if (!mChannelSoftware)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < numrealchannels; count++)
        {
            new (&mChannelSoftware[count]) ChannelSoftware;
            mChannelPool->setChannel(count, &mChannelSoftware[count], mDSPHead);
        }
    }

    if (usermode & FMOD_ACCURATETIME || usermode & FMOD_CREATESAMPLE)
    {
        mVisited = (bool *)FMOD_Memory_Calloc(mNumOrders * 256 * sizeof(bool));
        if (!mVisited)
        {
            return FMOD_ERR_MEMORY;
        }

        calculateLength();
    }
    else
    {
        mVisited = 0;
        mWaveFormat.lengthpcm = (unsigned int)-1;
    }

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    play();

    return result;
}

FMOD_RESULT CodecS3M::closeInternal()
{
    int count;

    stop();

    for (count = 0; count < mNumSamples; count++)
    {
        if (mSample[count].mSound)
        {
            mSample[count].mSound->release();
            mSample[count].mSound = 0;
        }
    }

    if (mVirtualChannel)
    {
        FMOD_Memory_Free(mVirtualChannel);
        mVirtualChannel = 0;
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

    if (mPattern)
    {
        for (count = 0; count < mNumPatterns; count++)
        {
            if (mPattern[count].mData)
            {
                FMOD_Memory_Free(mPattern[count].mData);
                mPattern[count].mData = 0;
            }
        }

        FMOD_Memory_Free(mPattern);
        mPattern = 0;
    }

    for (count = 0; count < mNumChannels; count++)
    {
        if (mMusicChannel[count])
        {
            FMOD_Memory_Free(mMusicChannel[count]);
            mMusicChannel[count] = 0;
        }
    }

    if (mVisited)
    {
        FMOD_Memory_Free(mVisited);
        mVisited = 0;
    }

    if (mDSPHead)
    {
        mDSPHead->release(true);
        mDSPHead = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecS3M::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int numsamples;
    int numchannels;
    LocalCriticalSection criticalsection(mSystem->mDSPCrit);

    numchannels = mWaveFormat.channels;

    SoundI::getSamplesFromBytes(sizebytes, &numsamples, numchannels, mWaveFormat.format);

    if (mPlaying && mMasterSpeed)
    {
        unsigned int mixedsofar = 0;
        unsigned int mixedleft = mMixerSamplesLeft;
        unsigned int samplestomix;
        char * destptr = (char *)buffer;

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
                result = mDSPHead->execute(buff, &buff, &read, numchannels, &numchannels, FMOD_SPEAKERMODE_STEREO_LINEAR);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mDSPHead->resetVisited();
            }
            criticalsection.leave();

            SoundI::getBytesFromSamples(read, &bytes, numchannels, mWaveFormat.format);

            if (buff != destptr && buffer)
            {
                memcpy(destptr, buff, bytes);
            }

            mixedsofar += read;
            destptr += bytes;
            mixedleft -= read;
        }

        mMixerSamplesLeft = mixedleft;
    }

    if (bytesread)
    {
        *bytesread = sizebytes;
    }

    return result;
}

FMOD_RESULT CodecS3M::setPositionInternal(int subsound, unsigned int position, unsigned int postype)
{
    if (postype == FMOD_TIMEUNIT_MODORDER)
    {
        play();

        mOrder = position;
        mNextOrder = position;

        return FMOD_OK;
    }
    else if (postype == FMOD_TIMEUNIT_PCM)
    {
        bool restarted = false;

        if (position == mPCMOffset)
        {
            return FMOD_OK;
        }

        if (position < mPCMOffset)
        {
            play();
            restarted = true;
        }

        while (mPCMOffset < position)
        {
            update(true);
        }

        if (restarted)
        {
            bool oldplaying = mPlaying;
            bool oldfinished = mFinished;

            stop();

            mPlaying = oldplaying;
            mFinished = oldfinished;
        }

        return FMOD_OK;
    }

    return FMOD_ERR_FORMAT;
}

FMOD_RESULT CodecS3M::openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecS3M *s3m = (CodecS3M *)codec_state;

    return s3m->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecS3M::closeCallback(FMOD_CODEC_STATE * codec_state)
{
    CodecS3M *s3m = (CodecS3M *)codec_state;

    return s3m->closeInternal();
}

FMOD_RESULT CodecS3M::readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecS3M *s3m = (CodecS3M *)codec_state;

    return s3m->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecS3M::setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype)
{
    CodecS3M *s3m = (CodecS3M *)codec_state;

    return s3m->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
