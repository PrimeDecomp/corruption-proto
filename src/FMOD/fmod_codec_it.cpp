// G2MEAB prototype translation unit; partial reconstruction. openInternal 0x805CD074 and the per-tick
// note/effect processor updateNote 0x805CA318 are still empty placeholders.
// G2MEAB .text: 0x805C84F4..0x805D12C8 (48 retained native functions).
// Original basename directly named by target allocation/free evidence ("fmod_codec_it.cpp").
// Evidence: IT descriptor 0x8073FC0C registration 0x805C84F4, format 0xA/state 0x3C04, binds 0x805D11A4/
// 0x805D11D0/0x805D11FC/0x805D1228 to open 0x805CD074/close 0x805D08D4/read 0x805D0C80/seek 0x805D10D4.
// The packed-sample block allocation 0x805C8798 and free 0x805C8864 and the open/close allocations name
// this file; their __LINE__ values (0xE1, 0x102, 0x17DA..0x1827) are not reproduced by this partial file.
// The weak inlines emitted here between 0x805D01C4 and 0x805D0870 (MusicVirtualChannel/MusicChannelIT ctors,
// MusicChannel/MusicVirtualChannel dtors, ChannelGroupI and FMOD_DSP_DESCRIPTION_EX ctors, DSPI byte setters,
// SoundI::getBytesFromSamples, SystemI::getSoftwareFormat, Codec::init, placement new) and the
// LinkedListNode/math helpers at 0x805CCCE4..0x805CCD44 are emitted by the open and updateNote bodies.
// Final 0x805D1254 registers the IT descriptor and ends at 0x805D12C8, MIDI registration.

#include "fmod_codec_it.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_channelpool.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_music.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"
#include "fmod_types.h"

