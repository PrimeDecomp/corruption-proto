// G2MEAB prototype translation unit; complete reconstruction (all native functions have bodies).
// G2MEAB .text: 0x805EB6EC..0x805F0414 (24 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: XM descriptor807539FC registrationB6EC, format15/stateA20, bindsF02F0/031C/0348/0374 to
// openEE040/closeEFB3C/readEFDCC/seekF0220. Open/close directly name fmod_codec_xm.cpp. Complete
// preceding duration, portamento/vibrato/tremolo/envelope, row/tick and advance group remains with
// this state/callback family. InitializerF03A0 closes descriptor atF0414, which starts Chorus DSP
// registration, not another codec helper. Preserve every retained callback, emitted helper and
// initializer; complete inventory and inlining uncertainty are recorded externally.

// Reconstructed with a later FMOD Ex (Gormiti, Wii/MWCC) debug information as reference. G2MEAB is older:
// the length calculation, the virtual channel lookup, portamento and the Amiga period helper are separate
// functions here, and the envelope/note helpers take their state as parameters.

#include "fmod_codec_xm.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_channel_software.h"
#include "fmod_channelpool.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_music.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX xmcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecXM::getDescriptionEx()
{
    memset(&xmcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    xmcodec.name = "FMOD XM Codec";
    xmcodec.version = 0x00010100;
    xmcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_MODORDER | FMOD_TIMEUNIT_MODROW | FMOD_TIMEUNIT_MODPATTERN;
    xmcodec.defaultasstream = 1;
    xmcodec.open = &CodecXM::openCallback;
    xmcodec.close = &CodecXM::closeCallback;
    xmcodec.read = &CodecXM::readCallback;
    xmcodec.getlength = &MusicSong::getLengthCallback;
    xmcodec.setposition = &CodecXM::setPositionCallback;
    xmcodec.getposition = &MusicSong::getPositionCallback;

    xmcodec.mType = FMOD_SOUND_TYPE_XM;
    xmcodec.mSize = sizeof(CodecXM);

    return &xmcodec;
}

FMOD_RESULT CodecXM::calculateLength()
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

FMOD_RESULT CodecXM::getVirtualChannel(MusicChannel * cptr, MusicVirtualChannel * vcptr, MusicSample * sptr, MusicVirtualChannel * * newvcptr)
{
    if (vcptr == &gDummyVirtualChannel)
    {
        spawnNewVirtualChannel(cptr, sptr, newvcptr);
    }
    else
    {
        *newvcptr = vcptr;
    }

    if (!newvcptr)
    {
        *newvcptr = vcptr;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::updateFlags(MusicVirtualChannel * vcptr, MusicSample * sptr)
{
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
        float finalvol;

        finalvol = (float)vcptr->mEnvVolume.mValue;
        finalvol *= (float)(vcptr->mVolume + vcptr->mVolumeDelta);
        finalvol *= (float)vcptr->mFadeOutVolume;
        finalvol *= (float)mGlobalVolume;
        finalvol *= (1.0f / (64.0f * 64.0f * 65536.0f * 128.0f));

        vcptr->mChannel.setVolume(finalvol);
    }

    if (vcptr->mNoteControl & FMUSIC_PAN)
    {
        float finalpan;

        finalpan = ((float)vcptr->mPan - 128.0f) * mPanSeparation / 127.0f;

        vcptr->mChannel.setPan(finalpan, true);
    }

    if (vcptr->mNoteControl & FMUSIC_FREQ)
    {
        int finalfreq;

        finalfreq = vcptr->mFrequency + vcptr->mFrequencyDelta;
        if (finalfreq < 1)
        {
            finalfreq = 1;
        }

        if (mMusicFlags & 1)
        {
            finalfreq = (int)(8363.0f * (float)pow(2.0, (4608.0f - (float)finalfreq) / 768.0f));
        }
        else
        {
            finalfreq = 14317056 / finalfreq;
        }

        vcptr->mChannel.setFrequency((float)finalfreq);
    }

    if (vcptr->mNoteControl & FMUSIC_STOP)
    {
        vcptr->mChannel.stopEx(false, false, true, true, false);
        vcptr->mSampleOffset = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicChannelXM::portamento()
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
    else if (vcptr->mFrequency > mPortaTarget)
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

FMOD_RESULT MusicChannelXM::vibrato()
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
        vcptr->mFrequencyDelta = -delta;
    }
    else
    {
        vcptr->mFrequencyDelta = delta;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelXM::tremolo()
{
    unsigned char temp;
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    temp = (mTremoloPosition & 31);

    switch ((mWaveControl >> 4) & 3)
    {
        case 0:
        {
            vcptr->mVolumeDelta = gSineTable[temp];
            break;
        }
        case 1:
        {
            temp <<= 3;
            if (mTremoloPosition < 0)
            {
                temp = 255 - temp;
            }
            vcptr->mVolumeDelta = temp;
            break;
        }
        case 2:
        {
            vcptr->mVolumeDelta = 255;
            break;
        }
        case 3:
        {
            vcptr->mVolumeDelta = gSineTable[temp];
            break;
        }
    }

    vcptr->mVolumeDelta *= mTremoloDepth;
    vcptr->mVolumeDelta >>= 6;

    if (mTremoloPosition >= 0)
    {
        if (vcptr->mVolume + vcptr->mVolumeDelta > 64)
        {
            vcptr->mVolumeDelta = 64 - vcptr->mVolume;
        }
    }
    else
    {
        if ((short)(vcptr->mVolume - vcptr->mVolumeDelta) < 0)
        {
            vcptr->mVolumeDelta = vcptr->mVolume;
        }
        vcptr->mVolumeDelta = -vcptr->mVolumeDelta;
    }

    mTremoloPosition += mTremoloSpeed;
    if (mTremoloPosition > 31)
    {
        mTremoloPosition -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT CodecXM::processEnvelope(MusicEnvelopeState * env, MusicVirtualChannel * vcptr, int numpoints, unsigned short * points, int type, int loopstart, int loopend, unsigned char ISustain, unsigned char control)
{
    if (env->mPosition < numpoints)
    {
        if (!env->mTick || env->mTick == points[env->mPosition << 1])
        {
            int currpos, nextpos;
            int currtick, nexttick;
            int currval, nextval, tickdiff;

            do
            {
                if (type & 4 && env->mPosition == loopend)
                {
                    env->mPosition = loopstart;
                    env->mTick = points[env->mPosition << 1];
                }

                currpos = env->mPosition;
                nextpos = env->mPosition + 1;

                currtick = points[currpos << 1];
                nexttick = points[nextpos << 1];
                currval = points[(currpos << 1) + 1] << 16;
                nextval = points[(nextpos << 1) + 1] << 16;

                if (currpos == numpoints - 1)
                {
                    env->mValue = points[(currpos << 1) + 1];
                    env->mStopped = true;
                    vcptr->mNoteControl |= control;
                    return FMOD_OK;
                }

                if (type & 2 && currpos == ISustain && !vcptr->mKeyOff)
                {
                    env->mValue = points[(currpos << 1) + 1];
                    vcptr->mNoteControl |= control;
                    return FMOD_OK;
                }

                tickdiff = nexttick - currtick;
                if (tickdiff)
                {
                    env->mDelta = (nextval - currval) / tickdiff;
                }
                else
                {
                    env->mDelta = 0;
                }

                env->mFraction = currval;
                env->mPosition++;
            } while (env->mTick == points[env->mPosition << 1] && env->mPosition < numpoints);
        }
        else
        {
            env->mFraction += env->mDelta;
        }
    }

    env->mValue = env->mFraction >> 16;
    env->mTick++;

    vcptr->mNoteControl |= control;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelXM::instrumentVibrato(MusicInstrument * iptr)
{
    int delta;
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    switch (iptr->mVibratoType)
    {
        case 0:
        {
            delta = gFineSineTable[vcptr->mIVibPos];
            break;
        }
        case 1:
        {
            if (vcptr->mIVibPos < 128)
            {
                delta = 64;
            }
            else
            {
                delta = -64;
            }
            break;
        }
        case 2:
        {
            delta = (128 - ((vcptr->mIVibPos + 128) % 256)) >> 1;
            break;
        }
        case 3:
        {
            delta = (128 - ((384 - vcptr->mIVibPos) % 256)) >> 1;
            break;
        }
        default:
        {
            delta = 0;
            break;
        }
    }

    delta *= iptr->mVibratoDepth;
    if (iptr->mVibratoSweep)
    {
        delta = delta * vcptr->mIVibSweepPos / iptr->mVibratoSweep;
    }

    vcptr->mFrequencyDelta += delta >> 6;

    vcptr->mIVibSweepPos++;
    if (vcptr->mIVibSweepPos > iptr->mVibratoSweep)
    {
        vcptr->mIVibSweepPos = iptr->mVibratoSweep;
    }

    vcptr->mIVibPos += iptr->mVibratoRate;
    if (vcptr->mIVibPos > 255)
    {
        vcptr->mIVibPos -= 256;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelXM::processVolumeByte(unsigned char volume)
{
    MusicVirtualChannel *vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    if (volume >= 0x10 && volume <= 0x50)
    {
        vcptr->mVolume = volume - 0x10;
        vcptr->mNoteControl |= FMUSIC_VOLUME;
    }
    else
    {
        switch (volume >> 4)
        {
            case 0x6:
            {
                vcptr->mVolume -= (volume & 0xF);
                if (vcptr->mVolume < 0)
                {
                    vcptr->mVolume = 0;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0x7:
            {
                vcptr->mVolume += (volume & 0xF);
                if (vcptr->mVolume > 0x40)
                {
                    vcptr->mVolume = 0x40;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0x8:
            {
                vcptr->mVolume -= (volume & 0xF);
                if (vcptr->mVolume < 0)
                {
                    vcptr->mVolume = 0;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0x9:
            {
                vcptr->mVolume += (volume & 0xF);
                if (vcptr->mVolume > 0x40)
                {
                    vcptr->mVolume = 0x40;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0xA:
            {
                mVibSpeed = (volume & 0xF);
                break;
            }
            case 0xB:
            {
                mVibDepth = (volume & 0xF);
                break;
            }
            case 0xC:
            {
                vcptr->mPan = (volume & 0xF) << 4;
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case 0xD:
            {
                vcptr->mPan -= (volume & 0xF);
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case 0xE:
            {
                vcptr->mPan += (volume & 0xF);
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case 0xF:
            {
                if (volume & 0xF)
                {
                    mPortaSpeed = (volume & 0xF) << 4;
                }
                mPortaTarget = mPeriod;
                vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                break;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::getAmigaPeriod(int note, int finetune, int * period)
{
    *period = gPeriodTable[note];

    if (finetune < 0 && note)
    {
        *period = *period - ((*period - (int)gPeriodTable[note - 1]) * -finetune / 128);
    }
    else
    {
        *period = *period + (((int)gPeriodTable[note + 1] - *period) * finetune / 128);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::processNote(MusicNote * current, MusicChannelXM * cptr, MusicVirtualChannel * vcptr, MusicInstrument * iptr, MusicSample * sptr)
{
    if (current->mNumber)
    {
        vcptr->mVolume = sptr->mDefaultVolume;
        vcptr->mPan = sptr->mDefaultPan;

        vcptr->mEnvVolume.mValue = 64;
        vcptr->mEnvVolume.mPosition = 0;
        vcptr->mEnvVolume.mTick = 0;
        vcptr->mEnvVolume.mDelta = 0;
        vcptr->mEnvPan.mValue = 32;
        vcptr->mEnvPan.mPosition = 0;
        vcptr->mEnvPan.mTick = 0;
        vcptr->mEnvPan.mDelta = 0;
        vcptr->mFadeOutVolume = 65536;
        vcptr->mEnvVolume.mStopped = false;
        vcptr->mEnvPan.mStopped = false;
        vcptr->mKeyOff = false;
        vcptr->mIVibSweepPos = 0;
        vcptr->mIVibPos = 0;

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
        vcptr->mNoteControl |= FMUSIC_PAN;
    }

    if (current->mVolume)
    {
        cptr->processVolumeByte(current->mVolume);
    }

    if (current->mNote == 0xFF || current->mEffect == 0x14)
    {
        vcptr->mKeyOff = true;
    }

    if (iptr->mVolumeType & 1)
    {
        if (!vcptr->mEnvVolume.mStopped)
        {
            processEnvelope(&vcptr->mEnvVolume, vcptr, iptr->mVolumeNumPoints, iptr->mVolumePoints, iptr->mVolumeType, iptr->mVolumeLoopStart, iptr->mVolumeLoopEnd, iptr->mVolumeSustain, FMUSIC_VOLUME);
        }
    }
    else if (vcptr->mKeyOff)
    {
        vcptr->mEnvVolume.mValue = 0;
    }

    if (iptr->mPanType & 1 && !vcptr->mEnvPan.mStopped)
    {
        processEnvelope(&vcptr->mEnvPan, vcptr, iptr->mPanNumPoints, iptr->mPanPoints, iptr->mPanType, iptr->mPanLoopStart, iptr->mPanLoopEnd, iptr->mPanSustain, FMUSIC_PAN);
    }

    if (vcptr->mKeyOff)
    {
        vcptr->mFadeOutVolume -= iptr->mVolumeFade;
        if (vcptr->mFadeOutVolume < 0)
        {
            vcptr->mFadeOutVolume = 0;
        }
        vcptr->mNoteControl |= FMUSIC_VOLUME;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::updateNote()
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
        MusicChannelXM * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicSample * sptr;
        MusicInstrument * iptr;
        unsigned char paramx, paramy;
        int oldvolume, oldfreq, oldpan;
        bool porta;

        paramx = current->mEffectParam >> 4;
        paramy = current->mEffectParam & 0xF;

        cptr = (MusicChannelXM *)mMusicChannel[count];

        if (cptr->mVirtualChannelHead.isEmpty())
        {
            vcptr = &gDummyVirtualChannel;
            vcptr->mSample = &gDummySample;
        }
        else
        {
            vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();
        }

        porta = (current->mEffect == FMUSIC_XM_PORTATO || current->mEffect == FMUSIC_XM_PORTATOVOLSLIDE);

        if (current->mNumber && !porta)
        {
            cptr->mInstrument = current->mNumber - 1;
        }

        if (current->mNote && current->mNote != 0xFF && !porta)
        {
            cptr->mNote = current->mNote - 1;
        }

        if (cptr->mInstrument >= mNumInstruments)
        {
            iptr = &gDummyInstrument;
            sptr = &gDummySample;
            sptr->mSound = 0;
        }
        else
        {
            iptr = &mInstrument[cptr->mInstrument];
            if (iptr->mKeyMap[cptr->mNote] >= 16)
            {
                sptr = &gDummySample;
            }
            else
            {
                sptr = &iptr->mSample[iptr->mKeyMap[cptr->mNote]];
            }

            if (!porta)
            {
                vcptr->mSample = sptr;
            }
        }

        oldvolume = vcptr->mVolume;
        oldfreq = vcptr->mFrequency;
        oldpan = vcptr->mPan;

        if (cptr->mRecentEffect == FMUSIC_XM_TREMOLO && current->mEffect != FMUSIC_XM_TREMOLO)
        {
            vcptr->mVolume += vcptr->mVolumeDelta;
        }
        cptr->mRecentEffect = current->mEffect;

        vcptr->mVolumeDelta = 0;
        vcptr->mNoteControl = 0;

        if (current->mNote && current->mNote != 0xFF)
        {
            if (!porta || vcptr == &gDummyVirtualChannel)
            {
                getVirtualChannel(cptr, vcptr, sptr, &vcptr);
            }

            if (!vcptr)
            {
                vcptr = &gDummyVirtualChannel;
                vcptr->mSample = &gDummySample;
            }

            cptr->mRealNote = current->mNote + sptr->mRelative - 1;

            if (mMusicFlags & 1)
            {
                cptr->mPeriod = (10 * 12 * 16 * 4) - (cptr->mRealNote * 16 * 4) - (sptr->mFineTune / 2);
            }
            else
            {
                getAmigaPeriod(cptr->mRealNote, sptr->mFineTune, &cptr->mPeriod);
            }

            if (!porta)
            {
                vcptr->mFrequency = cptr->mPeriod;
            }

            vcptr->mNoteControl = FMUSIC_TRIGGER;
        }

        vcptr->mFrequencyDelta = 0;
        vcptr->mNoteControl |= FMUSIC_FREQ;
        vcptr->mNoteControl |= FMUSIC_VOLUME;

        processNote(current, cptr, vcptr, iptr, sptr);

        switch (current->mEffect)
        {
            case FMUSIC_XM_PORTAUP:
            {
                if (current->mEffectParam)
                {
                    cptr->mPortaUp = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_XM_PORTADOWN:
            {
                if (current->mEffectParam)
                {
                    cptr->mPortaDown = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_XM_PORTATO:
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
            case FMUSIC_XM_PORTATOVOLSLIDE:
            {
                cptr->mPortaTarget = cptr->mPeriod;
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }
                vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                vcptr->mNoteControl &= ~FMUSIC_FREQ;
                break;
            }
            case FMUSIC_XM_VIBRATO:
            {
                if (paramx)
                {
                    cptr->mVibSpeed = paramx;
                }
                if (paramy)
                {
                    cptr->mVibDepth = paramy;
                }
                cptr->vibrato();
                break;
            }
            case FMUSIC_XM_VIBRATOVOLSLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }
                cptr->vibrato();
                break;
            }
            case FMUSIC_XM_TREMOLO:
            {
                if (paramx)
                {
                    cptr->mTremoloSpeed = paramx;
                }
                if (paramy)
                {
                    cptr->mTremoloDepth = paramy;
                }
                break;
            }
            case FMUSIC_XM_SETPANPOSITION:
            {
                vcptr->mPan = current->mEffectParam;
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case FMUSIC_XM_SETSAMPLEOFFSET:
            {
                unsigned int offset;

                if (current->mEffectParam)
                {
                    cptr->mSampleOffset = current->mEffectParam;
                }

                offset = cptr->mSampleOffset << 8;

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
            case FMUSIC_XM_VOLUMESLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mVolumeSlide = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_XM_PATTERNJUMP:
            {
                mNextOrder = current->mEffectParam;
                mNextRow = 0;

                if (mNextOrder >= mNumOrders)
                {
                    mNextOrder = 0;
                    mFinished = true;
                }

                jumpflag = true;
                break;
            }
            case FMUSIC_XM_SETVOLUME:
            {
                vcptr->mVolume = current->mEffectParam;
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_XM_PATTERNBREAK:
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
            case FMUSIC_XM_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_XM_FINEPORTAUP:
                    {
                        if (paramy)
                        {
                            cptr->mFinePortaUp = paramy;
                        }
                        vcptr->mFrequency -= (cptr->mFinePortaUp << 2);
                        break;
                    }
                    case FMUSIC_XM_FINEPORTADOWN:
                    {
                        if (paramy)
                        {
                            cptr->mFinePortaDown = paramy;
                        }
                        vcptr->mFrequency += (cptr->mFinePortaDown << 2);
                        break;
                    }
                    case FMUSIC_XM_SETVIBRATOWAVE:
                    {
                        cptr->mWaveControl &= 0xF0;
                        cptr->mWaveControl |= paramy;
                        break;
                    }
                    case FMUSIC_XM_SETFINETUNE:
                    {
                        sptr->mFineTune = paramy;
                        break;
                    }
                    case FMUSIC_XM_PATTERNLOOP:
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
                    case FMUSIC_XM_SETTREMOLOWAVE:
                    {
                        cptr->mWaveControl &= 0xF;
                        cptr->mWaveControl |= (paramy << 4);
                        break;
                    }
                    case FMUSIC_XM_SETPANPOSITION16:
                    {
                        vcptr->mPan = paramy << 4;
                        vcptr->mNoteControl |= FMUSIC_PAN;
                        break;
                    }
                    case FMUSIC_XM_FINEVOLUMESLIDEUP:
                    {
                        if (paramy)
                        {
                            cptr->mFineVolumeSlideUp = paramy;
                        }

                        vcptr->mVolume += cptr->mFineVolumeSlideUp;
                        if (vcptr->mVolume > 64)
                        {
                            vcptr->mVolume = 64;
                        }

                        vcptr->mNoteControl |= FMUSIC_VOLUME;
                        break;
                    }
                    case FMUSIC_XM_FINEVOLUMESLIDEDOWN:
                    {
                        if (paramy)
                        {
                            cptr->mFineVolumeSlideUp = paramy;
                        }

                        vcptr->mVolume -= cptr->mFineVolumeSlideUp;
                        if (vcptr->mVolume < 0)
                        {
                            vcptr->mVolume = 0;
                        }

                        vcptr->mNoteControl |= FMUSIC_VOLUME;
                        break;
                    }
                    case FMUSIC_XM_NOTEDELAY:
                    {
                        vcptr->mVolume = oldvolume;
                        vcptr->mFrequency = oldfreq;
                        vcptr->mPan = oldpan;
                        vcptr->mNoteControl &= ~FMUSIC_FREQ;
                        vcptr->mNoteControl &= ~FMUSIC_VOLUME;
                        vcptr->mNoteControl &= ~FMUSIC_PAN;
                        vcptr->mNoteControl &= ~FMUSIC_TRIGGER;
                        break;
                    }
                    case FMUSIC_XM_PATTERNDELAY:
                    {
                        mPatternDelay = paramy;
                        mPatternDelay *= mSpeed;
                        break;
                    }
                    case FMUSIC_XM_FUNKREPEAT:
                    {
                        break;
                    }
                }
                break;
            }
            case FMUSIC_XM_SETSPEED:
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
            case FMUSIC_XM_SETGLOBALVOLUME:
            {
                mGlobalVolume = current->mEffectParam;
                if (mGlobalVolume > 64)
                {
                    mGlobalVolume = 64;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_XM_GLOBALVOLSLIDE:
            {
                if (current->mEffectParam)
                {
                    mGlobalVolumeSlide = current->mEffectParam;
                }
                break;
            }
            case FMUSIC_XM_SETENVELOPEPOS:
            {
                if (iptr->mVolumeType & 1)
                {
                    int currpos, nextpos;
                    int currtick, currvol, nextvol, tickdiff;

                    currpos = 0;
                    while (current->mEffectParam > iptr->mVolumePoints[(currpos + 1) << 1] && currpos < iptr->mVolumeNumPoints)
                    {
                        currpos++;
                    }

                    vcptr->mEnvVolume.mPosition = currpos;

                    if (vcptr->mEnvVolume.mPosition >= iptr->mVolumeNumPoints - 1)
                    {
                        vcptr->mEnvVolume.mValue = iptr->mVolumePoints[((iptr->mVolumeNumPoints - 1) << 1) + 1];
                        vcptr->mEnvVolume.mStopped = true;
                    }
                    else
                    {
                        vcptr->mEnvVolume.mStopped = false;
                        vcptr->mEnvVolume.mTick = current->mEffectParam;

                        nextpos = vcptr->mEnvVolume.mPosition + 1;

                        currtick = iptr->mVolumePoints[currpos << 1];
                        currvol = iptr->mVolumePoints[(currpos << 1) + 1] << 16;
                        nextvol = iptr->mVolumePoints[(nextpos << 1) + 1] << 16;
                        tickdiff = iptr->mVolumePoints[nextpos << 1] - currtick;

                        if (tickdiff)
                        {
                            vcptr->mEnvVolume.mDelta = (nextvol - currvol) / tickdiff;
                        }
                        else
                        {
                            vcptr->mEnvVolume.mDelta = 0;
                        }

                        tickdiff = vcptr->mEnvVolume.mTick - currtick;
                        vcptr->mEnvVolume.mFraction = currvol + (vcptr->mEnvVolume.mDelta * tickdiff);
                        vcptr->mEnvVolume.mValue = vcptr->mEnvVolume.mFraction >> 16;
                        vcptr->mEnvVolume.mPosition++;
                    }
                }
                break;
            }
            case FMUSIC_XM_PANSLIDE:
            {
                if (current->mEffectParam)
                {
                    cptr->mPanSlide = current->mEffectParam;
                    vcptr->mNoteControl |= FMUSIC_PAN;
                }
                break;
            }
            case FMUSIC_XM_MULTIRETRIG:
            {
                if (current->mEffectParam)
                {
                    cptr->mRetrigX = paramx;
                    cptr->mRetrigY = paramy;
                }
                break;
            }
            case FMUSIC_XM_TREMOR:
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
            case FMUSIC_XM_EXTRAFINEPORTA:
            {
                if (paramx == 1)
                {
                    if (paramy)
                    {
                        cptr->mXtraPortaUp = paramy;
                    }
                    vcptr->mFrequency -= cptr->mXtraPortaUp;
                }
                else if (paramx == 2)
                {
                    if (paramy)
                    {
                        cptr->mXtraPortaDown = paramy;
                    }
                    vcptr->mFrequency += cptr->mXtraPortaDown;
                }
                break;
            }
            case FMUSIC_XM_Z:
            {
                break;
            }
        }

        cptr->instrumentVibrato(iptr);

        updateFlags((MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext(), sptr);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::updateEffects()
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
        MusicChannelXM * cptr;
        MusicVirtualChannel * vcptr = 0;
        MusicInstrument * iptr;
        MusicSample * sptr;
        unsigned char effect, paramx, paramy;

        cptr = (MusicChannelXM *)mMusicChannel[count];

        if (cptr->mVirtualChannelHead.isEmpty())
        {
            vcptr = &gDummyVirtualChannel;
        }
        else
        {
            vcptr = (MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext();
        }

        if (cptr->mInstrument >= mNumInstruments)
        {
            iptr = &gDummyInstrument;
            sptr = &gDummySample;
            sptr->mSound = 0;
        }
        else
        {
            iptr = &mInstrument[cptr->mInstrument];
            if (iptr->mKeyMap[cptr->mNote] >= 16)
            {
                sptr = &gDummySample;
            }
            else
            {
                sptr = &iptr->mSample[iptr->mKeyMap[cptr->mNote]];
            }

            if (!sptr)
            {
                sptr = &gDummySample;
            }
        }

        effect = current->mEffect;
        paramx = current->mEffectParam >> 4;
        paramy = current->mEffectParam & 0xF;

        vcptr->mVolumeDelta = 0;
        vcptr->mFrequencyDelta = 0;
        vcptr->mNoteControl = 0;

        if (iptr->mVolumeType & 1 && !vcptr->mEnvVolume.mStopped)
        {
            processEnvelope(&vcptr->mEnvVolume, vcptr, iptr->mVolumeNumPoints, iptr->mVolumePoints, iptr->mVolumeType, iptr->mVolumeLoopStart, iptr->mVolumeLoopEnd, iptr->mVolumeSustain, FMUSIC_VOLUME);
        }

        if (iptr->mPanType & 1 && !vcptr->mEnvPan.mStopped)
        {
            processEnvelope(&vcptr->mEnvPan, vcptr, iptr->mPanNumPoints, iptr->mPanPoints, iptr->mPanType, iptr->mPanLoopStart, iptr->mPanLoopEnd, iptr->mPanSustain, FMUSIC_PAN);
        }

        if (vcptr->mKeyOff)
        {
            vcptr->mFadeOutVolume -= iptr->mVolumeFade;
            if (vcptr->mFadeOutVolume < 0)
            {
                vcptr->mFadeOutVolume = 0;
            }
            vcptr->mNoteControl |= FMUSIC_VOLUME;
        }

        switch (current->mVolume >> 4)
        {
            case 0x6:
            {
                vcptr->mVolume -= (current->mVolume & 0xF);
                if (vcptr->mVolume < 0)
                {
                    vcptr->mVolume = 0;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0x7:
            {
                vcptr->mVolume += (current->mVolume & 0xF);
                if (vcptr->mVolume > 64)
                {
                    vcptr->mVolume = 64;
                }
                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case 0xB:
            {
                cptr->mVibDepth = (current->mVolume & 0xF);

                cptr->vibrato();

                cptr->mVibPos += cptr->mVibSpeed;
                if (cptr->mVibPos > 31)
                {
                    cptr->mVibPos -= 64;
                }
                break;
            }
            case 0xD:
            {
                vcptr->mPan -= (current->mVolume & 0xF);
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case 0xE:
            {
                vcptr->mPan += (current->mVolume & 0xF);
                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case 0xF:
            {
                cptr->portamento();
                break;
            }
        }

        switch (effect)
        {
            case FMUSIC_XM_ARPEGGIO:
            {
                if (current->mEffectParam)
                {
                    switch (mTick % 3)
                    {
                        case 1:
                        {
                            if (mMusicFlags & 1)
                            {
                                vcptr->mFrequencyDelta = -paramx << 6;
                            }
                            else
                            {
                                int per1, per2;

                                getAmigaPeriod(cptr->mRealNote + paramx, sptr->mFineTune, &per1);
                                getAmigaPeriod(cptr->mRealNote, sptr->mFineTune, &per2);

                                vcptr->mFrequencyDelta = per1 - per2;
                            }
                            break;
                        }
                        case 2:
                        {
                            if (mMusicFlags & 1)
                            {
                                vcptr->mFrequencyDelta = -paramy << 6;
                            }
                            else
                            {
                                int per1, per2;

                                getAmigaPeriod(cptr->mRealNote + paramy, sptr->mFineTune, &per1);
                                getAmigaPeriod(cptr->mRealNote, sptr->mFineTune, &per2);

                                vcptr->mFrequencyDelta = per1 - per2;
                            }
                            break;
                        }
                    }
                    vcptr->mNoteControl |= FMUSIC_FREQ;
                }
                break;
            }
            case FMUSIC_XM_PORTAUP:
            {
                vcptr->mFrequencyDelta = 0;

                vcptr->mFrequency -= cptr->mPortaUp << 2;
                if (vcptr->mFrequency < 56)
                {
                    vcptr->mFrequency = 56;
                }
                vcptr->mNoteControl |= FMUSIC_FREQ;
                break;
            }
            case FMUSIC_XM_PORTADOWN:
            {
                vcptr->mFrequencyDelta = 0;

                vcptr->mFrequency += cptr->mPortaDown << 2;
                vcptr->mNoteControl |= FMUSIC_FREQ;
                break;
            }
            case FMUSIC_XM_PORTATO:
            {
                vcptr->mFrequencyDelta = 0;

                cptr->portamento();
                break;
            }
            case FMUSIC_XM_VIBRATO:
            {
                cptr->vibrato();

                cptr->mVibPos += cptr->mVibSpeed;
                if (cptr->mVibPos > 31)
                {
                    cptr->mVibPos -= 64;
                }
                break;
            }
            case FMUSIC_XM_PORTATOVOLSLIDE:
            {
                unsigned char slideup, slidedown; // Guessed names

                vcptr->mFrequencyDelta = 0;

                cptr->portamento();

                slideup = cptr->mVolumeSlide >> 4;
                slidedown = cptr->mVolumeSlide & 0xF;

                if (slideup)
                {
                    vcptr->mVolume += slideup;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (slidedown)
                {
                    vcptr->mVolume -= slidedown;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_XM_VIBRATOVOLSLIDE:
            {
                unsigned char slideup, slidedown; // Guessed names

                cptr->vibrato();

                cptr->mVibPos += cptr->mVibSpeed;
                if (cptr->mVibPos > 31)
                {
                    cptr->mVibPos -= 64;
                }

                slideup = cptr->mVolumeSlide >> 4;
                slidedown = cptr->mVolumeSlide & 0xF;

                if (slideup)
                {
                    vcptr->mVolume += slideup;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (slidedown)
                {
                    vcptr->mVolume -= slidedown;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_XM_TREMOLO:
            {
                cptr->tremolo();
                break;
            }
            case FMUSIC_XM_VOLUMESLIDE:
            {
                unsigned char slideup, slidedown; // Guessed names

                slideup = cptr->mVolumeSlide >> 4;
                slidedown = cptr->mVolumeSlide & 0xF;

                if (slideup)
                {
                    vcptr->mVolume += slideup;
                    if (vcptr->mVolume > 64)
                    {
                        vcptr->mVolume = 64;
                    }
                }
                else if (slidedown)
                {
                    vcptr->mVolume -= slidedown;
                    if (vcptr->mVolume < 0)
                    {
                        vcptr->mVolume = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_VOLUME;
                break;
            }
            case FMUSIC_XM_SPECIAL:
            {
                switch (paramx)
                {
                    case FMUSIC_XM_NOTECUT:
                    {
                        if (mTick == paramy)
                        {
                            vcptr->mVolume = 0;
                            vcptr->mNoteControl |= FMUSIC_VOLUME;
                        }
                        break;
                    }
                    case FMUSIC_XM_RETRIG:
                    {
                        if (paramy)
                        {
                            if (!(mTick % paramy))
                            {
                                vcptr->mNoteControl |= FMUSIC_TRIGGER;
                                vcptr->mNoteControl |= FMUSIC_VOLUME;
                                vcptr->mNoteControl |= FMUSIC_FREQ;
                            }
                        }
                        break;
                    }
                    case FMUSIC_XM_NOTEDELAY:
                    {
                        if (mTick == paramy)
                        {
                            getVirtualChannel(cptr, vcptr, sptr, &vcptr);

                            vcptr->mFrequency = cptr->mPeriod;
                            vcptr->mNoteControl |= FMUSIC_FREQ;
                            vcptr->mNoteControl |= FMUSIC_TRIGGER;

                            processNote(current, cptr, vcptr, iptr, sptr);
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
            case FMUSIC_XM_MULTIRETRIG:
            {
                if (cptr->mRetrigY && !(mTick % cptr->mRetrigY))
                {
                    if (cptr->mRetrigX)
                    {
                        switch (cptr->mRetrigX)
                        {
                            case 1:
                            {
                                vcptr->mVolume--;
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
                                vcptr->mVolume = vcptr->mVolume * 2 / 3;
                                break;
                            }
                            case 7:
                            {
                                vcptr->mVolume >>= 1;
                                break;
                            }
                            case 8:
                            {
                                break;
                            }
                            case 9:
                            {
                                vcptr->mVolume++;
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
                            case 0xE:
                            {
                                vcptr->mVolume = vcptr->mVolume * 3 / 2;
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

                    vcptr->mNoteControl |= FMUSIC_VOLUME;
                    vcptr->mNoteControl |= FMUSIC_TRIGGER;
                }
                break;
            }
            case FMUSIC_XM_GLOBALVOLSLIDE:
            {
                unsigned char slideup, slidedown; // Guessed names

                slideup = mGlobalVolumeSlide >> 4;
                slidedown = mGlobalVolumeSlide & 0xF;

                if (slideup)
                {
                    mGlobalVolume += slideup;
                    if (mGlobalVolume > 64)
                    {
                        mGlobalVolume = 64;
                    }
                }
                else if (slidedown)
                {
                    mGlobalVolume -= slidedown;
                    if (mGlobalVolume < 0)
                    {
                        mGlobalVolume = 0;
                    }
                }
                break;
            }
            case FMUSIC_XM_PANSLIDE:
            {
                unsigned char slideup, slidedown; // Guessed names

                slideup = cptr->mPanSlide >> 4;
                slidedown = cptr->mPanSlide & 0xF;

                if (slideup)
                {
                    vcptr->mPan += slideup;
                    if (vcptr->mPan > 255)
                    {
                        vcptr->mPan = 255;
                    }
                }
                else if (slidedown)
                {
                    vcptr->mPan -= slidedown;
                    if (vcptr->mPan < 0)
                    {
                        vcptr->mPan = 0;
                    }
                }

                vcptr->mNoteControl |= FMUSIC_PAN;
                break;
            }
            case FMUSIC_XM_TREMOR:
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
        }

        cptr->instrumentVibrato(iptr);

        updateFlags((MusicVirtualChannel *)cptr->mVirtualChannelHead.getNext(), sptr);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecXM::update(bool audible)
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

            updateNote();

            if (mNextRow == -1)
            {
                mNextRow = mRow + 1;
                if (mNextRow >= mPattern[mOrderList[mOrder]].mRows)
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
    }

    mPCMOffset += mMixerSamplesPerTick;

    return FMOD_OK;
}

FMOD_RESULT CodecXM::openInternal(unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    unsigned short filenumpatterns = 0;
    int count;
    unsigned int mainHDRsize;
    char str[256];
    unsigned char temp;
    FMOD_RESULT result;

    if (!mFile->mSeekable)
    {
        return FMOD_ERR_FORMAT;
    }

    init(FMOD_SOUND_TYPE_XM);

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

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(str, 1, 17, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(str, "Extended Module: ", 17))
    {
        return FMOD_ERR_FORMAT;
    }

    for (count = 0; count < 64; count++)
    {
        mMusicChannel[count] = 0;
    }

    mPattern = 0;
    mPanSeparation = 1.0f;
    mMasterSpeed = 1.0f;
    mDefaultSpeed = 6;
    mDefaultBPM = 125;
    mDefaultGlobalVolume = 64;
    mNumPatterns = 0;
    mRestart = 0;
    mNumSamples = 0;

    result = mFile->read(mSongName, 1, 20, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getByte(&temp);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (temp != 0x1A)
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->seek(60, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&mainHDRsize, 4, 1, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mNumOrders);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mRestart);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mNumChannels);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&filenumpatterns);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mNumInstruments);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mMusicFlags);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mDefaultSpeed);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->getWord(&mDefaultBPM);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(mOrderList, 1, 256, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = metaData(FMOD_TAGTYPE_FMOD, "Number of channels", &mNumChannels, sizeof(mNumChannels), FMOD_TAGDATATYPE_INT, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumChannels; count++)
    {
        mMusicChannel[count] = FMOD_Object_Calloc(MusicChannelXM);
        if (!mMusicChannel[count])
        {
            return FMOD_ERR_MEMORY;
        }
    }

    result = mFile->seek(mainHDRsize + 60, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mNumPatterns = 0;
    for (count = 0; count < mNumOrders; count++)
    {
        if (mOrderList[count] >= mNumPatterns)
        {
            mNumPatterns = mOrderList[count] + 1;
        }
    }

    mNumPatternsMem = (mNumPatterns > filenumpatterns ? mNumPatterns : filenumpatterns);

    mPattern = (MusicPattern *)FMOD_Memory_Calloc(mNumPatternsMem * sizeof(MusicPattern));
    if (!mPattern)
    {
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < filenumpatterns; count++)
    {
        unsigned int patternHDRsize;
        unsigned short patternsize, rows;
        MusicPattern * pptr = &mPattern[count];

        result = mFile->read(&patternHDRsize, 4, 1, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getByte((unsigned char *)0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getWord(&rows);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (result != FMOD_OK) // tested twice (0x805EE584/0x805EE58C)
        {
            return result;
        }

        result = mFile->getWord(&patternsize);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (result != FMOD_OK) // tested twice (0x805EE5A4/0x805EE5AC)
        {
            return result;
        }

        pptr->mRows = rows;

        pptr->mData = (MusicNote *)FMOD_Memory_Calloc(mNumChannels * pptr->mRows * sizeof(MusicNote));
        if (!pptr->mData)
        {
            return FMOD_ERR_MEMORY;
        }

        if (patternsize)
        {
            int count2;
            MusicNote * nptr = pptr->mData;

            for (count2 = 0; count2 < pptr->mRows * mNumChannels; count2++)
            {
                unsigned char dat;

                result = mFile->getByte(&dat);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (dat & 0x80)
                {
                    if (dat & 1)
                    {
                        result = mFile->getByte(&nptr->mNote);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                    if (dat & 2)
                    {
                        result = mFile->getByte(&nptr->mNumber);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                    if (dat & 4)
                    {
                        result = mFile->getByte(&nptr->mVolume);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                    if (dat & 8)
                    {
                        result = mFile->getByte(&nptr->mEffect);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                    if (dat & 16)
                    {
                        result = mFile->getByte(&nptr->mEffectParam);
                        if (result != FMOD_OK)
                        {
                            return result;
                        }
                    }
                }
                else
                {
                    if (dat)
                    {
                        nptr->mNote = dat;
                    }

                    result = mFile->getByte(&nptr->mNumber);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    result = mFile->getByte(&nptr->mVolume);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

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

                if (nptr->mNote == 97)
                {
                    nptr->mNote = 0xFF;
                }

                if (nptr->mNumber > 0x80)
                {
                    nptr->mNumber = 0;
                }

                nptr++;
            }
        }
    }

    if (mNumPatterns > filenumpatterns)
    {
        for (count = filenumpatterns; count < mNumPatterns; count++)
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

    mInstrument = (MusicInstrument *)FMOD_Memory_Calloc(mNumInstruments * sizeof(MusicInstrument));
    if (!mInstrument)
    {
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < mNumInstruments; count++)
    {
        unsigned int count2;
        MusicInstrument * iptr;
        unsigned int instHDRsize;
        unsigned short numsamples;
        unsigned char * buff = 0;
        unsigned int buffsize = 0; // Guessed name
        unsigned int firstsampleoffset;

        iptr = &mInstrument[count];

        mFile->tell(&firstsampleoffset);

        result = mFile->read(&instHDRsize, 4, 1, 0);
        if (result != FMOD_OK)
        {
            break;
        }

        firstsampleoffset += instHDRsize;

        result = mFile->read(iptr->mName, 1, 22, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getByte((unsigned char *)0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getWord(&numsamples);
        if (result != FMOD_OK)
        {
            return result;
        }

        iptr->mNumSamples = numsamples;

        if (numsamples)
        {
            unsigned int sampHDRsize;

            result = mFile->read(&sampHDRsize, 4, 1, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->read(iptr->mKeyMap, 1, 96, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->read(iptr->mVolumePoints, 2, 24, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->read(iptr->mPanPoints, 2, 24, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVolumeNumPoints);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mPanNumPoints);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVolumeSustain);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVolumeLoopStart);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVolumeLoopEnd);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mPanSustain);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mPanLoopStart);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mPanLoopEnd);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVolumeType);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mPanType);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVibratoType);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVibratoSweep);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVibratoDepth);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&iptr->mVibratoRate);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getWord(&iptr->mVolumeFade);
            if (result != FMOD_OK)
            {
                return result;
            }

            iptr->mVolumeFade *= 2;

            if (iptr->mVolumeNumPoints < 2)
            {
                iptr->mVolumeType = 0;
            }

            if (iptr->mPanNumPoints < 2)
            {
                iptr->mPanType = 0;
            }

            result = mFile->seek(firstsampleoffset, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            for (count2 = 0; count2 < numsamples; count2++)
            {
                MusicSample * sptr = &iptr->mSample[count2];
                char sample_name[22];
                unsigned int sample_mode;
                unsigned int sample_length;
                FMOD_SOUND_FORMAT sample_format; // Guessed name
                int sample_channels;
                unsigned char dat;

                memset(sptr, 0, sizeof(MusicSample));
                mNumSamples++;

                result = mFile->read(&sample_length, 4, 1, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->read(&sptr->mLoopStart, 4, 1, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->read(&sptr->mLoopLength, 4, 1, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->getByte(&sptr->mDefaultVolume);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->getByte(&sptr->mFineTune);
                if (result != FMOD_OK)
                {
                    return result;
                }

                sample_mode = FMOD_SOFTWARE | FMOD_2D;
                sample_channels = 1;
                sample_format = FMOD_SOUND_FORMAT_PCM8;

                result = mFile->getByte(&dat);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (dat & 1)
                {
                    sample_mode |= FMOD_LOOP_NORMAL;
                }
                if (dat & 2)
                {
                    sample_mode &= ~FMOD_LOOP_NORMAL;
                    sample_mode |= FMOD_LOOP_BIDI;
                }
                if (dat & 16)
                {
                    sample_format = FMOD_SOUND_FORMAT_PCM16;
                }
                if (dat & 32)
                {
                    sample_channels = 2;
                }

                if (!(sample_mode & FMOD_LOOP_NORMAL) && !(sample_mode & FMOD_LOOP_BIDI))
                {
                    sptr->mLoopStart = 0;
                    sptr->mLoopLength = sample_length;
                }

                if (!sptr->mLoopLength)
                {
                    sptr->mLoopStart = 0;
                    sptr->mLoopLength = sample_length;
                    sample_mode &= ~(FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);
                    sample_mode |= FMOD_LOOP_OFF;
                }

                sptr->mRawLength = sample_length;

                sptr->mLoopStart = sptr->mLoopStart / (sample_format == FMOD_SOUND_FORMAT_PCM8 ? 1 : 2) / sample_channels;
                sptr->mLoopLength = sptr->mLoopLength / (sample_format == FMOD_SOUND_FORMAT_PCM8 ? 1 : 2) / sample_channels;

                result = mFile->getByte(&sptr->mDefaultPan);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->getByte(&sptr->mRelative);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->getByte((unsigned char *)0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                result = mFile->read(sample_name, 1, 22, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (sample_length)
                {
                    FMOD_CREATESOUNDEXINFO exinfo;

                    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
                    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
                    exinfo.length = sample_length;
                    exinfo.numchannels = 1;
                    exinfo.defaultfrequency = 44100;
                    exinfo.format = sample_format;

                    result = mSystem->createSound(0, sample_mode | FMOD_OPENUSER, &exinfo, &sptr->mSound);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    if (sample_mode & FMOD_LOOP_NORMAL || sample_mode & FMOD_LOOP_BIDI)
                    {
                        sptr->mSound->setLoopPoints(sptr->mLoopStart, FMOD_TIMEUNIT_PCM, sptr->mLoopStart + sptr->mLoopLength - 1, FMOD_TIMEUNIT_PCM);
                    }
                }
            }

            for (; count2 < 16; count2++)
            {
                iptr->mSample[count2].mSound = 0;
            }

            for (count2 = 0; count2 < numsamples; count2++)
            {
                MusicSample * sptr = &iptr->mSample[count2];
                FMOD_SOUND_FORMAT format; // Guessed name
                int channels;
                unsigned int lenbytes = 0;

                if (sptr->mSound)
                {
                    lenbytes = sptr->mRawLength;
                    sptr->mSound->getFormat(0, &format, &channels, 0);
                }

                if (!lenbytes)
                {
                    continue;
                }

                if (lenbytes > buffsize)
                {
                    buff = (unsigned char *)FMOD_Memory_ReAlloc(buff, lenbytes + 16);
                    if (!buff)
                    {
                        return FMOD_ERR_MEMORY;
                    }
                    buffsize = lenbytes;
                }

                result = mFile->read(buff, 1, lenbytes, 0);
                if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
                {
                    return result;
                }

                if (!FMOD_strncmp((char *)buff + 4, "OggS", 4))
                {
                    FMOD_CREATESOUNDEXINFO exinfo;
                    unsigned int mode;

                    memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
                    exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
                    exinfo.length = lenbytes;
                    exinfo.numchannels = sptr->mSound->mChannels;
                    exinfo.defaultfrequency = (int)sptr->mSound->mDefaultFrequency;
                    exinfo.format = sptr->mSound->mFormat;
                    mode = sptr->mSound->mMode;

                    sptr->mSound->release();
                    sptr->mSound = 0;

                    mode &= ~FMOD_OPENUSER;
                    mode |= FMOD_OPENMEMORY;

                    result = mSystem->createSound((char *)buff + 4, mode, &exinfo, &sptr->mSound);
                    if (result == FMOD_OK)
                    {
                        if (mode & FMOD_LOOP_NORMAL || mode & FMOD_LOOP_BIDI)
                        {
                            result = sptr->mSound->setLoopPoints(sptr->mLoopStart, FMOD_TIMEUNIT_PCM, sptr->mLoopStart + sptr->mLoopLength - 1, FMOD_TIMEUNIT_PCM);
                            if (result != FMOD_OK)
                            {
                                return result;
                            }
                        }
                    }
                }
                else
                {
                    void * ptr1, * ptr2;
                    unsigned int len1, len2;
                    int newval, oldval;
                    unsigned int count3;

                    if (sptr->mSound->mMode & FMOD_LOOP_NORMAL) // tests mMode bit 0x2 (0x805EF098)
                    {
                        short * wptr = (short *)buff;

                        for (count3 = 0; count3 < lenbytes / 2; count3++)
                        {
                            wptr[count3] = FMOD_SWAPENDIAN_WORD((unsigned short)wptr[count3]);
                        }
                    }

                    result = sptr->mSound->lock(0, lenbytes, &ptr1, &ptr2, &len1, &len2);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }

                    if (ptr1 && len1)
                    {
                        memcpy(ptr1, buff, lenbytes);

                        oldval = 0;

                        if (format == FMOD_SOUND_FORMAT_PCM8)
                        {
                            unsigned char * bptr = (unsigned char *)ptr1;

                            for (count3 = 0; count3 < lenbytes; count3++)
                            {
                                newval = *bptr + oldval;
                                *bptr = (signed char)newval;
                                oldval = newval;
                                bptr++;
                            }
                        }
                        else if (format == FMOD_SOUND_FORMAT_PCM16)
                        {
                            unsigned short * wptr = (unsigned short *)ptr1;

                            for (count3 = 0; count3 < lenbytes / 2; count3++)
                            {
                                newval = *wptr + oldval;
                                *wptr = (short)newval;
                                oldval = newval;
                                wptr++;
                            }
                        }
                    }

                    result = sptr->mSound->unlock(ptr1, ptr2, len1, len2);
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }

            if (buff)
            {
                FMOD_Memory_Free(buff);
            }
        }
        else
        {
            for (count2 = 0; count2 < 16; count2++)
            {
                iptr->mSample[count2].mSound = 0;
            }

            result = mFile->seek(firstsampleoffset, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    mSample = 0;
    if (mNumSamples)
    {
        mSample = (MusicSample * *)FMOD_Memory_Calloc(mNumSamples * sizeof(MusicSample *));
        if (!mSample)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    {
        int count2, numsamples = 0;

        for (count = 0; count < mNumInstruments; count++)
        {
            MusicInstrument * iptr = &mInstrument[count];

            for (count2 = 0; count2 < iptr->mNumSamples; count2++)
            {
                mSample[numsamples] = &iptr->mSample[count2];
                numsamples++;
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
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        memset(&descriptionex, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

        FMOD_strcpy(descriptionex.name, "FMOD XM Target Unit");
        descriptionex.version = 0x00010100;
        descriptionex.channels = mWaveFormat.channels;
        descriptionex.mFormat = mWaveFormat.format;
        descriptionex.mCategory = FMOD_DSP_CATEGORY_SOUNDCARD;

        result = mSystem->createDSP(&descriptionex, &mDSPHead);
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
        int numrealchannels = mNumVirtualChannels * 2;

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

FMOD_RESULT CodecXM::closeInternal()
{
    int count;

    stop();

    if (mSample)
    {
        for (count = 0; count < mNumSamples; count++)
        {
            if (mSample[count] && mSample[count]->mSound)
            {
                mSample[count]->mSound->release();
                mSample[count]->mSound = 0;
                mSample[count] = 0;
            }
        }

        FMOD_Memory_Free(mSample);
        mSample = 0;
    }

    if (mInstrument)
    {
        FMOD_Memory_Free(mInstrument);
        mInstrument = 0;
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

FMOD_RESULT CodecXM::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
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

FMOD_RESULT CodecXM::setPositionInternal(int subsound, unsigned int position, unsigned int postype)
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

FMOD_RESULT CodecXM::openCallback(FMOD_CODEC_STATE * codec_state, unsigned int usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecXM *xm = (CodecXM *)codec_state;

    return xm->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecXM::closeCallback(FMOD_CODEC_STATE * codec_state)
{
    CodecXM *xm = (CodecXM *)codec_state;

    return xm->closeInternal();
}

FMOD_RESULT CodecXM::readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecXM *xm = (CodecXM *)codec_state;

    return xm->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecXM::setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, unsigned int postype)
{
    CodecXM *xm = (CodecXM *)codec_state;

    return xm->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
