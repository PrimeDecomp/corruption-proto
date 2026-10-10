// G2MEAB prototype translation unit; every native function is reconstructed (NonMatching).
// .text: 0x80621870..0x80622C90 (18 native functions): ctor 0x80621870, the 14 overrides of vtable
// 0x806EFC70, the AX frame helpers updateStart 0x80621F38 / updateStop 0x80621FF4 and the empty
// AXAcquireVoice callback 0x80622C8C. No 4.06 counterpart; the asserted basename is the OSPanic
// filename of init 0x806218D4. Its .data (volume table 0x806EF7C0, jump tables, vtable, strings
// 0x806EFD04) is not split.

#include "fmod_channel_gc.h"
#include "fmod.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_codeci.h"
#include "fmod_memory.h"
#include "fmod_output_gc.h"
#include "fmod_soundi.h"

#include <dolphin/ax.h>
#include <dolphin/mix.h>
#include <dolphin/os.h>
#include <string.h>

namespace FMOD {

int gGCVolumeTable[256] =
{
    -960, -481, -421, -385, -360, -341, -325, -312, -300, -290, -281, -273, -265, -258, -252, -246,
    -240, -235, -230, -225, -221, -216, -212, -208, -205, -201, -198, -195, -191, -188, -185, -183,
    -180, -177, -175, -172, -170, -167, -165, -163, -160, -158, -156, -154, -152, -150, -148, -146,
    -145, -143, -141, -139, -138, -136, -134, -133, -131, -130, -128, -127, -125, -124, -122, -121,
    -120, -118, -117, -116, -114, -113, -112, -111, -109, -108, -107, -106, -105, -104, -102, -101,
    -100, -99, -98, -97, -96, -95, -94, -93, -92, -91, -90, -89, -88, -87, -86, -85,
    -84, -83, -83, -82, -81, -80, -79, -78, -77, -77, -76, -75, -74, -73, -73, -72,
    -71, -70, -69, -69, -68, -67, -66, -66, -65, -64, -64, -63, -62, -61, -61, -60,
    -59, -59, -58, -57, -57, -56, -55, -55, -54, -53, -53, -52, -52, -51, -50, -50,
    -49, -49, -48, -47, -47, -46, -46, -45, -44, -44, -43, -43, -42, -42, -41, -41,
    -40, -39, -39, -38, -38, -37, -37, -36, -36, -35, -35, -34, -34, -33, -33, -32,
    -32, -31, -31, -30, -30, -29, -29, -28, -28, -27, -27, -26, -26, -26, -25, -25,
    -24, -24, -23, -23, -22, -22, -21, -21, -21, -20, -20, -19, -19, -18, -18, -18,
    -17, -17, -16, -16, -16, -15, -15, -14, -14, -14, -13, -13, -12, -12, -12, -11,
    -11, -10, -10, -10, -9, -9, -8, -8, -8, -7, -7, -7, -6, -6, -5, -5,
    -5, -4, -4, -4, -3, -3, -3, -2, -2, -2, -1, -1, -1, 0, 0, 0
};

ChannelGC::ChannelGC()
{
    mVoice = 0;
    mUnk80 = false;
    mPendingStart = false;
    mPendingStop = false;
    mReverbRoomA = 0;
    mReverbRoomB = 0;
}

FMOD_RESULT ChannelGC::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
    FMOD_RESULT result;

    result = ChannelReal::init(index, system, output, dspmixtarget);
    if (result != FMOD_OK)
    {
        return result;
    }

    mOutputGC = (OutputGC *)output;
    mUnk80 = false;

    mVoice = AXAcquireVoice(15, voiceCallback, (u32)this);
    if (!mVoice)
    {
        OSPanic("fmod_channel_gc.cpp", 117, "Couldn't aquire voice\n");
    }

