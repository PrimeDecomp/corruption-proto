// G2MEAB .text 0x8061A6E0..0x80621208 (48 native functions), reconstructed from FMOD Ex 4.06.00 (PS3)
// debug information and the G2MEAB code. Functions follow the native address order, which is the 4.06
// source-line order. Implemented: FMOD_Cart2Angle, getInstance, validate, streamThread, updateStreams,
// sortSpeakerList, allocDSPCodec, the constructor, release, setOutput, set/getHardwareChannels,
// getDSPBufferSize, setSpeakerMode, close, setSpeakerPosition, set/get3DSettings, set/get3DNumListeners,
// getChannelsPlaying, getCPUUsage, both createDSP, createDSPByType, createChannelGroup, playSound,
// getChannel, getMasterChannelGroup, lock/unlockDSP, recordStop, stopDSP, getListenerObject and the
// ChannelStreamPool add/allocate emitted here. Still empty placeholders: updateChannels (0x8061A9AC),
// findChannel(SoundI *) (0x8061B600), createSample (0x8061BD2C), createSoundInternal (0x8061C1C0), setUpPlugins (0x8061E0A8),
// init (0x8061EE30), closeEx (0x8061F688), update (0x8061FA80), set3DListenerAttributes (0x8061FD98),
// createSound (0x806201A0) and stopSound (0x80620DA8). The 4.06 functions without G2MEAB code follow
// at the end as placeholders (the unused static FMOD_CHECKFLOAT helper is not kept).

#include "fmod_systemi.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_3d.h"
#include "fmod_channel_stream.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_channelpool.h"
#include "fmod_codec.h"
#include "fmod_dsp.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_geometryi.h"
#include "fmod_globals.h"
#include "fmod_linkedlist.h"
#include "fmod_listener.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_os_output.h"
#include "fmod_outputi.h"
#include "fmod_pluginfactory.h"
#include "fmod_reverbi.h"
#include "fmod_sound_sample.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_thread.h"
#include "fmod_time.h"

#include <stdlib.h>
#include <string.h>

namespace FMOD {

// __sinit 0x80621194 constructs gStreamHead (bss 0x80755EF4), gStreamThread (0x80755F08) and
// gStreamTimeStamp (0x8075602C) in this order.
LinkedListNode SystemI::gStreamHead;
Thread SystemI::gStreamThread;
TimeStamp SystemI::gStreamTimeStamp;
FMOD_OS_CRITICALSECTION * SystemI::gSoundListCrit;
FMOD_OS_CRITICALSECTION * SystemI::gStreamListCrit;
FMOD_OS_CRITICALSECTION * SystemI::gStreamFillCrit;
FMOD_OS_CRITICALSECTION * SystemI::gStreamCrit;
bool SystemI::gStreamThreadActive;

FMOD_RESULT SystemI::getInstance(unsigned int id, SystemI * * sys)
{
    SystemI * current;

    if (sys)
    {
        *sys = 0;
    }

    for (current = (SystemI *)gSystemHead->getNext(); current != gSystemHead; current = (SystemI *)current->getNext())
    {
        if (current->mIndex == id)
        {
            if (sys)
            {
                *sys = current;
            }
            return FMOD_OK;
        }
    }

    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT SystemI::validate(System * system, SystemI * * systemi)
{
    SystemI * sys = (SystemI *)system;

    if (!system)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!systemi)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!gSystemHead->exists(sys))
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *systemi = sys;

    return FMOD_OK;
}

void SystemI::streamThread(void * data)
{
    SystemI * current;

    gStreamTimeStamp.stampIn();

    current = (SystemI *)gSystemHead->getNext();
    while (current != gSystemHead)
    {
        SystemI * next = (SystemI *)current->getNext();

        current->updateStreams();

        current = next;
    }

    gStreamTimeStamp.stampOut(95);
}

FMOD_RESULT SystemI::updateStreams()
{
    LinkedListNode * current;

    FMOD_OS_CriticalSection_Enter(gStreamListCrit);

    current = gStreamHead.getNext();
    while (current != &gStreamHead)
    {
        LinkedListNode * next = current->getNext();
        ChannelStream * channelstream = (ChannelStream *)current->getData();

        if (!channelstream->mFinished && !channelstream->mBusy)
        {
            channelstream->updateStream();
        }

        current = next;
    }

    FMOD_OS_CriticalSection_Leave(gStreamListCrit);

    return FMOD_OK;
}