#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX itcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecIT::getDescriptionEx()
{
    memset(&itcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    itcodec.name = "FMOD IT Codec";
    itcodec.version = 0x00010100;
    itcodec.defaultasstream = 1;
    itcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_MODORDER | FMOD_TIMEUNIT_MODROW | FMOD_TIMEUNIT_MODPATTERN;
    itcodec.open = &CodecIT::openCallback;
    itcodec.close = &CodecIT::closeCallback;
    itcodec.read = &CodecIT::readCallback;
    itcodec.getlength = &MusicSong::getLengthCallback;
    itcodec.setposition = &CodecIT::setPositionCallback;
    itcodec.getposition = &MusicSong::getPositionCallback;

    itcodec.mType = FMOD_SOUND_TYPE_IT;
    itcodec.mSize = sizeof(CodecIT);

    return &itcodec;
}

FMOD_RESULT CodecIT::calculateLength()
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

FMOD_RESULT CodecIT::readBits(unsigned char bitwidth, unsigned int * result)
{
    unsigned int val;
    unsigned int data;

    if (bitwidth <= mRemBits)
    {
        val = *mSourcePos;
        val = FMOD_SWAPENDIAN_DWORD(val);
        data = val;
        val &= ((1 << bitwidth) - 1);
        data >>= bitwidth;
        *mSourcePos = FMOD_SWAPENDIAN_DWORD(data);
        mRemBits -= bitwidth;
    }
    else
    {
        unsigned int nbits = bitwidth - mRemBits;

        data = *mSourcePos++;
        val = FMOD_SWAPENDIAN_DWORD(data);
        data = *mSourcePos;
        data = FMOD_SWAPENDIAN_DWORD(data);
        val |= (data & ((1 << nbits) - 1)) << mRemBits;
        data >>= nbits;
        *mSourcePos = FMOD_SWAPENDIAN_DWORD(data);
        mRemBits = 32 - nbits;
    }

    if (result)
    {
        *result = val;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::readBlock(char * * buff)
{
    unsigned short size;

    size = (unsigned short)((*buff)[1] << 8);
    size |= ((*buff)[0] & 0xFF);
    *buff += 2;

    mSourceBuffer = (char *)FMOD_Memory_Alloc(size * 2);
    if (!mSourceBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    memcpy(mSourceBuffer, *buff, size);
    *buff += size;

    mSourcePos = (unsigned int *)mSourceBuffer;
    mRemBits = 32;

    return FMOD_OK;
}

FMOD_RESULT CodecIT::freeBlock()
{
    if (mSourceBuffer)
    {
        FMOD_Memory_Free(mSourceBuffer);
        mSourceBuffer = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::decompress8(char * * src, void * dst, int len, bool it215, int channels)
{
    FMOD_RESULT result;
    unsigned short blklen;
    unsigned short blkpos;
    unsigned char width;
    unsigned int value;
    signed char d1, d2;
    signed char * destpos;

    if (!dst)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!src || !*src)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    destpos = (signed char *)dst;

    while (len)
    {
        result = readBlock(src);
        if (result != FMOD_OK)
        {
            return result;
        }

        blklen = (len < 0x8000) ? len : 0x8000;
        blkpos = 0;
        width = 9;
        d1 = d2 = 0;

        while (blkpos < blklen)
        {
            signed char v;

            readBits(width, &value);

            if (width < 7)
            {
                if (value == (unsigned int)(1 << (width - 1)))
                {
                    readBits(3, &value);
                    value++;
                    width = (value < width) ? value : value + 1;
                    continue;
                }
            }
            else if (width < 9)
            {
                unsigned char border = (0xFF >> (9 - width)) - 4;

                if (value > border && value <= (unsigned int)(border + 8))
                {
                    value -= border;
                    width = (value < width) ? value : value + 1;
                    continue;
                }
            }
            else if (width == 9)
            {
                if (value & 0x100)
                {
                    width = (value + 1) & 0xFF;
                    continue;
                }
            }
            else
            {
                freeBlock();
                return FMOD_ERR_FORMAT;
            }

            if (width < 8)
            {
                signed char shift = 8 - width;

                v = (signed char)(value << shift);
                v >>= shift;
            }
            else
            {
                v = (signed char)value;
            }

            d1 += v;
            d2 += d1;

            *(destpos += channels) = it215 ? d2 : d1;
            blkpos++;
        }

        freeBlock();
        len -= blklen;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::decompress16(char * * src, void * dst, int len, bool it215, int channels)
{
    FMOD_RESULT result;
    unsigned short blklen;
    unsigned short blkpos;
    unsigned char width;
    unsigned int value;
    signed short d1, d2;
    signed short * destpos;

    if (!dst)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!src || !*src)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    destpos = (signed short *)dst;

    while (len)
    {
        result = readBlock(src);
        if (result != FMOD_OK)
        {
            return result;
        }

        blklen = (len < 0x4000) ? len : 0x4000;
        blkpos = 0;
        width = 17;
        d1 = d2 = 0;

        while (blkpos < blklen)
        {
            signed short v;

            readBits(width, &value);

            if (width < 7)
            {
                if (value == (unsigned int)(1 << (width - 1)))
                {
                    readBits(4, &value);
                    value++;
                    width = (value < width) ? value : value + 1;
                    continue;
                }
            }
            else if (width < 17)
            {
                unsigned short border = (0xFFFF >> (17 - width)) - 8;

                if (value > border && value <= (unsigned short)(border + 16))
                {
                    value -= border;
                    width = (value < width) ? value : value + 1;
                    continue;
                }
            }
            else if (width == 17)
            {
                if (value & 0x10000)
                {
                    width = (value + 1) & 0xFF;
                    continue;
                }
            }
            else
            {
                freeBlock();
                return FMOD_ERR_FORMAT;
            }

            if (width < 16)
            {
                unsigned char shift = 16 - width;

                v = (unsigned short)(value << shift);
                v >>= shift;
            }
            else
            {
                v = (signed short)value;
            }

            d1 += v;
            d2 += d1;

            *(destpos += channels) = it215 ? d2 : d1;
            blkpos++;
        }

        freeBlock();
        len -= blklen;
    }

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::volumeSlide()
{
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    if (!(mVolumeSlide & 0xF))
    {
        mVolume += (mVolumeSlide >> 4);
    }
    if (!(mVolumeSlide >> 4))
    {
        mVolume -= (mVolumeSlide & 0xF);
    }

    if (mVolume > 64)
    {
        mVolume = 64;
    }
    if (mVolume < 0)
    {
        mVolume = 0;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::portamento()
{
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();
    CodecIT * mod = mModule;

    if (mPortaReached)
    {
        return FMOD_OK;
    }

    if (vcptr->mFrequency < mPortaTarget)
    {
        if (mod->mMusicFlags & FMUSIC_ITFLAGS_EFFECT_G)
        {
            vcptr->mFrequency += mPortaSpeed << 2;
        }
        else
        {
            vcptr->mFrequency += mPortaUpDown << 2;
        }

        if (vcptr->mFrequency >= mPortaTarget)
        {
            vcptr->mFrequency = mPortaTarget;
            mPortaReached = true;
        }
    }
    else if (vcptr->mFrequency > mPortaTarget)
    {
        if (mod->mMusicFlags & FMUSIC_ITFLAGS_EFFECT_G)
        {
            vcptr->mFrequency -= mPortaSpeed << 2;
        }
        else
        {
            vcptr->mFrequency -= mPortaUpDown << 2;
        }

        if (vcptr->mFrequency < mPortaTarget)
        {
            vcptr->mFrequency = mPortaTarget;
            mPortaReached = true;
        }
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::vibrato()
{
    int delta;
    unsigned char temp;
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();
    CodecIT * mod = mModule;

    temp = (mVibPos & 31);

    switch (mWaveControlVibrato)
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
    delta <<= 1;

    if (mod->mMusicFlags & FMUSIC_ITFLAGS_OLD_EFFECTS)
    {
        delta <<= 1;
    }

    mVibPos += mVibSpeed;
    if (mVibPos > 31)
    {
        mVibPos -= 64;
    }

    if (mVibPos >= 0)
    {
        vcptr->mFrequencyDelta -= delta;
    }
    else
    {
        vcptr->mFrequencyDelta += delta;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::fineVibrato()
{
    int delta;
    unsigned char temp;
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();
    CodecIT * mod = mModule;

    temp = (mVibPos & 31);

    switch (mWaveControlVibrato)
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

    if (mod->mMusicFlags & FMUSIC_ITFLAGS_OLD_EFFECTS)
    {
        delta <<= 1;
    }

    if (mVibPos >= 0)
    {
        vcptr->mFrequencyDelta += delta;
    }
    else
    {
        vcptr->mFrequencyDelta -= delta;
    }

    mVibPos += mVibSpeed;
    if (mVibPos > 31)
    {
        mVibPos -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::tremolo()
{
    unsigned char temp;
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    temp = (mTremoloPosition & 31);

    switch (mWaveControlTremolo)
    {
        case 0:
        {
            mVolumeDelta = gSineTable[temp];
            break;
        }
        case 1:
        {
            temp <<= 3;
            if (mTremoloPosition < 0)
            {
                temp = 255 - temp;
            }
            mVolumeDelta = temp;
            break;
        }
        case 2:
        {
            mVolumeDelta = 255;
            break;
        }
        case 3:
        {
            mVolumeDelta = gSineTable[temp];
            break;
        }
    }

    mVolumeDelta *= mTremoloDepth;
    mVolumeDelta >>= 6;

    if (mTremoloPosition >= 0)
    {
        if (mVolume + mVolumeDelta > 64)
        {
            mVolumeDelta = 64 - mVolume;
        }
    }
    else
    {
        if ((short)(mVolume - mVolumeDelta) < 0)
        {
            mVolumeDelta = mVolume;
        }
        mVolumeDelta = -mVolumeDelta;
    }

    mTremoloPosition += mTremoloSpeed;
    if (mTremoloPosition > 31)
    {
        mTremoloPosition -= 64;
    }

    vcptr->mNoteControl |= FMUSIC_VOLUME;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::panbrello()
{
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();

    switch (mWaveControlPan)
    {
        case 0:
        {
            vcptr->mPanDelta = gFineSineTable[mPanbrelloPos];
            break;
        }
        case 1:
        {
            vcptr->mPanDelta = (128 - mPanbrelloPos) >> 1;
            break;
        }
        case 2:
        {
            if (mPanbrelloPos < 128)
            {
                vcptr->mPanDelta = 64;
            }
            else
            {
                vcptr->mPanDelta = -64;
            }
            break;
        }
        case 3:
        {
            vcptr->mPanDelta = gFineSineTable[mPanbrelloPos];
            break;
        }
    }

    vcptr->mPanDelta *= mPanbrelloDepth;
    vcptr->mPanDelta >>= 5;

    if (mPanbrelloPos >= 0)
    {
        if (vcptr->mPan + vcptr->mPanDelta > 64)
        {
            vcptr->mPanDelta = 64 - vcptr->mPan;
        }
    }
    else
    {
        if ((short)(vcptr->mPan - vcptr->mPanDelta) < 0)
        {
            vcptr->mPanDelta = vcptr->mPan;
        }
        vcptr->mPanDelta = -vcptr->mPanDelta;
    }

    mPanbrelloPos += mPanbrelloSpeed;
    if (mPanbrelloPos > 255)
    {
        mPanbrelloPos -= 256;
    }

    vcptr->mNoteControl |= FMUSIC_PAN;

    return FMOD_OK;
}

FMOD_RESULT CodecIT::processEnvelope(MusicEnvelopeState * env, MusicVirtualChannel * vcptr, int numpoints, MusicEnvelopeNode * points, int type, int loopstart, int loopend, int susloopstart, int susloopend, unsigned char control)
{
    if (env->mPosition < numpoints)
    {
        if (env->mTick != points[env->mPosition].mTick)
        {
            env->mFraction += env->mDelta;
            if (env->mFraction < 0 && type == FMUSIC_ENVELOPE_SUSTAIN)
            {
                env->mFraction = 0;
            }
        }
        else
        {
            while (env->mTick == points[env->mPosition].mTick && env->mPosition < numpoints)
            {
                int currpos, nextpos;
                int currtick, nexttick;
                int currval, nextval, tickdiff;

            Loop:
                currpos = env->mPosition;
                nextpos = env->mPosition + 1;

                currtick = points[currpos].mTick;
                nexttick = points[nextpos].mTick;

                currval = points[currpos].mValue << 16;
                nextval = points[nextpos].mValue << 16;

                if ((type & FMUSIC_ENVELOPE_SUSTAIN) && currpos >= susloopend && !vcptr->mKeyOff)
                {
                    if (susloopend == susloopstart)
                    {
                        env->mValue = points[currpos].mValue;
                        return FMOD_OK;
                    }

                    env->mPosition = susloopstart;
                    env->mTick = points[env->mPosition].mTick - 1;
                    goto Loop;
                }

                if ((type & FMUSIC_ENVELOPE_LOOP) && currpos >= loopend)
                {
                    if (loopend <= loopstart)
                    {
                        env->mValue = points[loopstart].mValue;
                        return FMOD_OK;
                    }

                    env->mPosition = loopstart;
                    env->mTick = points[env->mPosition].mTick - 1;
                    goto Loop;
                }

                if (currpos == numpoints - 1)
                {
                    env->mValue = points[currpos].mValue;
                    env->mStopped = true;
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
            }
        }
    }

    env->mValue = env->mFraction >> 16;
    env->mTick++;

    vcptr->mNoteControl |= control;

    return FMOD_OK;
}

// Non-linear IT pitch envelope value: the period difference between the note and the note shifted by
// value/2 semitones (half steps average the two neighbours), scaled to the sample's C5 speed. Guessed name.
static inline int getEnvelopePitch(MusicVirtualChannel * vcptr, int note, signed char value)
{
    if (value & 1)
    {
        unsigned int c5speed = vcptr->mSample->mMiddleC;
        int index = note + (value >> 1);

        return (int)(gITPeriodTable[note] * 8363 / c5speed) - ((int)(gITPeriodTable[index] * 8363 / c5speed) + (int)(gITPeriodTable[index + 1] * 8363 / c5speed)) / 2;
    }
    else
    {
        unsigned int c5speed = vcptr->mSample->mMiddleC;

        return gITPeriodTable[note] * 8363 / c5speed - gITPeriodTable[note + (value >> 1)] * 8363 / c5speed;
    }
}

FMOD_RESULT CodecIT::processPitchEnvelope(MusicVirtualChannel * vcptr, MusicInstrument * iptr, int note)
{
    if (vcptr->mEnvPitchPos < iptr->mPitchNumpoints)
    {
        MusicEnvelopeNode * points = (MusicEnvelopeNode *)iptr->mPitchPoints;

        if (vcptr->mEnvPitchTick != points[vcptr->mEnvPitchPos].mTick)
        {
            vcptr->mEnvPitchFrac += vcptr->mEnvPitchDelta;
        }
        else
        {
            while (vcptr->mEnvPitchTick == points[vcptr->mEnvPitchPos].mTick)
            {
                int currpos, nextpos;
                int currtick, nexttick;
                int currpitch, nextpitch, tickdiff;

            Loop:
                currpos = vcptr->mEnvPitchPos;
                nextpos = vcptr->mEnvPitchPos + 1;

                currtick = points[currpos].mTick;
                nexttick = points[nextpos].mTick;

                if ((mMusicFlags & FMUSIC_ITFLAGS_LINEARFREQUENCY) || (iptr->mPitchType & FMUSIC_ENVELOPE_FILTER))
                {
                    currpitch = points[currpos].mValue << 5;
                    nextpitch = points[nextpos].mValue << 5;
                }
                else
                {
                    currpitch = getEnvelopePitch(vcptr, note, points[currpos].mValue);
                    nextpitch = getEnvelopePitch(vcptr, note, points[nextpos].mValue);
                }

                if ((iptr->mPitchType & FMUSIC_ENVELOPE_SUSTAIN) && currpos >= iptr->mPitchSustainLoopEnd && !vcptr->mKeyOff)
                {
                    if (iptr->mPitchSustainLoopEnd == iptr->mPitchSustainLoopStart)
                    {
                        if ((mMusicFlags & FMUSIC_ITFLAGS_LINEARFREQUENCY) || (iptr->mPitchType & FMUSIC_ENVELOPE_FILTER))
                        {
                            vcptr->mEnvPitch = points[currpos].mValue << 5;
                        }
                        else
                        {
                            vcptr->mEnvPitch = getEnvelopePitch(vcptr, note, points[currpos].mValue);
                        }
                        return FMOD_OK;
                    }

                    vcptr->mEnvPitchPos = iptr->mPitchSustainLoopStart;
                    vcptr->mEnvPitchTick = points[vcptr->mEnvPitchPos].mTick - 1;
                    goto Loop;
                }

                if ((iptr->mPitchType & FMUSIC_ENVELOPE_LOOP) && currpos >= iptr->mPitchLoopEnd)
                {
                    if (iptr->mPitchLoopEnd <= iptr->mPitchLoopStart)
                    {
                        if ((mMusicFlags & FMUSIC_ITFLAGS_LINEARFREQUENCY) || (iptr->mPitchType & FMUSIC_ENVELOPE_FILTER))
                        {
                            vcptr->mEnvPitch = points[iptr->mPitchLoopStart].mValue << 5;
                        }
                        else
                        {
                            vcptr->mEnvPitch = getEnvelopePitch(vcptr, note, points[currpos].mValue);
                        }
                        return FMOD_OK;
                    }

                    vcptr->mEnvPitchPos = iptr->mPitchLoopStart;
                    vcptr->mEnvPitchTick = points[vcptr->mEnvPitchPos].mTick - 1;
                    goto Loop;
                }

                if (currpos == iptr->mPitchNumpoints - 1)
                {
                    if ((mMusicFlags & FMUSIC_ITFLAGS_LINEARFREQUENCY) || (iptr->mPitchType & FMUSIC_ENVELOPE_FILTER))
                    {
                        vcptr->mEnvPitch = points[currpos].mValue << 5;
                    }
                    else
                    {
                        vcptr->mEnvPitch = getEnvelopePitch(vcptr, note, points[currpos].mValue);
                    }
                    vcptr->mEnvPitchStopped = true;
                    return FMOD_OK;
                }

                tickdiff = nexttick - currtick;
                if (tickdiff)
                {
                    vcptr->mEnvPitchDelta = ((nextpitch - currpitch) << 16) / tickdiff;
                }
                else
                {
                    vcptr->mEnvPitchDelta = 0;
                }

                vcptr->mEnvPitchFrac = currpitch << 16;
                vcptr->mEnvPitchPos++;
            }
        }
    }

    if (!(iptr->mPitchType & FMUSIC_ENVELOPE_FILTER))
    {
        vcptr->mNoteControl |= FMUSIC_FREQ;
    }

    vcptr->mEnvPitch = vcptr->mEnvPitchFrac >> 16;
    vcptr->mEnvPitchTick++;

    return FMOD_OK;
}

FMOD_RESULT CodecIT::sampleVibrato(MusicVirtualChannel * vcptr)
{
    int delta = 0;
    MusicSample * sptr = vcptr->mSample;

    switch (sptr->mVibType)
    {
        case 0:
        {
            delta = gFineSineTable[vcptr->mIVibPos];
            break;
        }
        case 1:
        {
            delta = (128 - ((vcptr->mIVibPos + 128) % 256)) >> 1;
            break;
        }
        case 2:
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
        case 3:
        {
            delta = gFineSineTable[vcptr->mIVibPos];
            break;
        }
    }

    delta *= sptr->mVibDepth;
    delta = (delta * vcptr->mIVibSweepPos) >> 16;
    vcptr->mFrequencyDelta -= delta >> 7;

    vcptr->mIVibSweepPos += sptr->mVibRate << 1;
    if (vcptr->mIVibSweepPos > 65536)
    {
        vcptr->mIVibSweepPos = 65536;
    }

    vcptr->mIVibPos += sptr->mVibSpeed;
    if (vcptr->mIVibPos > 255)
    {
        vcptr->mIVibPos -= 256;
    }

    vcptr->mNoteControl |= FMUSIC_FREQ;

    return FMOD_OK;
}

FMOD_RESULT MusicChannelIT::processVolumeByte(MusicNote * current, bool firsttick)
{
    MusicVirtualChannel * vcptr = (MusicVirtualChannel *)mVirtualChannelHead.getNext();
    CodecIT * mod = mModule;
    unsigned char volume = current->mVolume - 1;

    if (firsttick)
    {
        if (volume <= 64)
        {
            mVolume = volume;
        }

        if (volume >= 65 && volume <= 74)
        {
            unsigned char param = volume - 65;

            if (param)
            {
                mVolumeColumnVolumeSlide = param;
            }

            mVolume += mVolumeColumnVolumeSlide;
            if (mVolume > 64)
            {
                mVolume = 64;
            }
        }

        if (volume >= 75 && volume <= 84)
        {
            unsigned char param = volume - 75;

            if (param)
            {
                mVolumeColumnVolumeSlide = param;
            }

            mVolume -= mVolumeColumnVolumeSlide;
            if (mVolume < 0)
            {
                mVolume = 0;
            }
        }

        if (volume >= 128 && volume <= 192)
        {
            mPan = volume - 128;
            vcptr->mPan = mPan;
            vcptr->mNoteControl |= FMUSIC_PAN;
        }
    }

    if (volume >= 85 && volume <= 94)
    {
        unsigned char param = volume - 85;

        if (param)
        {
            mVolumeColumnVolumeSlide = param;
        }

        if (!firsttick)
        {
            mVolume += mVolumeColumnVolumeSlide;
            if (mVolume > 64)
            {
                mVolume = 64;
            }
        }
    }

    if (volume >= 95 && volume <= 104)
    {
        unsigned char param = volume - 95;

        if (param)
        {
            mVolumeColumnVolumeSlide = param;
        }

        if (!firsttick)
        {
            mVolume -= mVolumeColumnVolumeSlide;
            if (mVolume < 0)
            {
                mVolume = 0;
            }
        }
    }

    if (volume >= 105 && volume <= 114)
    {
        unsigned char param = volume - 105;

        if (param)
        {
            mPortaUpDown = param;
        }

        vcptr->mFrequency += mPortaUpDown << 4;
    }

    if (volume >= 115 && volume <= 124)
    {
        unsigned char param = volume - 115;

        if (param)
        {
            mPortaUpDown = param;
        }

        vcptr->mFrequency -= mPortaUpDown << 4;
        if (vcptr->mFrequency < 1)
        {
            vcptr->mNoteControl |= FMUSIC_STOP;
        }
        else
        {
            vcptr->mNoteControl |= FMUSIC_FREQ;
        }
    }

    if (volume >= 193 && volume <= 202)
    {
        unsigned char param = volume - 193;

        if (!mod->mTick)
        {
            if (param)
            {
                if (mod->mMusicFlags & FMUSIC_ITFLAGS_EFFECT_G)
                {
                    mPortaSpeed = param << 4;
                }
                else
                {
                    mPortaUpDown = param << 4;
                }
            }

            mPortaTarget = mPeriod;
            if (current->mNote)
            {
                mPortaReached = false;
            }
        }
        else
        {
            portamento();
        }
    }

    if (volume >= 203 && volume <= 212)
    {
        unsigned char param = volume - 203;

        if (!mod->mTick)
        {
            if (param)
            {
                mVibDepth = param;
            }
            if (param)
            {
                mVibType = 8;
            }

            if (vcptr->mBackground)
            {
                return FMOD_OK;
            }

            if (!(mod->mMusicFlags & FMUSIC_ITFLAGS_OLD_EFFECTS))
            {
                if (mVibType == 21)
                {
                    fineVibrato();
                }
                else
                {
                    vibrato();
                }
            }
        }
        else
        {
            if (vcptr->mBackground)
            {
                return FMOD_OK;
            }

            if (mVibType == 21)
            {
                fineVibrato();
            }
            else
            {
                vibrato();
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::unpackRow()
{
    unsigned char channelvariable;

    if (!mPatternData)
    {
        return FMOD_ERR_INTERNAL;
    }

    memset(mNote, 0, mNumChannels * sizeof(MusicNote));

    do
    {
        channelvariable = *mPatternData++;

        if (channelvariable)
        {
            MusicNote * nptr;
            unsigned char maskvariable;
            int channel;

            channel = (channelvariable - 1) & 63;
            nptr = &mNote[channel];

            if (channelvariable & 128)
            {
                maskvariable = *mPatternData++;
                mPreviousMaskVariable[channel] = maskvariable;
            }
            else
            {
                maskvariable = mPreviousMaskVariable[channel];
            }

            if (maskvariable & 1)
            {
                unsigned char note = *mPatternData++;

                if (note >= 254)
                {
                    nptr->mNote = note;
                }
                else
                {
                    nptr->mNote = note + 1;
                }
                mLastNote[channel] = nptr->mNote;
            }
            if (maskvariable & 2)
            {
                nptr->mNumber = *mPatternData++;
                mLastNumber[channel] = nptr->mNumber;
            }
            if (maskvariable & 4)
            {
                nptr->mVolume = *mPatternData++ + 1;
                mLastVolume[channel] = nptr->mVolume;
            }
            if (maskvariable & 8)
            {
                nptr->mEffect = *mPatternData++;
                nptr->mEffectParam = *mPatternData++;
                mLastEffect[channel] = nptr->mEffect;
                mLastEffectParam[channel] = nptr->mEffectParam;
            }
            if (maskvariable & 16)
            {
                nptr->mNote = mLastNote[channel];
            }
            if (maskvariable & 32)
            {
                nptr->mNumber = mLastNumber[channel];
            }
            if (maskvariable & 64)
            {
                nptr->mVolume = mLastVolume[channel];
            }
            if (maskvariable & 128)
            {
                nptr->mEffect = mLastEffect[channel];
                nptr->mEffectParam = mLastEffectParam[channel];
            }
        }
    } while (channelvariable);

    return FMOD_OK;
}

FMOD_RESULT CodecIT::updateNote(bool audible)
{
    return FMOD_OK;
}

FMOD_RESULT CodecIT::update(bool audible)
{
    if (!mTick)
    {
        if (mNextOrder >= 0)
        {
            mOrder = mNextOrder;
            if (mNextOrder >= 0)
            {
                mOrder = mNextOrder;
            }

            while (mOrderList[mOrder] == 254)
            {
                mOrder++;
                if (mOrder >= mNumOrders)
                {
                    if (!mLooping)
                    {
                        stop();
                    }
                    mOrder = mRestart;
                }
            }

            if (mOrderList[mOrder] == 255)
            {
                mOrder = mRestart;
                mPatternData = (unsigned char *)mPattern[mOrderList[mOrder]].mData;
            }
        }

        if ((mNextRow >= 0 && mNextRow != mRow + 1) || mNextOrder >= 0)
        {
            int count;

            mPatternData = (unsigned char *)mPattern[mOrderList[mOrder]].mData;

            for (count = 0; count < mNextRow; count++)
            {
                unpackRow();
            }
        }

        if (mNextRow >= 0)
        {
            mRow = mNextRow;
            if (mNextRow >= 0)
            {
                mRow = mNextRow;
            }
            unpackRow();
        }

        mNextOrder = -1;
        mNextRow = -1;

        updateNote(audible);

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
    else
    {
        updateNote(audible);
    }

    mTick++;
    if (mTick >= mSpeed + mPatternDelay + mPatternDelayTicks)
    {
        mPatternDelay = 0;
        mPatternDelayTicks = 0;
        mTick = 0;
    }

    mPCMOffset += mMixerSamplesPerTick;

    return FMOD_OK;
}

FMOD_RESULT CodecIT::play()
{
    int pattern;

    MusicSong::play();

    do
    {
        pattern = mOrderList[mOrder];
        if (pattern >= mNumPatternsMem)
        {
            mOrder++;
        }
    } while (pattern >= mNumPatternsMem && mOrder < mNumOrders && mOrder < 255);

    if (pattern < mNumPatternsMem)
    {
        mPatternData = (unsigned char *)mPattern[pattern].mData;
        unpackRow();
    }
    else
    {
        mFinished = true;
        mPlaying = false;
        return FMOD_ERR_FORMAT;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return FMOD_OK;
}

FMOD_RESULT CodecIT::closeInternal()
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

    if (mLowPass)
    {
        for (count = 0; count < mUnk3A30; count++)
        {
            if (mLowPass[count])
            {
                mLowPass[count]->release(true);
            }
        }

        FMOD_Memory_Free(mLowPass);
        mLowPass = 0;
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
        for (count = 0; count < mNumPatternsMem; count++)
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

    for (count = 0; count < 50; count++)
    {
        if (mMixPlugin[count])
        {
            mMixPlugin[count]->mChannelGroup.mDSPHead->release(true);
            FMOD_Memory_Free(mMixPlugin[count]);
        }
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

    if (mDSPFinalHead)
    {
        mDSPFinalHead->release(true);
        mDSPFinalHead = 0;
    }

    if (mDSPEffectHead)
    {
        mDSPEffectHead->release(true);
        mDSPEffectHead = 0;
    }

    if (mDSPHead)
    {
        mDSPHead->release(true);
        mDSPHead = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecIT::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
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
                result = mDSPFinalHead->execute(buff, &buff, &read, numchannels, &numchannels, FMOD_SPEAKERMODE_STEREO_LINEAR);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mDSPFinalHead->resetVisited();
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
    }

    if (bytesread)
    {
        *bytesread = sizebytes;
    }

    return result;
}

FMOD_RESULT CodecIT::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
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
            bool playing = mPlaying;
            bool finished = mFinished;

            stop();

            mPlaying = playing;
            mFinished = finished;
        }

        return FMOD_OK;
    }

    return FMOD_ERR_FORMAT;
}

FMOD_RESULT CodecIT::openCallback(FMOD_CODEC_STATE * codec_state, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecIT * cit = (CodecIT *)codec_state;

    return cit->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecIT::closeCallback(FMOD_CODEC_STATE * codec_state)
{
    CodecIT * cit = (CodecIT *)codec_state;

    return cit->closeInternal();
}

FMOD_RESULT CodecIT::readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecIT * cit = (CodecIT *)codec_state;

    return cit->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecIT::setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecIT * cit = (CodecIT *)codec_state;

    return cit->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