    OSDisableInterrupts();
    if (mVoice)
    {
        MIXInitChannel(mVoice, 0, 0, 0, 0, 64, 127, 0);
    }
    OSRestoreInterrupts(TRUE);

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::alloc()
{
    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    ((SampleGC *)mSound)->mChannel = this;

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::start()
{
    FMOD_RESULT result;
    SampleGC * sample;
    MemBlockHeader * block;
    unsigned int lengthbytes;
    unsigned int loopstart;
    unsigned int loopend;
    int startaddress;
    unsigned int loopaddress;
    unsigned int endaddress;
    unsigned int ratio;
    unsigned int mixmode = 0;
    AXPBADPCM adpcm;
    AXPBADDR addr;
    AXPBSRC src;
    AXPBADPCMLOOP adpcmloop;

    sample = (SampleGC *)mSound;
    if (!sample)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    block = (MemBlockHeader *)sample->mARAMBlock;

    result = SoundI::getBytesFromSamples(sample->mLength, &lengthbytes, sample->mChannels, sample->mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }

    loopstart = sample->mLoopStart;
    loopend = sample->mLoopStart + sample->mLoopLength;
    if (sample->mFormat == FMOD_SOUND_FORMAT_PCM16)
    {
        loopstart *= 2;
        loopend *= 2;
    }

    startaddress = mOutputGC->mARAMBase + block->mBlockOffset * mOutputGC->mARAMPool.mBlockSize;

    if (sample->mFormat == FMOD_SOUND_FORMAT_GCADPCM)
    {
        /*
            ADPCM addresses are in nibbles: 14 samples per 8-byte frame after a 2-nibble header.
        */
        startaddress *= 2;
        loopend -= 1;
        loopend = loopend % 14 + (loopend / 14 * 16 + 2);
        loopstart = loopstart % 14 + (loopstart / 14 * 16 + 2);
        endaddress = startaddress + loopend;
        if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
        {
            loopaddress = startaddress + loopstart;
        }
        else
        {
            loopaddress = mOutputGC->mSilenceAddress * 2 + 2;
        }
        startaddress += 2;
    }
    else if (sample->mFormat == FMOD_SOUND_FORMAT_PCM16)
    {
        startaddress >>= 1;
        endaddress = startaddress + loopend / 2 - 1;
        if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
        {
            loopaddress = startaddress + loopstart / 2;
        }
        else
        {
            loopaddress = mOutputGC->mSilenceAddress / 2;
        }
    }
    else if (sample->mFormat == FMOD_SOUND_FORMAT_PCM8)
    {
        endaddress = startaddress + loopend - 1;
        if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
        {
            loopaddress = startaddress + loopstart;
        }
        else
        {
            loopaddress = mOutputGC->mSilenceAddress;
        }
    }

    if (sample->mFormat == FMOD_SOUND_FORMAT_GCADPCM)
    {
        addr.format = 0; /* AX ADPCM */
    }
    else if (sample->mFormat & FMOD_SOUND_FORMAT_PCM16)
    {
        addr.format = 0xA; /* AX PCM16 */

        OSDisableInterrupts();
        if (mVoice)
        {
            AXVPB * voice = mVoice;

            voice->pb.adpcm.gain = 0x800;
            voice->sync |= AX_SYNC_FLAG_COPYADPCM;
        }
        OSRestoreInterrupts(TRUE);
    }
    else if (sample->mFormat == FMOD_SOUND_FORMAT_PCM8)
    {
        addr.format = 0x19; /* AX PCM8 */

        OSDisableInterrupts();
        if (mVoice)
        {
            AXVPB * voice = mVoice;

            voice->pb.adpcm.gain = 0x100;
            voice->sync |= AX_SYNC_FLAG_COPYADPCM;
        }
        OSRestoreInterrupts(TRUE);
    }

    addr.currentAddressHi = (u16)(startaddress >> 16);
    addr.currentAddressLo = (u16)(startaddress & 0xFFFF);
    addr.loopFlag = (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI)) ? (u16)1 : (u16)0;
    addr.loopAddressHi = (u16)(loopaddress >> 16);
    addr.loopAddressLo = (u16)(loopaddress & 0xFFFF);
    addr.endAddressHi = (u16)(endaddress >> 16);
    addr.endAddressLo = (u16)(endaddress & 0xFFFF);

    OSDisableInterrupts();
    if (mVoice)
    {
        AXSetVoiceAddr(mVoice, &addr);
    }
    OSRestoreInterrupts(TRUE);

    ratio = (unsigned int)(65536.0f * (sample->mDefaultFrequency / 32000.0f));
    src.ratioHi = (u16)(ratio >> 16);
    src.ratioLo = (u16)(ratio & 0xFFFF);
    src.currentAddressFrac = 0;
    src.last_samples[0] = 0;
    src.last_samples[1] = 0;
    src.last_samples[2] = 0;
    src.last_samples[3] = 0;

    OSDisableInterrupts();
    if (mVoice)
    {
        AXSetVoiceSrcType(mVoice, AX_SRC_TYPE_LINEAR);
        AXSetVoiceSrc(mVoice, &src);
    }
    OSRestoreInterrupts(TRUE);

    if (sample->mFormat == FMOD_SOUND_FORMAT_GCADPCM)
    {
        unsigned char * info = ((unsigned char * *)sample->mCodec->plugindata)[sample->mSubSoundIndex];

        memcpy(&adpcm, info, sizeof(AXPBADPCM));
        memcpy(&adpcmloop, info + sizeof(AXPBADPCM), sizeof(AXPBADPCMLOOP));

        OSDisableInterrupts();
        if (mVoice)
        {
            AXSetVoiceAdpcm(mVoice, &adpcm);
        }
        OSRestoreInterrupts(TRUE);

        if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
        {
            OSDisableInterrupts();
            if (mVoice)
            {
                AXSetVoiceAdpcmLoop(mVoice, &adpcmloop);
            }
            OSRestoreInterrupts(TRUE);
        }
    }

    OSDisableInterrupts();
    if (mVoice)
    {
        if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
        {
            AXSetVoiceType(mVoice, 1); /* AX stream (looping) */
        }
        else
        {
            AXSetVoiceType(mVoice, 0); /* AX normal */
        }
    }
    OSRestoreInterrupts(TRUE);

    if (sample->mMode & (FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mStartAddress = *(u32 *)&mVoice->pb.addr.loopAddressHi;
    }
    else
    {
        mStartAddress = *(u32 *)&mVoice->pb.addr.currentAddressHi;
    }

    if (MIXIsMute(mVoice))
    {
        mixmode |= 4; /* mute */
    }
    if (!MIXAuxAIsPostFader(mVoice))
    {
        mixmode |= 1; /* aux A pre-fader */
    }
    if (!MIXAuxBIsPostFader(mVoice))
    {
        mixmode |= 2; /* aux B pre-fader */
    }

    MIXInitChannel(mVoice, mixmode, gGCVolumeTable[0], MIXGetAuxA(mVoice), MIXGetAuxB(mVoice), MIXGetPan(mVoice), MIXGetSPan(mVoice), MIXGetFader(mVoice));

    mPendingStart = true;
    mPendingPosition = (unsigned int)-1;

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::updateStart()
{
    unsigned int position = *(u32 *)&mVoice->pb.addr.currentAddressHi;

    if (mVoice->pb.addr.format == 0)
    {
        position >>= 1;
    }
    else if (mVoice->pb.addr.format == 0xA)
    {
        position <<= 1;
    }

    if (position > mOutputGC->mSilenceAddress + 0x100)
    {
        if (mPendingPosition != (unsigned int)-1)
        {
            AXSetVoiceCurrentAddr(mVoice, mPendingPosition);
        }
        if (!(mFlags & CHANNELREAL_FLAG_PAUSED))
        {
            AXSetVoiceState(mVoice, 1);
        }
        MIXSetInput(mVoice, mInput);
    }

    mPendingStart = false;

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::stop(bool force, bool updateflags)
{
    mPendingStop = true;

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::updateStop()
{
    if (mPendingStop)
    {
        AXSetVoiceState(mVoice, 0);
    }

    mPendingStop = false;

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::setVolume(float volume)
{
    if (!mVoice)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    volume *= mParent->mVolumeOcclusion;
    volume *= mParent->mVolume3D;
    volume *= mParent->mConeVolume3D;
    volume *= mParent->mChannelGroup->mRealVolume;

    mInput = gGCVolumeTable[(int)(255.0f * volume)];

    OSDisableInterrupts();
    if (mVoice)
    {
        MIXSetInput(mVoice, mInput);
    }
    OSRestoreInterrupts(TRUE);

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::setFrequency(float frequency)
{
    if (mVoice)
    {
        float ratio;

        frequency *= mParent->mPitch3D;
        frequency *= mParent->mChannelGroup->mRealPitch;
        ratio = frequency / 32000.0f;

        OSDisableInterrupts();
        if (mVoice)
        {
            AXSetVoiceSrcRatio(mVoice, ratio);
        }
        OSRestoreInterrupts(TRUE);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::setPan(float pan, float fbpan)
{
    if (mVoice)
    {
        float value;

        value = 63.5 * (1.0f + pan);

        OSDisableInterrupts();
        if (mVoice)
        {
            MIXSetPan(mVoice, (int)(0.5f + value));
        }
        OSRestoreInterrupts(TRUE);

        value = 63.5 * (1.0f + fbpan);

        OSDisableInterrupts();
        if (mVoice)
        {
            MIXSetSPan(mVoice, (int)(0.5f + value));
        }
        OSRestoreInterrupts(TRUE);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    float pan = (frontright + backright) / 2.0f - (frontleft + backleft) / 2.0f;
    float fbpan = (frontleft + frontright) / 2.0f - (backleft + backright) / 2.0f;

    return setPan(pan, fbpan);
}

FMOD_RESULT ChannelGC::setPaused(bool paused)
{
    if (mVoice && !mPendingStart)
    {
        if (paused)
        {
            OSDisableInterrupts();
            if (mVoice)
            {
                AXSetVoiceState(mVoice, 0);
            }
            OSRestoreInterrupts(TRUE);
        }
        else
        {
            unsigned int position = *(u32 *)&mVoice->pb.addr.currentAddressHi;

            if (mVoice->pb.addr.format == 0)
            {
                position >>= 1;
            }
            else if (mVoice->pb.addr.format == 0xA)
            {
                position <<= 1;
            }

            if (position > mOutputGC->mSilenceAddress + 0x100)
            {
                AXSetVoiceState(mVoice, 1);
                MIXSetInput(mVoice, mInput);
            }
        }
    }

    return ChannelReal::setPaused(paused);
}

FMOD_RESULT ChannelGC::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    unsigned int offset = position;
    AXVPB * voice = mVoice;
    unsigned int address;

    if (voice->pb.addr.format == 0)
    {
        unsigned int frames = position / 16 * 14;
        unsigned int nibble = position % 16;

        if (nibble)
        {
            position = frames + nibble;
            position -= 2;
        }
        else
        {
            position = frames;
        }
    }

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        offset = position;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(position, &offset, mSound->mChannels, mSound->mFormat);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        offset = (unsigned int)(mSound->mDefaultFrequency * ((float)position / 1000.0f));
    }

    offset = mStartAddress + offset;
    if (offset > *(u32 *)&voice->pb.addr.endAddressHi)
    {
        address = *(u32 *)&voice->pb.addr.endAddressHi;
    }
    else
    {
        address = offset;
    }

    if (mPendingStart)
    {
        mPendingPosition = address;
    }
    else
    {
        AXSetVoiceCurrentAddr(voice, address);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    unsigned int address;
    unsigned int pcm = 0;
    bool playing;

    if (!position)
    {
        return FMOD_OK;
    }

    *position = 0;

    if (!mVoice || !mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }

    isPlaying(&playing);
    if (!playing || (mFlags & CHANNELREAL_FLAG_ENDDELAY))
    {
        mSound->getLength(position, FMOD_TIMEUNIT_PCM);
        return FMOD_OK;
    }

    OSDisableInterrupts();
    if (mVoice)
    {
        address = *(u32 *)&mVoice->pb.addr.currentAddressHi;
        pcm = address - mStartAddress;

        if (mVoice->pb.addr.format == 0)
        {
            unsigned int frames;
            unsigned int nibble;

            address >>= 1;
            frames = pcm / 16 * 14;
            nibble = pcm % 16;
            if (nibble)
            {
                pcm = frames + nibble;
                pcm -= 2;
            }
            else
            {
                pcm = frames;
            }
        }
        else if (mVoice->pb.addr.format == 0xA)
        {
            address <<= 1;
        }
    }
    OSRestoreInterrupts(TRUE);

    if (address < mOutputGC->mSilenceAddress + 0x100)
    {
        pcm = 0;
    }

    if (postype == FMOD_TIMEUNIT_PCM)
    {
        *position = pcm;
    }
    else if (postype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getBytesFromSamples(pcm, position, mSound->mChannels, mSound->mFormat);
    }
    else if (postype == FMOD_TIMEUNIT_MS)
    {
        *position = (unsigned int)(1000.0f * ((float)pcm / mSound->mDefaultFrequency));
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::isPlaying(bool * isplaying)
{
    AXVPB * voice;

    if ((mFlags & CHANNELREAL_FLAG_ALLOCATED) || (mFlags & CHANNELREAL_FLAG_PAUSED) || (mFlags & CHANNELREAL_FLAG_ENDDELAY))
    {
        if (isplaying)
        {
            *isplaying = true;
        }
        return FMOD_OK;
    }

    voice = mVoice;
    if (voice)
    {
        if (voice->pb.state == 0)
        {
            *isplaying = false;
            return FMOD_OK;
        }

        OSDisableInterrupts();
        if (mVoice)
        {
            unsigned int position = *(u32 *)&voice->pb.addr.currentAddressHi;

            if (voice->pb.addr.format == 0)
            {
                position >>= 1;
            }
            else if (voice->pb.addr.format == 0xA)
            {
                position <<= 1;
            }

            if (position < mOutputGC->mSilenceAddress + 0x100)
            {
                *isplaying = false;
            }
            else
            {
                *isplaying = true;
            }
        }
        OSRestoreInterrupts(TRUE);
    }
    else
    {
        *isplaying = false;
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

FMOD_RESULT ChannelGC::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    int level = prop->Room + 10000;

    level /= 39;

    if (level > 255)
    {
        level = 255;
    }

    if (prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2)
    {
        return FMOD_ERR_REVERB_INSTANCE;
    }

    if ((prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0) && gReverbA.mActive)
    {
        MIXSetAuxA(mVoice, gGCVolumeTable[level]);
        mReverbRoomA = prop->Room;
    }

    if ((prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1) && gReverbB.mActive)
    {
        if (gReverbB.mType == 5)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        MIXSetAuxB(mVoice, gGCVolumeTable[level]);
        mReverbRoomB = prop->Room;
    }

    if (!(prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0) && !(prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1) && gReverbA.mActive)
    {
        MIXSetAuxA(mVoice, gGCVolumeTable[level]);
        mReverbRoomA = prop->Room;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelGC::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    if (!prop)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    memset(prop, 0, sizeof(FMOD_REVERB_CHANNELPROPERTIES));

    if (prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0)
    {
        prop->Room = mReverbRoomA;
    }
    if (prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1)
    {
        prop->Room = mReverbRoomB;
    }
    if (!(prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0) && !(prop->Flags & FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1))
    {
        prop->Room = mReverbRoomA;
    }

    return FMOD_OK;
}

void ChannelGC::voiceCallback(void * voice)
{
}

} // namespace FMOD