FMOD_RESULT SystemI::updateChannels(int delta)
{
}

FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, SoundI * sound, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample_out)
{
}

FMOD_RESULT SystemI::createSoundInternal(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
{
}

FMOD_RESULT SystemI::setUpPlugins()
{
}

FMOD_RESULT SystemI::sortSpeakerList()
{
    int count;
    int count2;
    bool used[8];
    int outputchannels;

    for (count = 0; count < 8; count++)
    {
        mSpeakerList[count] = 0;
    }

    memset(used, 0, 8);

    outputchannels = mMaxOutputChannels;
    if (mSpeakerMode == FMOD_SPEAKERMODE_QUAD || mSpeakerMode == FMOD_SPEAKERMODE_SURROUND)
    {
        outputchannels = 6;
    }

    for (count = 0; count < outputchannels; count++)
    {
        int lowest = 361;

        for (count2 = 0; count2 < outputchannels; count2++)
        {
            if (mSpeaker[count2].mSpeaker == FMOD_SPEAKER_LOW_FREQUENCY)
            {
                continue;
            }
            if (mSpeakerMode == FMOD_SPEAKERMODE_QUAD && mSpeaker[count2].mSpeaker == FMOD_SPEAKER_FRONT_CENTER)
            {
                continue;
            }

            if (mSpeaker[count2].mXZAngle < lowest && !used[count2])
            {
                lowest = mSpeaker[count2].mXZAngle;
                mSpeakerList[count] = &mSpeaker[count2];
            }
        }

        if (mSpeakerList[count])
        {
            used[mSpeakerList[count]->mSpeaker] = true;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::allocDSPCodec(FMOD_SOUND_FORMAT format, DSPI * * dsp)
{
    if (format == FMOD_SOUND_FORMAT_MPEG)
    {
        return mDSPCodecPool_MPEG.alloc(dsp);
    }
    else if (format == FMOD_SOUND_FORMAT_IMAADPCM)
    {
        return mDSPCodecPool_ADPCM.alloc(dsp);
    }

    return FMOD_ERR_FORMAT;
}

SystemI::SystemI()
{
    int count;

    mInitialized = false;
    mPluginsLoaded = false;
    mOutputType = FMOD_OUTPUTTYPE_AUTODETECT;
    mOutput = 0;
    mSoftware = 0;
    mEmulated = 0;
    mMainThreadID = 0;
    mChannel = 0;
    mPluginFactory = 0;
    mLastTimeStamp = 0;
    mStreamFileBufferSize = 16 * 1024;
    mStreamFileBufferSizeType = FMOD_TIMEUNIT_RAWBYTES;
    mUserData = 0;
    mNumSoftwareChannels = 64;
    mMinHardwareChannels2D = 0;
    mMaxHardwareChannels2D = 1000;
    mMinHardwareChannels3D = 0;
    mMaxHardwareChannels3D = 1000;
    mDSPBlockSize = 1024;
    mDSPBufferSize = mDSPBlockSize * 4;
    mDSPReadBuff[0] = 0;
    mDSPReadBuff[1] = 0;
    mDSPReadBuffIndex = 0;

    for (count = 0; count < 128; count++)
    {
        mDSPMixBuff[count] = 0;
    }

    mMaxInputChannels = 8;
    mMaxOutputChannels = 0;

    setSpeakerPosition(FMOD_SPEAKER_FRONT_LEFT, -1.0f, 1.0f);
    setSpeakerPosition(FMOD_SPEAKER_FRONT_RIGHT, 1.0f, 1.0f);
    setSpeakerPosition(FMOD_SPEAKER_FRONT_CENTER, 0.0f, 1.0f);
    setSpeakerPosition(FMOD_SPEAKER_LOW_FREQUENCY, 0.0f, 0.0f);
    setSpeakerPosition(FMOD_SPEAKER_BACK_LEFT, -1.0f, -1.0f);
    setSpeakerPosition(FMOD_SPEAKER_BACK_RIGHT, 1.0f, -1.0f);
    setSpeakerPosition(FMOD_SPEAKER_SIDE_LEFT, -1.0f, 0.0f);
    setSpeakerPosition(FMOD_SPEAKER_SIDE_RIGHT, 1.0f, 0.0f);

    mOutputFormat = FMOD_SOUND_FORMAT_PCM16;
    setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
    mOutputRate = 48000;
    mOutputIndex = 0;
    mSelectedDriver = -1;
    mSelectedRecordDriver = -1;
    mResampleMethod = FMOD_DSP_RESAMPLER_LINEAR;
    mNumListeners = 1;
    mDistanceScale = 1.0f;
    mRolloffScale = 1.0f;
    mDopplerScale = 1.0f;
    mUnkB58 = 1;
    mGeometryList = 0;
    mGeometryMgr.mSystem = this;

    memset(mPluginPath, 0, 256);

    {
        FMOD_REVERB_PROPERTIES prop = FMOD_PRESET_OFF;

        memcpy(&mReverbProperties, &prop, sizeof(FMOD_REVERB_PROPERTIES));
    }

    mAdvancedSettings.maxXMAcodecs = FMOD_ADVANCEDSETTINGS_MAXXMACODECS;
    mAdvancedSettings.maxADPCMcodecs = FMOD_ADVANCEDSETTINGS_MAXADPCMCODECS;
    mAdvancedSettings.maxMPEGcodecs = FMOD_ADVANCEDSETTINGS_MAXMPEGCODECS;

    mDSPCodecPool_MPEG.mSystem = this;
    mDSPCodecPool_ADPCM.mSystem = this;
}

FMOD_RESULT SystemI::release()
{
    FMOD_RESULT result;

    result = close();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mPluginFactory)
    {
        mPluginFactory->release();
        mPluginFactory = 0;
    }

    removeNode();

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT SystemI::setOutput(FMOD_OUTPUTTYPE outputtype)
{
    FMOD_RESULT result;
    int numoutputs;
    int count;

    if (mInitialized)
    {
        return FMOD_ERR_INITIALIZED;
    }

    if (mOutput)
    {
        if (outputtype == mOutputType)
        {
            return FMOD_OK;
        }

        FMOD_Memory_Free(mOutput);
    }

    if (!mPluginsLoaded)
    {
        result = setUpPlugins();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = mPluginFactory->getNumOutputs(&numoutputs);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (outputtype == FMOD_OUTPUTTYPE_AUTODETECT)
    {
        FMOD_OS_Output_GetDefault(&outputtype);
    }

    for (count = 0; count < numoutputs; count++)
    {
        FMOD_OUTPUT_DESCRIPTION_EX * outputdesc = 0;

        result = mPluginFactory->getOutput(count, &outputdesc);
        if (result != FMOD_OK)
        {
            continue;
        }

        if (outputdesc->mType == outputtype)
        {
            result = mPluginFactory->createOutput(outputdesc, &mOutput);
            if (result != FMOD_OK)
            {
                return result;
            }

            mOutputType = mOutput->mDescription.mType;
            mOutputIndex = count;

            return FMOD_OK;
        }
    }

    return FMOD_ERR_PLUGIN_MISSING;
}

FMOD_RESULT SystemI::setHardwareChannels(int min2d, int max2d, int min3d, int max3d)
{
    if (mInitialized)
    {
        return FMOD_ERR_INITIALIZED;
    }

    if (min2d < 0 || max2d < 0 || min3d < 0 || max3d < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mMinHardwareChannels2D = min2d;
    mMaxHardwareChannels2D = max2d;
    mMinHardwareChannels3D = min3d;
    mMaxHardwareChannels3D = max3d;

    return FMOD_OK;
}

FMOD_RESULT SystemI::getHardwareChannels(int * num2d, int * num3d, int * total)
{
    FMOD_RESULT result;
    int numhw2d = 0;
    int numhw3d = 0;

    if (mOutput)
    {
        if (mOutput->mChannelPool)
        {
            result = mOutput->mChannelPool->getNumChannels(&numhw2d);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        if (mOutput->mChannelPool3D)
        {
            result = mOutput->mChannelPool3D->getNumChannels(&numhw3d);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (num3d)
    {
        *num3d = numhw3d;
    }
    if (num2d)
    {
        *num2d = numhw2d;
    }
    if (total)
    {
        *total = numhw3d + numhw2d;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::getDSPBufferSize(unsigned int * bufferlength, int * numbuffers)
{
    if (bufferlength)
    {
        *bufferlength = mDSPBlockSize;
    }
    if (numbuffers)
    {
        *numbuffers = mDSPBufferSize / mDSPBlockSize;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::setSpeakerMode(FMOD_SPEAKERMODE speakermode)
{
    if (mInitialized)
    {
        return FMOD_ERR_INITIALIZED;
    }

    mSpeakerMode = speakermode;

    switch (mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_MONO:
        {
            mMaxOutputChannels = 1;
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            mMaxOutputChannels = 2;
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        {
            mMaxOutputChannels = 4;
            break;
        }
        case FMOD_SPEAKERMODE_SURROUND:
        {
            mMaxOutputChannels = 4;
            break;
        }
        case FMOD_SPEAKERMODE_5POINT1:
        {
            mMaxOutputChannels = 6;
            break;
        }
        case FMOD_SPEAKERMODE_7POINT1:
        {
            mMaxOutputChannels = 8;
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            mMaxOutputChannels = 2;
            break;
        }
    }

    return sortSpeakerList();
}

FMOD_RESULT SystemI::init(int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata)
{
}

FMOD_RESULT SystemI::close()
{
    return closeEx(false);
}

FMOD_RESULT SystemI::closeEx(bool calledfrominit)
{
}

FMOD_RESULT SystemI::update()
{
}

FMOD_RESULT SystemI::setSpeakerPosition(FMOD_SPEAKER speaker, float x, float y)
{
    mSpeaker[speaker].mSpeaker = speaker;
    mSpeaker[speaker].mPosition.x = x;
    mSpeaker[speaker].mPosition.y = 0.0f;
    mSpeaker[speaker].mPosition.z = y;
    mSpeaker[speaker].mXZAngle = FMOD_Cart2Angle((int)(x * 256.0f), (int)(y * 256.0f));

    return sortSpeakerList();
}

FMOD_RESULT SystemI::set3DSettings(float dopplerscale, float distancescale, float rolloffscale)
{
    if (dopplerscale < 0.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (distancescale <= 0.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (rolloffscale < 0.0f)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mDopplerScale = dopplerscale;
    mDistanceScale = distancescale;
    mRolloffScale = rolloffscale;

    return FMOD_OK;
}

FMOD_RESULT SystemI::get3DSettings(float * dopplerscale, float * distancescale, float * rolloffscale)
{
    if (dopplerscale)
    {
        *dopplerscale = mDopplerScale;
    }
    if (distancescale)
    {
        *distancescale = mDistanceScale;
    }
    if (rolloffscale)
    {
        *rolloffscale = mRolloffScale;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::set3DNumListeners(int numlisteners)
{
    if (numlisteners < 1 || numlisteners > LISTENER_MAX)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mNumListeners = numlisteners;

    return FMOD_OK;
}

FMOD_RESULT SystemI::get3DNumListeners(int * numlisteners)
{
    if (!numlisteners)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numlisteners = mNumListeners;

    return FMOD_OK;
}

FMOD_RESULT SystemI::set3DListenerAttributes(int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
{
}

FMOD_RESULT SystemI::getChannelsPlaying(int * channels)
{
    LinkedListNode * current;
    int count;

    if (!channels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    count = 0;
    for (current = mChannelUsedListHead.getNext(); current != &mChannelUsedListHead; current = current->getNext())
    {
        count++;
    }

    *channels = count;

    return FMOD_OK;
}

FMOD_RESULT SystemI::getCPUUsage(float * dsp, float * stream, float * update, float * total)
{
    float value;
    float totalvalue = 0.0f;

    if (mDSPTimeStamp.getCPUUsage(&value) == FMOD_OK)
    {
        totalvalue += value;
        if (dsp)
        {
            *dsp = value;
        }
    }
    if (gStreamTimeStamp.getCPUUsage(&value) == FMOD_OK)
    {
        totalvalue += value;
        if (stream)
        {
            *stream = value;
        }
    }
    if (mUpdateTimeStamp.getCPUUsage(&value) == FMOD_OK)
    {
        totalvalue += value;
        if (update)
        {
            *update = value;
        }
    }

    if (total)
    {
        *total = totalvalue;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::createSound(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
{
}

FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION * description, DSPI * * dsp)
{
    FMOD_RESULT result;

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dsp = 0;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & FMOD_INIT_SOFTWARE_DISABLE)
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

    {
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_strcpy(descriptionex.name, description->name);
        descriptionex.version = description->version;
        descriptionex.channels = description->channels;
        descriptionex.create = description->create;
        descriptionex.release = description->release;
        descriptionex.reset = description->reset;
        descriptionex.read = description->read;
        descriptionex.setposition = description->setposition;
        descriptionex.numparameters = description->numparameters;
        descriptionex.paramdesc = description->paramdesc;
        descriptionex.setparameter = description->setparameter;
        descriptionex.getparameter = description->getparameter;
        descriptionex.config = description->config;
        descriptionex.configwidth = description->configwidth;
        descriptionex.configheight = description->configheight;
        descriptionex.userdata = description->userdata;
        descriptionex.mSize = 0;
        descriptionex.mCategory = FMOD_DSP_CATEGORY_FILTER;
        descriptionex.mFormat = FMOD_SOUND_FORMAT_PCMFLOAT;
        descriptionex.mModule = 0;
        descriptionex.mAEffect = 0;
        descriptionex.mResamplerBlockLength = 0;

        result = mPluginFactory->createDSP(&descriptionex, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        (*dsp)->mSystem = this;

        return FMOD_OK;
    }
}

FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION_EX * description, DSPI * * dsp)
{
    FMOD_RESULT result;

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dsp = 0;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & FMOD_INIT_SOFTWARE_DISABLE)
    {
        return FMOD_ERR_NEEDSSOFTWARE;
    }

    {
        FMOD_DSP_DESCRIPTION_EX descriptionex;

        FMOD_strcpy(descriptionex.name, description->name);
        descriptionex.version = description->version;
        descriptionex.channels = description->channels;
        descriptionex.create = description->create;
        descriptionex.release = description->release;
        descriptionex.reset = description->reset;
        descriptionex.read = description->read;
        descriptionex.setposition = description->setposition;
        descriptionex.numparameters = description->numparameters;
        descriptionex.paramdesc = description->paramdesc;
        descriptionex.setparameter = description->setparameter;
        descriptionex.getparameter = description->getparameter;
        descriptionex.config = description->config;
        descriptionex.configwidth = description->configwidth;
        descriptionex.configheight = description->configheight;
        descriptionex.userdata = description->userdata;
        descriptionex.mSize = 0;
        descriptionex.mCategory = description->mCategory;
        descriptionex.mFormat = description->mFormat;
        descriptionex.mModule = 0;
        descriptionex.mAEffect = description->mAEffect;
        descriptionex.mResamplerBlockLength = description->mResamplerBlockLength;

        result = mPluginFactory->createDSP(&descriptionex, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        (*dsp)->mSystem = this;

        return FMOD_OK;
    }
}

FMOD_RESULT SystemI::createDSPByType(FMOD_DSP_TYPE type, DSPI * * dsp)
{
    FMOD_RESULT result;
    int numdsps;
    int count;

    if (!mPluginFactory)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (type == FMOD_DSP_TYPE_MIXER)
    {
        FMOD_DSP_DESCRIPTION description;

        memset(&description, 0, sizeof(FMOD_DSP_DESCRIPTION));
        FMOD_strcpy(description.name, "FMOD Mixer unit");

        result = createDSP(&description, dsp);
        if (result != FMOD_OK)
        {
            return result;
        }

        return FMOD_OK;
    }

    result = mPluginFactory->getNumDSPs(&numdsps);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < numdsps; count++)
    {
        FMOD_DSP_DESCRIPTION_EX * descriptionex = 0;

        result = mPluginFactory->getDSP(count, &descriptionex);
        if (result != FMOD_OK)
        {
            continue;
        }

        if (descriptionex->mType == type)
        {
            result = mPluginFactory->createDSP(descriptionex, dsp);
            if (result != FMOD_OK)
            {
                return result;
            }

            return FMOD_OK;
        }
    }

    return FMOD_ERR_PLUGIN_MISSING;
}

FMOD_RESULT SystemI::createChannelGroup(const char * name, ChannelGroupI * * channelgroup)
{
    FMOD_RESULT result;
    ChannelGroupI * newgroup;

    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newgroup = FMOD_Object_Calloc(ChannelGroupI);
    if (!newgroup)
    {
        return FMOD_ERR_MEMORY;
    }

    newgroup->addAfter(&mChannelGroupHead);
    newgroup->mSystem = this;

    if (name)
    {
        FMOD_strncpy(newgroup->mName, name, 256);
    }
    else
    {
        FMOD_strcpy(newgroup->mName, "");
    }

    if (mSoftware)
    {
        FMOD_DSP_DESCRIPTION description;

        FMOD_strcpy(description.name, "ChannelGroup");
        if (name)
        {
            FMOD_strcat(description.name, ":");
            FMOD_strncat(description.name, name, 18);
        }

        description.version = 0x00010100;
        description.channels = 0;
        description.create = 0;
        description.release = 0;
        description.read = 0;
        description.setposition = 0;

        result = createDSP(&description, &newgroup->mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }

        newgroup->mDSPHead->setDefaults((float)mOutputRate, -1.0f, -1.0f, -1);
        newgroup->mDSPHead->mActive = true;

        result = mDSPChannelGroupTarget->addInput(newgroup->mDSPHead);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (name && !FMOD_strcmp("music", name))
    {
        mOutput->mMusicChannelGroup = newgroup;
    }

    *channelgroup = newgroup;

    return FMOD_OK;
}

FMOD_RESULT SystemI::playSound(FMOD_CHANNELINDEX channelid, SoundI * sound, bool paused, Channel * * channel)
{
    FMOD_RESULT result;
    ChannelI * chan = 0;

    if (channel && channelid == FMOD_CHANNEL_REUSE)
    {
        ChannelI::validate(*channel, &chan);
    }

    if (!sound)
    {
        *channel = 0;
        return FMOD_ERR_INVALID_PARAM;
    }

    if (sound->mOpenState != FMOD_OPENSTATE_READY)
    {
        *channel = 0;
        return FMOD_ERR_NOTREADY;
    }

    if (sound->mType == FMOD_SOUND_TYPE_PLAYLIST)
    {
        return FMOD_ERR_FORMAT;
    }

    result = findChannel(channelid, sound, &chan);
    if (result != FMOD_OK)
    {
        *channel = 0;
        return result;
    }

    result = chan->play(sound, paused, true);
    if (result != FMOD_OK)
    {
        *channel = 0;
        chan->stopEx(false, true, true, true, false);
        return result;
    }

    result = chan->updatePosition();
    if (result != FMOD_OK)
    {
        *channel = 0;
        return result;
    }

    if (channelid == FMOD_CHANNEL_REUSE && *channel)
    {
        chan->mHandleCurrent = chan->mHandleOriginal;
    }
    else
    {
        result = chan->referenceStamp(true);
        if (result != FMOD_OK)
        {
            *channel = 0;
            return result;
        }
    }

    if (channel)
    {
        *channel = (Channel *)chan->mHandleCurrent;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::getChannel(int id, Channel * * channel)
{
    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (id < 0 || id >= mNumChannels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channel = (Channel *)((mIndex << 28) | ((id << 16) & 0x0FFF0000));

    return FMOD_OK;
}

FMOD_RESULT SystemI::getMasterChannelGroup(ChannelGroupI * * channelgroup)
{
    if (!channelgroup)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channelgroup = mChannelGroup;

    return FMOD_OK;
}

FMOD_RESULT SystemI::lockDSP()
{
    FMOD_OS_CriticalSection_Enter(mDSPCrit);

    return FMOD_OK;
}

FMOD_RESULT SystemI::unlockDSP()
{
    FMOD_OS_CriticalSection_Leave(mDSPCrit);

    return FMOD_OK;
}

FMOD_RESULT SystemI::recordStop()
{
    return FMOD_ERR_UNSUPPORTED;
}

FMOD_RESULT SystemI::stopSound(SoundI * sound)
{
}

FMOD_RESULT SystemI::stopDSP(DSPI * dsp)
{
    ChannelI * current;

    current = (ChannelI *)mChannelUsedListHead.getNext();
    while (current != &mChannelUsedListHead)
    {
        ChannelI * next = (ChannelI *)current->getNext();
        DSPI * currentdsp;

        current->getCurrentDSP(&currentdsp);
        if (currentdsp == dsp)
        {
            current->stop();
        }

        current = next;
    }

    return FMOD_OK;
}

FMOD_RESULT SystemI::getListenerObject(int listener, Listener * * listenerobject)
{
    if (!listenerobject || listener < 0 || listener >= mNumListeners)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *listenerobject = &mListener[listener];

    return FMOD_OK;
}

// ChannelStreamPool (declared in fmod_channel_stream.h) is emitted here natively; its file string is
// "fmod_freelist.h" line 0x34, so it was probably an inline/template from a header G2MEAB does not keep.
FMOD_RESULT ChannelStreamPool::add()
{
    ChannelStream * channelstream;

    channelstream = FMOD_Object_Calloc(ChannelStream);
    if (!channelstream)
    {
        return FMOD_ERR_MEMORY;
    }

    channelstream->addAfter(&mFreeHead);

    return FMOD_OK;
}

FMOD_RESULT ChannelStreamPool::allocate(ChannelStream * * channel)
{
    if (!channel)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFreeHead.isEmpty())
    {
        return FMOD_ERR_MEMORY;
    }

    *channel = (ChannelStream *)mFreeHead.getNext();

    (*channel)->removeNode();
    (*channel)->addAfter(&mUsedHead);

    return FMOD_OK;
}

// 4.06 functions with no G2MEAB native code; kept as placeholders.

FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, DSPI * dsp, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::getOutput(FMOD_OUTPUTTYPE * output)
{
}

FMOD_RESULT SystemI::setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplermethod)
{
}

FMOD_RESULT SystemI::getNumDrivers(int * numdrivers)
{
}

FMOD_RESULT SystemI::getDriverName(int id, char * name, int namelen)
{
}

FMOD_RESULT SystemI::getDriverCaps(int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode)
{
}

FMOD_RESULT SystemI::setDriver(int driver)
{
}

FMOD_RESULT SystemI::getDriver(int * driver)
{
}

FMOD_RESULT SystemI::setSoftwareChannels(int numsoftwarechannels)
{
}

FMOD_RESULT SystemI::getSoftwareChannels(int * numsoftwarechannels)
{
}

FMOD_RESULT SystemI::setDSPBufferSize(unsigned int bufferlength, int numbuffers)
{
}

FMOD_RESULT SystemI::setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int buffersize)
{
}

FMOD_RESULT SystemI::attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek)
{
}

FMOD_RESULT SystemI::setAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT SystemI::getAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT SystemI::setPluginPath(const char * path)
{
}

FMOD_RESULT SystemI::loadPlugin(const char * filename, FMOD_PLUGINTYPE * plugintype, int * index)
{
}

FMOD_RESULT SystemI::getNumPlugins(FMOD_PLUGINTYPE type, int * numplugins)
{
}

FMOD_RESULT SystemI::getPluginInfo(FMOD_PLUGINTYPE type, int index, char * name, int namelen, unsigned int * version)
{
}

FMOD_RESULT SystemI::unloadPlugin(FMOD_PLUGINTYPE type, int index)
{
}

FMOD_RESULT SystemI::setOutputByPlugin(int index)
{
}

FMOD_RESULT SystemI::getOutputByPlugin(int * index)
{
}

FMOD_RESULT SystemI::updateFinished()
{
}

FMOD_RESULT SystemI::getSpeakerPosition(FMOD_SPEAKER speaker, float * x, float * y)
{
}

FMOD_RESULT SystemI::get3DListenerAttributes(int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up)
{
}

FMOD_RESULT SystemI::set3DReverbProperties(int reverbid, const FMOD_REVERB_PROPERTIES * prop, const FMOD_VECTOR * pos, float mindistance, float maxdistance)
{
}

FMOD_RESULT SystemI::get3DReverbProperties(int reverbid, FMOD_REVERB_PROPERTIES * prop, const FMOD_VECTOR * pos, float * mindistance, float * maxdistance)
{
}

FMOD_RESULT SystemI::getVersion(unsigned int * version)
{
}

FMOD_RESULT SystemI::getOutputHandle(void * * handle)
{
}

FMOD_RESULT SystemI::getSoundRAM(int * currentalloced, int * maxalloced, int * total)
{
}

FMOD_RESULT SystemI::getNumCDROMDrives(int * numdrives)
{
}

FMOD_RESULT SystemI::getCDROMDriveName(int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen)
{
}

FMOD_RESULT SystemI::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT SystemI::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT SystemI::createStream(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
{
}

FMOD_RESULT SystemI::createCodec(FMOD_CODEC_DESCRIPTION * description)
{
}

FMOD_RESULT SystemI::createDSPByIndex(int index, DSPI * * dsp)
{
}

FMOD_RESULT SystemI::playDSP(FMOD_CHANNELINDEX channelid, DSPI * dsp, bool paused, ChannelI * * channel)
{
}

unsigned int SystemI::getReverbMaxInstances()
{
}

FMOD_RESULT SystemI::deleteReverb(unsigned int instance)
{
}

FMOD_RESULT SystemI::setReverbProperties(const FMOD_REVERB_PROPERTIES * prop, bool force_create)
{
}

FMOD_RESULT SystemI::createReverb(unsigned int * instance, ReverbI * * reverb)
{
}

FMOD_RESULT SystemI::getReverbProperties(FMOD_REVERB_PROPERTIES * prop)
{
}

FMOD_RESULT SystemI::getDSPHead(DSPI * * dsp)
{
}

FMOD_RESULT SystemI::addDSP(DSPI * dsp)
{
}

FMOD_RESULT SystemI::setRecordDriver(int driver)
{
}

FMOD_RESULT SystemI::getRecordDriver(int * driver)
{
}

FMOD_RESULT SystemI::getRecordNumDrivers(int * numdrivers)
{
}

FMOD_RESULT SystemI::getRecordDriverName(int id, char * name, int namelen)
{
}

FMOD_RESULT SystemI::recordStart(SoundI * sound, bool loop)
{
}

FMOD_RESULT SystemI::getRecordPosition(unsigned int * position)
{
}

FMOD_RESULT SystemI::isRecording(bool * recording)
{
}

FMOD_RESULT SystemI::createGeometry(int maxNumPolygons, int maxNumVertices, GeometryI * * geometry)
{
}

FMOD_RESULT SystemI::setGeometrySettings(float maxWorldSize)
{
}

FMOD_RESULT SystemI::getGeometrySettings(float * maxWorldSize)
{
}

FMOD_RESULT SystemI::loadGeometry(const void * data, int dataSize, GeometryI * * geometry)
{
}

FMOD_RESULT SystemI::setNetworkProxy(const char * proxy)
{
}

FMOD_RESULT SystemI::getNetworkProxy(char * proxy, int proxylen)
{
}

FMOD_RESULT SystemI::setNetworkTimeout(int timeout)
{
}

FMOD_RESULT SystemI::getNetworkTimeout(int * timeout)
{
}

FMOD_RESULT SystemI::setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype)
{
}

FMOD_RESULT SystemI::getStreamBufferSize(unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype)
{
}

FMOD_RESULT SystemI::setUserData(void * userdata)
{
}

FMOD_RESULT SystemI::getUserData(void * * userdata)
{
}

FMOD_RESULT SystemI::getSoundList(SoundI * * sound)
{
}

FMOD_RESULT SystemI::getChannelList(ChannelI * * channel)
{
}

FMOD_RESULT SystemI::flushDSPConnectionRequests(bool calledfrommainthread)
{
}

FMOD_RESULT SystemI::createFile(File * * file)
{
}

} // namespace FMOD
