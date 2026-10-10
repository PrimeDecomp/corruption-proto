// G2MEAB prototype translation unit; complete reconstruction (all native functions have bodies).
// G2MEAB .text: 0x805D4E84..0x805D8160 (17 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: MOD descriptor80740CCC registration4E84 binds803C/8068/8094/80C0 to6518/7908/7B18/7F6C,
// formatC/stateFEC. Open6518 and close7908 directly name fmod_codec_mod.cpp. Duration4F3C,
// portamento/vibrato/tremolo helpers4FA8/5030/516C, row529C, tick5B94 and advance63A4 form the
// complete effect engine used by read/seek. Final80EC registers the same descriptor and ends8160,
// MPEG registration. Preserve every retained callback, emitted helper and initializer; complete
// inventory and inlining uncertainty are recorded externally.

// Reconstructed with the XM codec as template (no reference debug information covers MOD). The class and
// helper names are descriptive guesses; see fmod_codec_mod.h.

#include "fmod_codec_mod.h"
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

FMOD_CODEC_DESCRIPTION_EX modcodec; // Guessed name

FMOD_CODEC_DESCRIPTION_EX * CodecMOD::getDescriptionEx()
{
    memset(&modcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    modcodec.name = "FMOD MOD Codec";
    modcodec.version = 0x00010100;
    modcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_MODORDER | FMOD_TIMEUNIT_MODROW | FMOD_TIMEUNIT_MODPATTERN;
    modcodec.defaultasstream = 1;
    modcodec.open = &CodecMOD::openCallback;
    modcodec.close = &CodecMOD::closeCallback;
    modcodec.read = &CodecMOD::readCallback;
    modcodec.getlength = &MusicSong::getLengthCallback;
    modcodec.setposition = &CodecMOD::setPositionCallback;
    modcodec.getposition = &MusicSong::getPositionCallback;

    modcodec.mType = FMOD_SOUND_TYPE_MOD;
    modcodec.mSize = sizeof(CodecMOD);

    return &modcodec;
}

FMOD_RESULT CodecMOD::calculateLength()
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

FMOD_RESULT MusicChannelMOD::portamento()
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

FMOD_RESULT MusicChannelMOD::vibrato()
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

FMOD_RESULT MusicChannelMOD::tremolo()
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
            delta = gSineTable[temp];
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
        if ((short)(vcptr->mVolume - delta) < 0)
        {
            delta = vcptr->mVolume;
        }
        vcptr->mVolumeDelta = delta;
    }

    mTremoloPosition += mTremoloSpeed;
    if (mTremoloPosition > 31)
    {
        mTremoloPosition -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT CodecMOD::updateNote(bool audible)
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
        MusicChannelMOD * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicSample * sptr;
        unsigned char paramx, paramy;
        int oldvolume, oldfreq;

        paramx = current->mEffectParam >> 4;
        paramy = current->mEffectParam & 0xF;

        cptr = (MusicChannelMOD *)mMusicChannel[count];

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

        if (cptr->mRecentEffect == FMUSIC_MOD_TREMOLO && current->mEffect != FMUSIC_MOD_TREMOLO)
        {
            vcptr->mVolume = oldvolume + vcptr->mVolumeDelta;
        }
        cptr->mRecentEffect = current->mEffect;

        vcptr->mVolumeDelta = 0;
        vcptr->mNoteControl = 0;

        if (current->mNote)
        {
            vcptr->mNoteControl |= FMUSIC_STOP;

            if (vcptr == &gDummyVirtualChannel)
            {
                if (spawnNewVirtualChannel(cptr, sptr, &vcptr) != FMOD_OK)
                {
                    vcptr = &gDummyVirtualChannel;
                    vcptr->mSample = &gDummySample;
                }
            }

            cptr->mNote = current->mNote;
            cptr->mPeriod = gPeriodTable[cptr->mNote - 1] * 8363 / sptr->mMiddleC;

            vcptr->mPan = mDefaultPan[count];

            if ((cptr->mWaveControl & 0xF) < 4)
            {
                cptr->mVibPos = 0;
            }
            if ((cptr->mWaveControl >> 4) < 4)
            {
                cptr->mTremoloPosition = 0;
            }

            if (current->mEffect != FMUSIC_MOD_PORTATO && current->mEffect != FMUSIC_MOD_PORTATOVOLSLIDE)
            {
                vcptr->mFrequency = cptr->mPeriod;
            }

            vcptr->mNoteControl = FMUSIC_TRIGGER;
        }

        if (current->mNumber)
        {
            vcptr->mVolume = sptr->mDefaultVolume;
        }

        vcptr->mFrequencyDelta = 0;
        vcptr->mNoteControl |= (FMUSIC_FREQ | FMUSIC_VOLUME | FMUSIC_PAN);

        switch (current->mEffect)
        {
            case FMUSIC_MOD_SETVOLUME:
            {
                vcptr->mVolume = current->mEffectParam;
                break;
            }
            case FMUSIC_MOD_PORTATO:
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
            case FMUSIC_MOD_PORTATOVOLSLIDE:
            {
                cptr->mPortaTarget = cptr->mPeriod;
                vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                vcptr->mNoteControl &= ~FMUSIC_FREQ;
                break;
            }
            case FMUSIC_MOD_VIBRATO:
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
            case FMUSIC_MOD_TREMOLO:
            {
                if (paramx)
                {
                    cptr->mTremoloSpeed = paramx;
                }
                if (paramy)
                {
                    cptr->mTremoloDepth = paramy;
                }
                vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_MOD_SETPANPOSITION:
            {
                vcptr->mPan = current->mEffectParam << 1;
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case FMUSIC_MOD_SETSAMPLEOFFSET:
            {
                unsigned int offset;

                if (current->mEffectParam)
                {
                    cptr->mSampleOffset = current->mEffectParam;
                }

                offset = cptr->mSampleOffset << 8;

                if (offset >= sptr->mLoopStart + sptr->mLoopLength)
                {
                    vcptr->mSampleOffset = sptr->mLoopStart + sptr->mLoopLength - 1;
                }
                else
                {
                    vcptr->mSampleOffset = offset;
                }
                break;
            }
            case FMUSIC_MOD_PATTERNJUMP:
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
            case FMUSIC_MOD_PATTERNBREAK:
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
            case FMUSIC_MOD_SETSPEED:
            {
                if (current->mEffectParam < 0x20)
                {
                    if (current->mEffectParam)
                    {
                        mSpeed = current->mEffectParam;
                    }
                }
                else
                {
                    setBPM(current->mEffectParam);
                }
                break;
            }
            case FMUSIC_MOD_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_MOD_FINEPORTAUP:
                    {
                        vcptr->mFrequency -= (paramy << 2);
                        break;
                    }
                    case FMUSIC_MOD_FINEPORTADOWN:
                    {
                        vcptr->mFrequency += (paramy << 2);
                        break;
                    }
                    case FMUSIC_MOD_SETVIBRATOWAVE:
                    {
                        cptr->mWaveControl &= 0xF0;
                        cptr->mWaveControl |= paramy;
                        break;
                    }
                    case FMUSIC_MOD_SETFINETUNE:
                    {
                        fineTune2Hz(paramy, &sptr->mMiddleC);
                        break;
                    }
                    case FMUSIC_MOD_PATTERNLOOP:
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
                    case FMUSIC_MOD_SETTREMOLOWAVE:
                    {
                        cptr->mWaveControl &= 0xF;
                        cptr->mWaveControl |= (paramy << 4);
                        break;
                    }
                    case FMUSIC_MOD_SETPANPOSITION16:
                    {
                        vcptr->mPan = paramy << 4;
                        vcptr->mNoteControl |= FMUSIC_PAN;
                        break;
                    }
                    case FMUSIC_MOD_FINEVOLUMESLIDEUP:
                    {
                        vcptr->mVolume += paramy;
                        if (vcptr->mVolume > 64)
                        {
                            vcptr->mVolume = 64;
                        }
                        break;
                    }
                    case FMUSIC_MOD_FINEVOLUMESLIDEDOWN:
                    {
                        vcptr->mVolume -= paramy;
                        if (vcptr->mVolume < 0)
                        {
                            vcptr->mVolume = 0;
                        }
                        break;
                    }
                    case FMUSIC_MOD_NOTEDELAY:
                    {
                        vcptr->mVolume = oldvolume;
                        vcptr->mFrequency = oldfreq;
                        vcptr->mNoteControl &= ~FMUSIC_FREQ;
                        vcptr->mNoteControl &= !FMUSIC_PAN; // stores 0 (0x805D597C): logical not in place of ~
                        vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                        vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                        break;
                    }
                    case FMUSIC_MOD_PATTERNDELAY:
                    {
                        mPatternDelay = paramy;
                        mPatternDelay *= mSpeed;
                        break;
                    }
                }
                break;
            }
            case FMUSIC_MOD_ARPEGGIO: // no-op case: the jump table 0x806E41A8 starts at 0
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
                vcptr->mChannel.setVolume(0.5f * ((float)(vcptr->mVolume + vcptr->mVolumeDelta) / 64.0f));
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

FMOD_RESULT CodecMOD::updateEffects()
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
        MusicChannelMOD * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicSample * sptr;
        unsigned char effect, paramx, paramy;

        cptr = (MusicChannelMOD *)mMusicChannel[count];

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
            case FMUSIC_MOD_ARPEGGIO:
            {
                if (current->mEffectParam)
                {
                    switch (mTick % 3)
                    {
                        case 1:
                        {
                            vcptr->mFrequencyDelta = (gPeriodTable[cptr->mNote + paramx - 1] * 8363 / sptr->mMiddleC) - (gPeriodTable[cptr->mNote - 1] * 8363 / sptr->mMiddleC);
                            break;
                        }
                        case 2:
                        {
                            vcptr->mFrequencyDelta = (gPeriodTable[cptr->mNote + paramy - 1] * 8363 / sptr->mMiddleC) - (gPeriodTable[cptr->mNote - 1] * 8363 / sptr->mMiddleC);
                            break;
                        }
                    }
                    vcptr->mNoteControl |= FMUSIC_FREQ;
                }
                break;
            }
            case FMUSIC_MOD_PORTAUP:
            {
                vcptr->mFrequency -= (current->mEffectParam << 2);
                if (vcptr->mFrequency < 56)
                {
                    vcptr->mFrequency = 56;
                }
                vcptr->mNoteControl |= FMUSIC_FREQ;
                break;
            }
            case FMUSIC_MOD_PORTADOWN:
            {
                vcptr->mFrequency += (current->mEffectParam << 2);
                vcptr->mNoteControl |= FMUSIC_FREQ;
                break;
            }
            case FMUSIC_MOD_PORTATO:
            {
                cptr->portamento();
                break;
            }
            case FMUSIC_MOD_VIBRATO:
            {
                cptr->vibrato();
                break;
            }
            case FMUSIC_MOD_PORTATOVOLSLIDE:
            {
                cptr->portamento();

                if (paramx)
                {
                    vcptr->mVolume += paramx;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (paramy)
                {
                    vcptr->mVolume -= paramy;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_MOD_VIBRATOVOLSLIDE:
            {
                cptr->vibrato();

                if (paramx)
                {
                    vcptr->mVolume += paramx;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (paramy)
                {
                    vcptr->mVolume -= paramy;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_MOD_TREMOLO:
            {
                cptr->tremolo();
                break;
            }
            case FMUSIC_MOD_VOLUMESLIDE:
            {
                if (paramx)
                {
                    vcptr->mVolume += paramx;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (paramy)
                {
                    vcptr->mVolume -= paramy;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_MOD_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_MOD_NOTECUT:
                    {
                        if (mTick == paramy)
                        {
                            vcptr->mVolume = 0;
                            vcptr->mNoteControl |= FMUSIC_VOLUME;
                        }
                        break;
                    }
                    case FMUSIC_MOD_RETRIG:
                    {
                        if (paramy)
                        {
                            if (!(mTick % paramy))
                            {
                                vcptr->mNoteControl |= FMUSIC_TRIGGER;
                                vcptr->mNoteControl |= FMUSIC_VOLUME;
                                vcptr->mNoteControl |= FMUSIC_PAN;
                            }
                        }
                        break;
                    }
                    case FMUSIC_MOD_NOTEDELAY:
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
                                vcptr->mNoteControl |= FMUSIC_VOLUME;
                            }

                            vcptr->mPan = mDefaultPan[count];
                            vcptr->mFrequency = cptr->mPeriod;
                            vcptr->mNoteControl |= FMUSIC_FREQ;
                            vcptr->mNoteControl |= FMUSIC_PAN;
                            vcptr->mNoteControl |= FMUSIC_TRIGGER;
                        }
                        else
                        {
                            vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                            vcptr->mNoteControl &= ~FMUSIC_FREQ;
                            vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                        }
                        break;
                    }
                }
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
            vcptr->mChannel.setVolume(0.5f * ((float)(vcptr->mVolume + vcptr->mVolumeDelta) / 64.0f));
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

FMOD_RESULT CodecMOD::update(bool audible)
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

    if (mSpeed)
    {
        mTick++;
        if (mTick >= mSpeed + mPatternDelay)
        {
            mPatternDelay = 0;
            mTick = 0;
        }
    }
    else
    {
        mFinished = true;
        mTick = -1;
    }

    mPCMOffset += mMixerSamplesPerTick;

    return FMOD_OK;
}

FMOD_RESULT CodecMOD::openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    char tag[4]; // Guessed name
    int count;

    if (!mFile->mSeekable)
    {
        return FMOD_ERR_FORMAT;
    }

    init(FMOD_SOUND_TYPE_MOD);

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

    mFile->setBigEndian(true);

    result = mFile->seek(1080, 0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    result = mFile->read(tag, 1, 4, 0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    if (FMOD_strncmp(tag, "M.K.", 4) && FMOD_strncmp(tag, "M!K!", 4) && FMOD_strncmp(tag, "6CHN", 4) && FMOD_strncmp(tag, "8CHN", 4) && FMOD_strncmp(tag + 2, "CH", 2) && FMOD_strncmp(tag + 1, "CHN", 3))
    {
        mFile->setBigEndian(false);
        return FMOD_ERR_FORMAT;
    }

    if (!FMOD_strncmp(tag, "M.K.", 4))
    {
        mNumChannels = 4;
    }
    else if (!FMOD_strncmp(tag, "M!K!", 4))
    {
        mNumChannels = 4;
    }
    else if (!FMOD_strncmp(tag, "FLT4", 4))
    {
        mNumChannels = 4;
    }
    else if (!FMOD_strncmp(tag, "6CHN", 4))
    {
        mNumChannels = 6;
    }
    else if (!FMOD_strncmp(tag, "8CHN", 4))
    {
        mNumChannels = 8;
    }
    else if (!FMOD_strncmp(tag + 2, "CH", 2))
    {
        tag[3] = 0;
        mNumChannels = atoi(tag);
    }
    else if (!FMOD_strncmp(tag + 1, "CHN", 3))
    {
        mNumChannels = tag[0] - '0';
    }
    else
    {
        mNumChannels = 0;
    }

    if (mNumChannels < 1 || mNumChannels > 32)
    {
        mFile->setBigEndian(false);
        return FMOD_ERR_FORMAT;
    }

    result = metaData(FMOD_TAGTYPE_FMOD, "Number of channels", &mNumChannels, sizeof(mNumChannels), FMOD_TAGDATATYPE_INT, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    for (count = 0; count < 64; count++)
    {
        mMusicChannel[count] = 0;
    }

    mPattern = 0;
    mPanSeparation = 0.8f;
    mMasterSpeed = 1.0f;
    mLooping = true;
    mNumSamples = 31;
    mDefaultSpeed = 6;
    mDefaultBPM = 125;
    mNumPatterns = 0;
    mRestart = 0;

    result = mFile->read(mSongName, 1, 20, 0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    for (count = 0; count < mNumSamples; count++)
    {
        char name[22]; // Guessed name
        unsigned char volume; // Guessed name
        unsigned int length, loopstart, looplength; // Guessed names
        unsigned int mode; // Guessed name
        int count2;

        memset(&mSample[count], 0, sizeof(MusicSample));

        result = mFile->read(name, 1, 22, 0);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }

        for (count2 = 0; count2 < 28; count2++) // 28 (0x805D6A34): runs past the 22 bytes read
        {
            if (name[count2] < 32)
            {
                name[count2] = 0;
            }
        }

        mode = FMOD_SOFTWARE | FMOD_2D;

        result = mFile->getWord(&length);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }
        length *= 2;

        result = mFile->getByte(&mSample[count].mMiddleC);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }
        fineTune2Hz((unsigned char)mSample[count].mMiddleC, &mSample[count].mMiddleC);

        result = mFile->getByte(&volume);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }
        mSample[count].mDefaultVolume = volume;

        result = mFile->getWord(&loopstart);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }
        if (loopstart * 2 < length)
        {
            loopstart *= 2;
        }

        result = mFile->getWord(&looplength);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }
        looplength *= 2;

        if (loopstart + looplength > length)
        {
            looplength = length - loopstart;
        }

        if (looplength > 2)
        {
            mode |= FMOD_LOOP_NORMAL;
        }
        else
        {
            mode |= FMOD_LOOP_OFF;
            loopstart = 0;
            looplength = length;
        }

        if (length)
        {
            FMOD_CREATESOUNDEXINFO exinfo;

            memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
            exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
            exinfo.length = length;
            exinfo.numchannels = 1;
            exinfo.defaultfrequency = mSample[count].mMiddleC;
            exinfo.format = FMOD_SOUND_FORMAT_PCM8;

            result = mSystem->createSound(0, mode | FMOD_OPENUSER, &exinfo, &mSample[count].mSound);
            if (result != FMOD_OK)
            {
                mFile->setBigEndian(false);
                return result;
            }

            if (mode & FMOD_LOOP_NORMAL)
            {
                result = mSample[count].mSound->setLoopPoints(loopstart, FMOD_TIMEUNIT_PCM, loopstart + looplength - 1, FMOD_TIMEUNIT_PCM);
                if (result != FMOD_OK)
                {
                    mFile->setBigEndian(false);
                    return result;
                }
            }

            mSample[count].mLoopStart = loopstart;
            mSample[count].mLoopLength = looplength;
        }
    }

    result = mFile->getByte(&mNumOrders);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    result = mFile->getByte((unsigned char *)0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    memset(mOrderList, 0, 256);

    result = mFile->read(mOrderList, 1, 128, 0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    for (count = 0; count < 128; count++)
    {
        if (mOrderList[count] > mNumPatterns)
        {
            mNumPatterns = mOrderList[count];
        }
    }
    mNumPatterns++;

    result = mFile->getDword((unsigned int *)0);
    if (result != FMOD_OK)
    {
        mFile->setBigEndian(false);
        return result;
    }

    for (count = 0; count < mNumChannels; count++)
    {
        mMusicChannel[count] = FMOD_Object_Calloc(MusicChannelMOD);
        if (!mMusicChannel[count])
        {
            mFile->setBigEndian(false);
            return FMOD_ERR_MEMORY;
        }
    }

    for (count = 0; count < mNumChannels; count++)
    {
        mDefaultPan[count] = !((count + 1) & 2) ? 0 : 255;
    }

    mNumPatternsMem = mNumPatterns;
    mPattern = (MusicPattern *)FMOD_Memory_Calloc(mNumPatterns * sizeof(MusicPattern));
    if (!mPattern)
    {
        mFile->setBigEndian(false);
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < mNumPatterns; count++)
    {
        MusicPattern * pptr = &mPattern[count];
        MusicNote * nptr;
        int count2;

        pptr->mRows = 64;
        pptr->mData = (MusicNote *)FMOD_Memory_Calloc(mNumChannels * pptr->mRows * sizeof(MusicNote));
        if (!pptr->mData)
        {
            mFile->setBigEndian(false);
            return FMOD_ERR_MEMORY;
        }

        nptr = pptr->mData;

        for (count2 = 0; count2 < mNumChannels * pptr->mRows; count2++)
        {
            unsigned short period; // Guessed name
            int count3;

            result = mFile->read(tag, 1, 4, 0); // the pattern bytes reuse the tag buffer, read unsigned (0x805D7038)
            if (result != FMOD_OK)
            {
                mFile->setBigEndian(false);
                return result;
            }

            nptr->mNumber = (((unsigned char *)tag)[0] & 0xF0) + (((unsigned char *)tag)[2] >> 4);
            period = ((((unsigned char *)tag)[0] & 0xF) << 8) + ((unsigned char *)tag)[1];

            nptr->mNote = 0;
            for (count3 = 0; count3 < 108; count3++)
            {
                if (period >= gPeriodTable[count3 + 24])
                {
                    nptr->mNote = count3 + 1;
                    break;
                }
            }

            nptr->mVolume = 0;
            nptr->mEffect = ((unsigned char *)tag)[2] & 0xF;
            nptr->mEffectParam = ((unsigned char *)tag)[3];
            nptr++;
        }
    }

    {
        unsigned int filepos, filesize, totalsamplebytes; // Guessed names

        totalsamplebytes = 0;

        result = mFile->tell(&filepos);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }

        result = mFile->getSize(&filesize);
        if (result != FMOD_OK)
        {
            mFile->setBigEndian(false);
            return result;
        }

        for (count = 0; count < mNumSamples; count++)
        {
            if (mSample[count].mSound)
            {
                totalsamplebytes += mSample[count].mSound->mLength;
            }
        }

        // Some files carry junk between the patterns and the sample data: read the samples from the end.
        if (filesize - totalsamplebytes > 1080 && filesize < filepos + totalsamplebytes)
        {
            result = mFile->seek(filesize - totalsamplebytes, 0);
            if (result != FMOD_OK)
            {
                mFile->setBigEndian(false);
                return result;
            }
        }
    }

    for (count = 0; count < mNumSamples; count++)
    {
        MusicSample * sptr = &mSample[count];

        if (sptr->mSound)
        {
            void * ptr1, * ptr2;
            unsigned int len1, len2, lenbytes;

            SoundI::getBytesFromSamples(sptr->mSound->mLength, &lenbytes, sptr->mSound->mChannels, sptr->mSound->mFormat);

            result = sptr->mSound->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);
            if (result != FMOD_OK)
            {
                mFile->setBigEndian(false);
                return result;
            }

            if (ptr1 && len1)
            {
                result = mFile->read(ptr1, 1, len1, 0);
                if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
                {
                    mFile->setBigEndian(false);
                    return result;
                }
            }

            result = sptr->mSound->unlock(ptr1, ptr2, len1, len2);
            if (result != FMOD_OK)
            {
                mFile->setBigEndian(false);
                return result;
            }
        }
    }

    mFile->setBigEndian(false);

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

        FMOD_strcpy(description.name, "FMOD MOD Target Unit");
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

FMOD_RESULT CodecMOD::closeInternal()
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

FMOD_RESULT CodecMOD::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
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

FMOD_RESULT CodecMOD::setPositionInternal(int subsound, unsigned int position, unsigned int postype)
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

FMOD_RESULT CodecMOD::openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecMOD *mod = (CodecMOD *)codec_state;

    return mod->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecMOD::closeCallback(FMOD_CODEC_STATE * codec_state)
{
    CodecMOD *mod = (CodecMOD *)codec_state;

    return mod->closeInternal();
}

FMOD_RESULT CodecMOD::readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecMOD *mod = (CodecMOD *)codec_state;

    return mod->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecMOD::setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype)
{
    CodecMOD *mod = (CodecMOD *)codec_state;

    return mod->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
