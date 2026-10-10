// G2MEAB prototype translation unit (no 4.06 counterpart); reconstructed from the native code.
// .text: 0x806236F0..0x8062591C (40 native functions, including __sinit 0x8062554C and the weak
// SampleGC destructor 0x80625854 emitted last).
// Evidence: getDescriptionEx 0x80623754 fills the descriptor 0x80756084 ("FMOD GameCube Output",
// type FMOD_OUTPUTTYPE_GC, size 0x198). init 0x80623AE8 brings up AR/ARQ/AI/AX/MIX, carves a custom
// MemPool over ARAM, uploads a silence block and the 0x1800-byte ring, and acquires two stereo AX
// voices that loop over the ring; the AX frame callback 0x80623920 flips the ring half and wakes the
// "Gamecube Mixer Thread" 0x8062386C, which mixes 0x300 samples into the half and DMAs it to ARAM
// (unlock 0x80625254). SampleGC 0x806255CC..0x80625854 keeps samples in ARAM blocks.

#include "fmod_output_gc.h"
#include "fmod.h"
#include "fmod_channel_gc.h"
#include "fmod_channelpool.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_os_output.h"
#include "fmod_outputi.h"
#include "fmod_pluginfactory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"
#include "fmod_thread.h"

#include <dolphin/ai.h>
#include <dolphin/ar.h>
#include <dolphin/ax.h>
#include <dolphin/axfx.h>
#include <dolphin/mix.h>
#include <dolphin/os.h>

#include <string.h>

namespace FMOD {

FMOD_OUTPUT_DESCRIPTION_EX gcoutput; // Guessed name, 0x80756084
ReverbGC gReverbA; // Guessed name, 0x8075611C
ReverbGC gReverbB; // Guessed name, 0x807566C4
Thread gMixerThread; // Guessed name, 0x80756C6C
ARQRequest gDMARequestLeft; // Guessed name, 0x80756D90
ARQRequest gDMARequestRight; // Guessed name, 0x80756DB0

AXVPB * gVoiceLeft; // Guessed name, 0x8079B880
AXVPB * gVoiceRight; // Guessed name, 0x8079B884
FMOD_OS_SEMAPHORE * gDMASemaphore; // Guessed name, 0x8079B888
ChannelPool * gChannelPool; // Guessed name, 0x8079B88C
int gCurrentHalf; // Guessed name, 0x8079B890
unsigned int gLeftStart; // Guessed name, 0x8079B894
unsigned int gLeftBuffer; // Guessed name, 0x8079B898
unsigned int gLeftHalf; // Guessed name, 0x8079B89C
unsigned int gRightBuffer; // Guessed name, 0x8079B8A0
unsigned int gRightHalf; // Guessed name, 0x8079B8A4
unsigned int gLastPosition; // Guessed name, 0x8079B8A8
unsigned int gBlockSize; // Guessed name, 0x8079B8AC

} // namespace FMOD

FMOD_RESULT FMOD_OS_Output_Register(FMOD::PluginFactory * pluginfactory)
{
    pluginfactory->registerOutput(FMOD::OutputGC::getDescriptionEx(), 0, 0);

    return FMOD_OK;
}

FMOD_RESULT FMOD_OS_Output_GetDefault(FMOD_OUTPUTTYPE * outputtype)
{
    if (!outputtype)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *outputtype = FMOD_OUTPUTTYPE_GC;

    return FMOD_OK;
}

namespace FMOD {

FMOD_OUTPUT_DESCRIPTION_EX * OutputGC::getDescriptionEx()
{
    memset(&gcoutput, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    gcoutput.name = "FMOD GameCube Output";
    gcoutput.version = 0x00010100;
    gcoutput.polling = 0;
    gcoutput.getnumdrivers = &OutputGC::getNumDriversCallback;
    gcoutput.getdrivername = &OutputGC::getDriverNameCallback;
    gcoutput.init = &OutputGC::initCallback;
    gcoutput.close = &OutputGC::closeCallback;
    gcoutput.start = &OutputGC::startCallback;
    gcoutput.stop = &OutputGC::stopCallback;
    gcoutput.update = &OutputGC::updateCallback;
    gcoutput.gethandle = &OutputGC::getHandleCallback;
    gcoutput.getposition = 0;
    gcoutput.lock = 0;
    gcoutput.unlock = 0;
    gcoutput.reverb_setproperties = &OutputGC::setReverbPropertiesCallback;
    gcoutput.getsoundram = &OutputGC::getSoundRAMCallback;
    gcoutput.createsample = &OutputGC::createSampleCallback;

    gcoutput.mType = FMOD_OUTPUTTYPE_GC;
    gcoutput.mSize = sizeof(OutputGC);

    return &gcoutput;
}

void OutputGC::mixThreadCallback(void * data)
{
    OutputGC * gc = (OutputGC *)data;

    gc->mixThread();
}

FMOD_RESULT OutputGC::mixThread()
{
    void * ptr1 = 0;
    void * ptr2 = 0;
    unsigned int len1 = 0;
    unsigned int len2 = 0;

    mSystem->mDSPTimeStamp.stampIn();

    lock(gCurrentHalf * 0x1800 / 2, 0xC00, &ptr1, &ptr2, &len1, &len2);
    mix(ptr1, 0x300);
    unlock(ptr1, ptr2, len1, len2);

    mSystem->mDSPTimeStamp.stampOut(95);

    return FMOD_OK;
}

void OutputGC::frameCallback()
{
    bool wakeup = false;

    if (gVoiceRight)
    {
        unsigned int position;
        int count;

        if (gChannelPool)
        {
            int numchannels;

            gChannelPool->getNumChannels(&numchannels);
            for (count = 0; count < numchannels; count++)
            {
                ChannelReal * channel;

                gChannelPool->getChannel(count, &channel);
                if (((ChannelGC *)channel)->mPendingStop)
                {
                    ((ChannelGC *)channel)->updateStop();
                }
            }
        }

        MIXUpdateSettings();

        if (gChannelPool)
        {
            int numchannels;

            gChannelPool->getNumChannels(&numchannels);
            for (count = 0; count < numchannels; count++)
            {
                ChannelReal * channel;

                gChannelPool->getChannel(count, &channel);
                if (((ChannelGC *)channel)->mPendingStart)
                {
                    ((ChannelGC *)channel)->updateStart();
                }
            }
        }

        position = *(unsigned int *)&gVoiceRight->pb.addr.currentAddressHi;

        if (position < gLastPosition)
        {
            gCurrentHalf = 1;
            wakeup = true;
        }
        if (position >= gRightHalf / 2 && gLastPosition < gRightHalf / 2)
        {
            gCurrentHalf = 0;
            wakeup = true;
        }

        gLastPosition = position;

        if (wakeup)
        {
            gMixerThread.wakeupThread(false);
        }
    }
}

void OutputGC::dmaCallback(unsigned long request)
{
    FMOD_OS_Semaphore_Signal(gDMASemaphore, false);
}

void OutputGC::voiceLeftDropCallback(void * voice)
{
    gVoiceLeft = 0;
}

void OutputGC::voiceRightDropCallback(void * voice)
{
    gVoiceRight = 0;
}

FMOD_RESULT OutputGC::getNumDrivers(int * numdrivers)
{
    *numdrivers = 1;

    return FMOD_OK;
}

FMOD_RESULT OutputGC::getDriverName(int driver, char * name, int namelen)
{
    FMOD_strcpy(name, "Gamecube audio output");

    return FMOD_OK;
}

FMOD_RESULT OutputGC::init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    FMOD_RESULT result;
    unsigned int * aramsettings = (unsigned int *)extradriverdata;
    int numchannels = 24;
    void * buffer;
    unsigned int address;
    int count;

    *outputrate = 32000;
    *outputformat = FMOD_SOUND_FORMAT_PCM16;

    mMixBuffer = 0;
    mDMABuffer = 0;
    mThreadActive = false;
    gReverbA.mActive = false;
    gReverbB.mActive = false;

    if (!ARCheckInit())
    {
        ARInit(0, 0);
    }
    ARQInit();
    AIInit(0);
    AXInit();
    MIXInit();

    if (!AICheckInit())
    {
        return FMOD_ERR_OUTPUT_INIT;
    }

    if (!mARAMInitialized)
    {
        if (aramsettings)
        {
            mARAMBase = aramsettings[0] + ARGetBaseAddress();
            mARAMSize = aramsettings[1];
        }
        else
        {
            mARAMBase = ARGetBaseAddress();
            mARAMSize = 0x1000000 - ARGetBaseAddress();
        }
        mARAMInitialized = true;
    }

    result = mARAMPool.initCustom(0, mARAMSize, 0x400);
    if (result != FMOD_OK)
    {
        return result;
    }

    FMOD_OS_Semaphore_Create(&gDMASemaphore);

    /*
        Silence block, used by stopped voices.
    */
    mSilenceBlock = mARAMPool.alloc(0x100, __FILE__, __LINE__);
    mSilenceAddress = mARAMBase + ((MemBlockHeader *)mSilenceBlock)->mBlockOffset * mARAMPool.mBlockSize;

    buffer = FMOD_Memory_Calloc(0x100);
    DCFlushRange(buffer, 0x100);
    ARQPostRequest(&mARQRequest, 0, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH, (u32)buffer, mSilenceAddress, 0x100, dmaCallback);
    FMOD_OS_Semaphore_Wait(gDMASemaphore);
    FMOD_Memory_Free(buffer);

    /*
        Output ring, two halves per channel.
    */
    mRingBlock = mARAMPool.alloc(0x1800, __FILE__, __LINE__);

    buffer = FMOD_Memory_Calloc(0x1800);
    address = mARAMBase + ((MemBlockHeader *)mRingBlock)->mBlockOffset * mARAMPool.mBlockSize;
    DCFlushRange(buffer, 0x1800);
    ARQPostRequest(&mARQRequest, 0, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH, (u32)buffer, address, 0x1800, dmaCallback);
    FMOD_OS_Semaphore_Wait(gDMASemaphore);
    FMOD_Memory_Free(buffer);

    AXRegisterCallback(frameCallback);

    OSDisableInterrupts();
    if (gVoiceLeft)
    {
        MIXReleaseChannel(gVoiceLeft);
        AXFreeVoice(gVoiceLeft);
        gVoiceLeft = 0;
    }
    OSRestoreInterrupts(TRUE);

    OSDisableInterrupts();
    if (gVoiceRight)
    {
        MIXReleaseChannel(gVoiceRight);
        AXFreeVoice(gVoiceRight);
        gVoiceRight = 0;
    }
    OSRestoreInterrupts(TRUE);

    if (!gVoiceLeft)
    {
        gVoiceLeft = AXAcquireVoice(31, voiceLeftDropCallback, (u32)this);
        if (!gVoiceLeft)
        {
            return FMOD_ERR_OUTPUT_INIT;
        }
    }
    if (!gVoiceRight)
    {
        gVoiceRight = AXAcquireVoice(31, voiceRightDropCallback, (u32)this);
        if (!gVoiceRight)
        {
            return FMOD_ERR_OUTPUT_INIT;
        }
    }

    OSDisableInterrupts();
    if (gVoiceLeft)
    {
        MIXInitChannel(gVoiceLeft, 0, 0, 0, 0, 0, 127, 0);
    }
    OSRestoreInterrupts(TRUE);

    OSDisableInterrupts();
    if (gVoiceRight)
    {
        MIXInitChannel(gVoiceRight, 0, 0, 0, 0, 127, 127, 0);
    }
    OSRestoreInterrupts(TRUE);

    switch (mSystem->mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_MONO:
        {
            MIXSetSoundMode(MIX_SOUND_MODE_MONO);
            AXSetMode(AX_MODE_STEREO);
            OSSetSoundMode(OS_SOUND_MODE_MONO);
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            MIXSetSoundMode(MIX_SOUND_MODE_STEREO);
            AXSetMode(AX_MODE_STEREO);
            OSSetSoundMode(OS_SOUND_MODE_STEREO);
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            MIXSetSoundMode(MIX_SOUND_MODE_DPL2);
            AXSetMode(AX_MODE_DPL2);
            OSSetSoundMode(OS_SOUND_MODE_STEREO);
            break;
        }
        case FMOD_SPEAKERMODE_RAW:
        case FMOD_SPEAKERMODE_QUAD:
        case FMOD_SPEAKERMODE_SURROUND:
        case FMOD_SPEAKERMODE_5POINT1:
        case FMOD_SPEAKERMODE_7POINT1:
        {
            MIXSetSoundMode(MIX_SOUND_MODE_STEREO);
            AXSetMode(AX_MODE_STEREO);
            OSSetSoundMode(OS_SOUND_MODE_STEREO);
            break;
        }
    }

    if (mSystem->mMinHardwareChannels2D + mSystem->mMinHardwareChannels3D > 24)
    {
        numchannels = 0;
    }
    if (numchannels > mSystem->mMaxHardwareChannels2D + mSystem->mMaxHardwareChannels3D)
    {
        numchannels = mSystem->mMaxHardwareChannels2D + mSystem->mMaxHardwareChannels3D;
    }

    mChannelPool3D = FMOD_Object_Alloc(ChannelPool);
    if (!mChannelPool3D)
    {
        close();
        return FMOD_ERR_MEMORY;
    }

    result = mChannelPool3D->init(mSystem, this, numchannels);
    if (result != FMOD_OK)
    {
        close();
        return result;
    }

    mChannel = (ChannelGC *)FMOD_Memory_Calloc(sizeof(ChannelGC) * numchannels);
    if (!mChannel)
    {
        close();
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < numchannels; count++)
    {
        new (&mChannel[count]) ChannelGC;

        mChannelPool3D->setChannel(count, &mChannel[count], 0);
    }

    mChannelPool = gChannelPool = mChannelPool3D;

    mUploadBuffer = FMOD_Memory_Alloc(0x4000);
    if (!mUploadBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    result = FMOD_OS_CriticalSection_Create(&mUploadCrit, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::close()
{
    if (mChannelPool3D)
    {
        mChannelPool3D->release();
        mChannelPool3D = 0;
    }

    if (mChannel)
    {
        FMOD_Memory_Free(mChannel);
        mChannel = 0;
    }

    AXRegisterCallback(0);

    if (mRingBlock)
    {
        mARAMPool.free(mRingBlock, __FILE__, __LINE__);
        mRingBlock = 0;
    }

    if (mSilenceBlock)
    {
        mARAMPool.free(mSilenceBlock, __FILE__, __LINE__);
        mSilenceBlock = 0;
    }

    if (mUploadBuffer)
    {
        FMOD_Memory_Free(mUploadBuffer);
        mUploadBuffer = 0;
    }

    if (mUploadCrit)
    {
        FMOD_OS_CriticalSection_Free(mUploadCrit);
        mUploadCrit = 0;
    }

    mARAMPool.close();

    MIXQuit();
    AXQuit();

    gVoiceLeft = 0;
    gVoiceRight = 0;

    return FMOD_OK;
}

FMOD_RESULT OutputGC::getHandle(void * * handle)
{
    return FMOD_ERR_UNIMPLEMENTED;
}

FMOD_RESULT OutputGC::update()
{
    if (!gVoiceLeft)
    {
        gCurrentHalf = 0;

        gVoiceLeft = AXAcquireVoice(31, voiceLeftDropCallback, (u32)this);
        if (!gVoiceLeft)
        {
            return FMOD_ERR_OUTPUT_INIT;
        }

        OSDisableInterrupts();
        if (gVoiceLeft)
        {
            MIXInitChannel(gVoiceLeft, 0, 0, 0, 0, 0, 127, 0);
        }
        OSRestoreInterrupts(TRUE);

        start();
    }

    if (!gVoiceRight)
    {
        gVoiceRight = AXAcquireVoice(31, voiceRightDropCallback, (u32)this);
        if (!gVoiceRight)
        {
            return FMOD_ERR_OUTPUT_INIT;
        }

        OSDisableInterrupts();
        if (gVoiceRight)
        {
            MIXInitChannel(gVoiceRight, 0, 0, 0, 0, 127, 127, 0);
        }
        OSRestoreInterrupts(TRUE);

        start();
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample)
{
    FMOD_RESULT result;
    unsigned int lengthbytes;
    SampleGC * newsample;

    if (!sample)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!*sample)
    {
        newsample = FMOD_Object_Calloc(SampleGC);
        if (!newsample)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    else
    {
        newsample = (SampleGC *)*sample;
    }

    if (!waveformat)
    {
        *sample = newsample;
        return FMOD_OK;
    }

    newsample->mFormat = waveformat->format;
    newsample->mChannels = waveformat->channels;
    newsample->mLength = waveformat->lengthpcm;
    newsample->mDefaultFrequency = (float)waveformat->frequency;
    newsample->mMode = mode;
    newsample->mSystem = mSystem;
    newsample->mOutput = this;

    result = SoundI::getBytesFromSamples(waveformat->lengthpcm, &lengthbytes, newsample->mChannels, newsample->mFormat);
    if (result != FMOD_OK)
    {
        return result;
    }

    lengthbytes = (lengthbytes + 31) & ~31;

    newsample->mARAMBlock = mARAMPool.alloc(lengthbytes, __FILE__, __LINE__);
    if (!newsample->mARAMBlock)
    {
        return FMOD_ERR_MEMORY;
    }

    newsample->mARAMAddress = mARAMBase + ((MemBlockHeader *)newsample->mARAMBlock)->mBlockOffset * mARAMPool.mBlockSize;

    *sample = newsample;

    return FMOD_OK;
}

FMOD_RESULT OutputGC::reverbStop(int instance)
{
    if (instance == 0)
    {
        AXRegisterAuxACallback(0, 0);

        if (gReverbA.mActive)
        {
            gReverbA.mActive = false;

            switch (gReverbA.mType)
            {
                case REVERBGC_STD:
                {
                    AXFXReverbStdShutdown(&gReverbA.mStd);
                    break;
                }
                case REVERBGC_HI:
                {
                    AXFXReverbHiShutdown(&gReverbA.mHi);
                    break;
                }
                case REVERBGC_HIDPL2:
                {
                    AXFXReverbHiShutdownDpl2(&gReverbA.mHiDpl2);
                    break;
                }
                default:
                {
                    return FMOD_ERR_INVALID_PARAM;
                }
            }
        }
    }
    else if (instance == 1)
    {
        AXRegisterAuxBCallback(0, 0);

        if (gReverbB.mActive)
        {
            gReverbB.mActive = false;

            switch (gReverbB.mType)
            {
                case REVERBGC_STD:
                {
                    AXFXReverbStdShutdown(&gReverbB.mStd);
                    break;
                }
                case REVERBGC_HI:
                {
                    AXFXReverbHiShutdown(&gReverbB.mHi);
                    break;
                }
                case REVERBGC_HIDPL2:
                {
                    AXFXReverbHiShutdownDpl2(&gReverbB.mHiDpl2);
                    break;
                }
                default:
                {
                    return FMOD_ERR_INVALID_PARAM;
                }
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::setReverbProperties(const FMOD_REVERB_PROPERTIES * prop)
{
    FMOD_RESULT result;
    AXFX_REVERBSTD reverbstd;
    AXFX_REVERBHI reverbhi;
    float time;
    float predelay;
    float damping;
    float coloration;
    float crosstalk;
    float mix;

    if (prop->Instance > 1)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    time = 0.5f * prop->DecayTime;
    predelay = prop->ReverbDelay;
    damping = prop->ModulationDepth;
    coloration = ((float)prop->Reflections + 10000.0f) / 11000.0f;
    crosstalk = prop->EnvDiffusion;
    mix = ((float)prop->Room + 10000.0f) / 10000.0f;

    time = time > 10.0f ? 10.0f : time;
    time = time < 0.01f ? 0.01f : time;
    predelay = predelay > 0.1f ? 0.1f : predelay;
    predelay = predelay < 0.0f ? 0.0f : predelay;
    coloration = coloration > 1.0f ? 1.0f : coloration;
    coloration = coloration < 0.0f ? 0.0f : coloration;
    crosstalk = crosstalk > 1.0f ? 1.0f : crosstalk;
    crosstalk = crosstalk < 0.0f ? 0.0f : crosstalk;
    mix = mix > 1.0f ? 1.0f : mix;
    mix = mix < 0.0f ? 0.0f : mix;

    if (prop->Instance == 1)
    {
        mReverbProps1.mDecayTime = prop->DecayTime;
        mReverbProps1.mReverbDelay = prop->ReverbDelay;
        mReverbProps1.mModulationDepth = prop->ModulationDepth;
        mReverbProps1.mReflections = prop->Reflections;
        mReverbProps1.mEnvDiffusion = prop->EnvDiffusion;
        mReverbProps1.mRoom = prop->Room;
    }
    else
    {
        mReverbProps0.mDecayTime = prop->DecayTime;
        mReverbProps0.mReverbDelay = prop->ReverbDelay;
        mReverbProps0.mModulationDepth = prop->ModulationDepth;
        mReverbProps0.mReflections = prop->Reflections;
        mReverbProps0.mEnvDiffusion = prop->EnvDiffusion;
        mReverbProps0.mRoom = prop->Room;
    }

    if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_PROLOGIC)
    {
        /*
            Dolby Pro Logic II only has one reverb, on aux A.
        */
        if (prop->Instance != 0)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        result = reverbStop(0);
        if (result != FMOD_OK)
        {
            return result;
        }
        result = reverbStop(1);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (prop->Room == -10000)
        {
            return FMOD_OK;
        }

        gReverbA.mType = REVERBGC_HIDPL2;
        gReverbA.mHiDpl2.time = time;
        gReverbA.mHiDpl2.preDelay = predelay;
        gReverbA.mHiDpl2.damping = damping;
        gReverbA.mHiDpl2.coloration = coloration;
        gReverbA.mHiDpl2.mix = mix;

        if (!AXFXReverbHiInitDpl2(&gReverbA.mHiDpl2))
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        AXRegisterAuxACallback((void (*)(void *, void *))AXFXReverbHiCallbackDpl2, &gReverbA.mHiDpl2);
        gReverbA.mActive = true;
    }
    else if (prop->Flags & 0x400)
    {
        result = reverbStop(prop->Instance == 1 ? 1 : 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (prop->Room == -10000)
        {
            return FMOD_OK;
        }

        if (prop->Instance == 1)
        {
            gReverbB.mType = REVERBGC_HI;
            reverbhi.coloration = coloration;
            reverbhi.mix = mix;
            reverbhi.time = time;
            reverbhi.damping = damping;
            reverbhi.preDelay = predelay;
            reverbhi.crosstalk = crosstalk;
            gReverbB.mHi = reverbhi;

            if (!AXFXReverbHiInit(&gReverbB.mHi))
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            AXRegisterAuxBCallback((void (*)(void *, void *))AXFXReverbHiCallback, &gReverbB.mHi);
            gReverbB.mActive = true;
        }
        else
        {
            gReverbA.mType = REVERBGC_HI;
            reverbhi.coloration = coloration;
            reverbhi.mix = mix;
            reverbhi.time = time;
            reverbhi.damping = damping;
            reverbhi.preDelay = predelay;
            reverbhi.crosstalk = crosstalk;
            gReverbA.mHi = reverbhi;

            if (!AXFXReverbHiInit(&gReverbA.mHi))
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            AXRegisterAuxACallback((void (*)(void *, void *))AXFXReverbHiCallback, &gReverbA.mHi);
            gReverbA.mActive = true;
        }
    }
    else
    {
        result = reverbStop(prop->Instance == 1 ? 1 : 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (prop->Room == -10000)
        {
            return FMOD_OK;
        }

        if (prop->Instance == 1)
        {
            gReverbB.mType = REVERBGC_STD;
            reverbstd.coloration = coloration;
            reverbstd.mix = mix;
            reverbstd.time = time;
            reverbstd.damping = damping;
            reverbstd.preDelay = predelay;
            gReverbB.mStd = reverbstd;

            if (!AXFXReverbStdInit(&gReverbB.mStd))
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            AXRegisterAuxBCallback((void (*)(void *, void *))AXFXReverbStdCallback, &gReverbB.mStd);
            gReverbB.mActive = true;
        }
        else
        {
            gReverbA.mType = REVERBGC_STD;
            reverbstd.coloration = coloration;
            reverbstd.mix = mix;
            reverbstd.time = time;
            reverbstd.damping = damping;
            reverbstd.preDelay = predelay;
            gReverbA.mStd = reverbstd;

            if (!AXFXReverbStdInit(&gReverbA.mStd))
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            AXRegisterAuxACallback((void (*)(void *, void *))AXFXReverbStdCallback, &gReverbA.mStd);
            gReverbA.mActive = true;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::getSoundRAM(int * currentalloced, int * maxalloced, int * total)
{
    if (currentalloced)
    {
        *currentalloced = mARAMPool.getCurrentAllocated();
    }
    if (maxalloced)
    {
        *maxalloced = mARAMPool.getMaxAllocated();
    }
    if (total)
    {
        *total = mARAMPool.mSizeBytes;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::start()
{
    FMOD_RESULT result;
    int outputrate;
    FMOD_SOUND_FORMAT outputformat;
    int outputchannels;

    result = mSystem->getSoftwareFormat(&outputrate, &outputformat, &outputchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mMixBuffer)
    {
        mMixBuffer = FMOD_Memory_Calloc(0x1800);
        if (!mMixBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    if (!mDMABuffer)
    {
        mDMABuffer = FMOD_Memory_Calloc(0x1800);
        if (!mDMABuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    OSDisableInterrupts();
    if (gVoiceLeft)
    {
        AXSetVoiceState(gVoiceLeft, AX_PB_STATE_STOP);
    }
    OSRestoreInterrupts(TRUE);

    OSDisableInterrupts();
    if (gVoiceRight)
    {
        AXSetVoiceState(gVoiceRight, AX_PB_STATE_STOP);
    }
    OSRestoreInterrupts(TRUE);

    gBlockSize = 0x600;
    gLeftStart = mARAMBase + ((MemBlockHeader *)mRingBlock)->mBlockOffset * mARAMPool.mBlockSize;
    gLeftBuffer = gLeftStart;
    gLeftHalf = gLeftStart + gBlockSize;
    gRightBuffer = gLeftStart + gBlockSize * 2;
    gRightHalf = gRightBuffer + gBlockSize;

    if (gVoiceLeft)
    {
        AXPBADDR addr;
        unsigned int loopaddress = gLeftStart / 2;
        unsigned int endaddress = (gLeftStart + gBlockSize * 2) / 2 - 1;

        addr.loopFlag = AXPBADDR_LOOP_ON;
        addr.format = AX_PB_FORMAT_PCM16;
        addr.loopAddressHi = (u16)(loopaddress >> 16);
        addr.loopAddressLo = (u16)loopaddress;
        addr.endAddressHi = (u16)(endaddress >> 16);
        addr.endAddressLo = (u16)endaddress;
        addr.currentAddressHi = (u16)(loopaddress >> 16);
        addr.currentAddressLo = (u16)loopaddress;

        OSDisableInterrupts();
        if (gVoiceLeft)
        {
            AXSetVoiceAddr(gVoiceLeft, &addr);
            AXSetVoiceSrcType(gVoiceLeft, AX_SRC_TYPE_NONE);
        }
        OSRestoreInterrupts(TRUE);
    }

    if (gVoiceRight)
    {
        AXPBADDR addr;
        unsigned int loopaddress = gRightBuffer / 2;
        unsigned int endaddress = (gRightBuffer + gBlockSize * 2) / 2 - 1;

        addr.loopFlag = AXPBADDR_LOOP_ON;
        addr.format = AX_PB_FORMAT_PCM16;
        addr.loopAddressHi = (u16)(loopaddress >> 16);
        addr.loopAddressLo = (u16)loopaddress;
        addr.endAddressHi = (u16)(endaddress >> 16);
        addr.endAddressLo = (u16)endaddress;
        addr.currentAddressHi = (u16)(loopaddress >> 16);
        addr.currentAddressLo = (u16)loopaddress;

        OSDisableInterrupts();
        if (gVoiceRight)
        {
            AXSetVoiceAddr(gVoiceRight, &addr);
            AXSetVoiceSrcType(gVoiceRight, AX_SRC_TYPE_NONE);
        }
        OSRestoreInterrupts(TRUE);
    }

    gLastPosition = gRightBuffer;

    if (!mThreadActive)
    {
        gMixerThread.initThread("Gamecube Mixer Thread", mixThreadCallback, this, Thread::PRIORITY_CRITICAL, 0, 0x4000, true, 0);
        gMixerThread.wakeupThread(false);
        mThreadActive = true;
    }

    OSDisableInterrupts();
    if (gVoiceLeft)
    {
        AXSetVoiceState(gVoiceLeft, AX_PB_STATE_RUN);
    }
    OSRestoreInterrupts(TRUE);

    OSDisableInterrupts();
    if (gVoiceRight)
    {
        AXSetVoiceState(gVoiceRight, AX_PB_STATE_RUN);
    }
    OSRestoreInterrupts(TRUE);

    return FMOD_OK;
}

FMOD_RESULT OutputGC::stop()
{
    OSDisableInterrupts();
    if (gVoiceLeft)
    {
        AXSetVoiceState(gVoiceLeft, AX_PB_STATE_STOP);
    }
    OSRestoreInterrupts(TRUE);

    OSDisableInterrupts();
    if (gVoiceRight)
    {
        AXSetVoiceState(gVoiceRight, AX_PB_STATE_STOP);
    }
    OSRestoreInterrupts(TRUE);

    if (mMixBuffer)
    {
        FMOD_Memory_Free(mMixBuffer);
        mMixBuffer = 0;
    }

    if (mDMABuffer)
    {
        FMOD_Memory_Free(mDMABuffer);
        mMixBuffer = 0; // sic: the native clears +0xD4 again, leaving mDMABuffer dangling
    }

    gMixerThread.closeThread();
    mThreadActive = false;

    return FMOD_OK;
}

FMOD_RESULT OutputGC::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    char * buffer = (char *)mMixBuffer;
    unsigned int bufferlength = length * 4;

    if (!buffer)
    {
        *ptr1 = 0;
        *ptr2 = 0;
        *len1 = 0;
        *len2 = 0;
        return FMOD_ERR_INVALID_PARAM;
    }

    if (length > bufferlength)
    {
        length = bufferlength;
    }

    if (offset >= bufferlength)
    {
        *ptr1 = 0;
        *ptr2 = 0;
        *len1 = 0;
        *len2 = 0;
        return FMOD_ERR_INVALID_PARAM;
    }

    if (offset + length <= bufferlength)
    {
        *ptr1 = buffer + offset;
        *len1 = length;
        *ptr2 = 0;
        *len2 = 0;
    }
    else
    {
        *ptr1 = buffer + offset;
        *len1 = bufferlength - offset;
        *ptr2 = buffer;
        *len2 = length - (bufferlength - offset);
    }

    return FMOD_OK;
}

FMOD_RESULT OutputGC::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    unsigned short * src = (unsigned short *)ptr1;
    unsigned short * left = (unsigned short *)((char *)mDMABuffer + gCurrentHalf * (gBlockSize * 2));
    unsigned short * right = (unsigned short *)((char *)left + gBlockSize);
    unsigned short * destleft = left;
    unsigned short * destright = right;
    unsigned int count;

    /*
        Deinterleave the stereo mix into the left and right halves.
    */
    for (count = 0; count < gBlockSize / 2; count++)
    {
        *destleft = src[0];
        *destright = src[1];
        src += 2;
        destleft++;
        destright++;
    }

    DCFlushRange(left, gBlockSize);
    DCFlushRange(right, gBlockSize);

    ARQPostRequest(&gDMARequestLeft, 0, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH, (u32)left, gLeftBuffer + gCurrentHalf * gBlockSize, gBlockSize, 0);
    ARQPostRequest(&gDMARequestRight, 0, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH, (u32)right, gRightBuffer + gCurrentHalf * gBlockSize, gBlockSize, 0);

    return FMOD_OK;
}

FMOD_RESULT OutputGC::getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->getNumDrivers(numdrivers);
}

FMOD_RESULT OutputGC::getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->getDriverName(id, name, namelen);
}

FMOD_RESULT OutputGC::initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->init(selecteddriver, flags, outputrate, outputchannels, outputformat, dspbufferlength, dspnumbuffers, extradriverdata);
}

FMOD_RESULT OutputGC::closeCallback(FMOD_OUTPUT_STATE * output)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->close();
}

FMOD_RESULT OutputGC::startCallback(FMOD_OUTPUT_STATE * output)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->start();
}

FMOD_RESULT OutputGC::stopCallback(FMOD_OUTPUT_STATE * output)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->stop();
}

FMOD_RESULT OutputGC::updateCallback(FMOD_OUTPUT_STATE * output)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->update();
}

FMOD_RESULT OutputGC::createSampleCallback(FMOD_OUTPUT_STATE * output, FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->createSample(mode, waveformat, sample);
}

FMOD_RESULT OutputGC::getHandleCallback(FMOD_OUTPUT_STATE * output, void * * handle)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->getHandle(handle);
}

FMOD_RESULT OutputGC::setReverbPropertiesCallback(FMOD_OUTPUT_STATE * output, const FMOD_REVERB_PROPERTIES * prop)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->setReverbProperties(prop);
}

FMOD_RESULT OutputGC::getSoundRAMCallback(FMOD_OUTPUT_STATE * output, int * currentalloced, int * maxalloced, int * total)
{
    OutputGC * gc = (OutputGC *)output;

    return gc->getSoundRAM(currentalloced, maxalloced, total);
}

SampleGC::SampleGC()
{
    mARAMBlock = 0;
    mChannel = 0;
    mLockOffset = 0;
    mLockLength = 0;
}

FMOD_RESULT SampleGC::release()
{
    FMOD_RESULT result;

    if (!mSystem)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    result = mSystem->stopSound(this);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    if (mARAMBlock)
    {
        mOutput->mARAMPool.free(mARAMBlock, "", 0);
        mARAMBlock = 0;
    }

    return Sample::release();
}

FMOD_RESULT SampleGC::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    mLockOffset = offset;
    mLockLength = length;

    if (mOutput->mUploadBusy)
    {
        return FMOD_ERR_ALREADYLOCKED;
    }

    FMOD_OS_CriticalSection_Enter(mOutput->mUploadCrit);
    mOutput->mUploadBusy = true;

    *ptr1 = mOutput->mUploadBuffer;
    *len1 = length;
    *ptr2 = 0;
    *len2 = 0;

    return FMOD_OK;
}

FMOD_RESULT SampleGC::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
    void * buffer;

    if (!mARAMBlock)
    {
        return FMOD_ERR_MEMORY;
    }

    buffer = mOutput->mUploadBuffer;
    if (!buffer)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    /*
        A playing GCADPCM voice needs the loop predictor of the new data.
    */
    if (mChannel && !mLockOffset && mFormat == FMOD_SOUND_FORMAT_GCADPCM)
    {
        AXPBADPCMLOOP adpcmloop;

        adpcmloop.loop_pred_scale = *(unsigned char *)buffer;
        adpcmloop.loop_yn1 = 0;
        adpcmloop.loop_yn2 = 0;

        AXSetVoiceAdpcmLoop(mChannel->mVoice, &adpcmloop);
    }

    mLockLength = (mLockLength + 31) & ~31;

    DCFlushRange(buffer, mLockLength);
    ARQPostRequest(&mOutput->mARQRequest, 0, ARQ_TYPE_MRAM_TO_ARAM, ARQ_PRIORITY_HIGH, (u32)buffer, mARAMAddress + ((mLockOffset + 31) & ~31), mLockLength, OutputGC::dmaCallback);
    FMOD_OS_Semaphore_Wait(gDMASemaphore);

    FMOD_OS_CriticalSection_Leave(mOutput->mUploadCrit);
    mOutput->mUploadBusy = false;

    return FMOD_OK;
}

} // namespace FMOD
