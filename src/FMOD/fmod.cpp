// Partial reconstruction of the G2MEAB unit (.text 0x805B5DA4..0x805B60A0). The three retained natives
// (FMOD_Memory_Initialize 0x805B5DA4, FMOD_Memory_GetStats 0x805B5F30, FMOD_System_Create 0x805B5F60)
// are reconstructed in native order. The remaining C API wrappers are dead-stripped in G2MEAB and stay
// as empty 4.06 placeholders.

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_dsp.h"
#include "fmod_globals.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

#include <string.h>

// Guessed name: the native materializes a bool from both node links (0x805B5DB8), unlike
// LinkedListNode::isEmpty, which tests only the next link.
static inline bool isSystemListEmpty(FMOD::LinkedListNode * head)
{
    if (head->getNext() == head && head->getPrev() == head)
    {
        return true;
    }
    return false;
}

FMOD_RESULT FMOD_Memory_Initialize(void * poolmem, int poollen, FMOD_MEMORY_ALLOCCALLBACK useralloc, FMOD_MEMORY_REALLOCCALLBACK userrealloc, FMOD_MEMORY_FREECALLBACK userfree)
{
    if (!isSystemListEmpty(FMOD::gSystemHead))
    {
        return FMOD_ERR_INITIALIZED;
    }

    if (poollen % 64)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (poollen && poolmem)
    {
        FMOD_RESULT result;

        if (useralloc || userrealloc || userfree)
        {
            return FMOD_ERR_INVALID_PARAM;
        }
        if (poollen < 64)
        {
            return FMOD_ERR_INVALID_PARAM;
        }

        result = FMOD::gSystemPool->init(poolmem, poollen, 64);
        if (result == FMOD_OK)
        {
            FMOD::gSystemPool->setCallbacks(0, 0, 0);
        }
        return result;
    }
    else if (poolmem || poollen)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    else if (useralloc && userrealloc && userfree)
    {
        FMOD::gSystemPool->setCallbacks(useralloc, userrealloc, userfree);
    }
    else if (!useralloc && !userrealloc && !userfree)
    {
        FMOD::gSystemPool->setCallbacks(FMOD::Memory_DefaultMalloc, FMOD::Memory_DefaultRealloc, FMOD::Memory_DefaultFree);
    }
    else
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_Memory_GetStats(int * currentalloced, int * maxalloced)
{
    if (currentalloced)
    {
        *currentalloced = FMOD::gSystemPool->getCurrentAllocated();
    }
    if (maxalloced)
    {
        *maxalloced = FMOD::gSystemPool->getMaxAllocated();
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_System_Create(FMOD_SYSTEM * * system)
{
    FMOD::SystemI * sys;
    FMOD::LinkedListNode * node;
    unsigned char used[16];
    int count;

    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    sys = FMOD_Object_Calloc(FMOD::SystemI);
    *system = (FMOD_SYSTEM *)sys;
    if (!*system)
    {
        return FMOD_ERR_MEMORY;
    }

    memset(used, 0, 16);

    for (node = FMOD::gSystemHead->getNext(); node != FMOD::gSystemHead; node = node->getNext())
    {
        used[((FMOD::SystemI *)node)->mIndex - 1] = 1;
    }

    sys->addAfter(FMOD::gSystemHead);

    for (count = 0; count < 15; count++)
    {
        if (!used[count])
        {
            sys->mIndex = count + 1;
            break;
        }
    }

    if (count == 15)
    {
        FMOD_Memory_Free(sys);
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}

FMOD_RESULT FMOD_System_Release(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_SetOutput(FMOD_SYSTEM * system, FMOD_OUTPUTTYPE output)
{
}

FMOD_RESULT FMOD_System_GetOutput(FMOD_SYSTEM * system, FMOD_OUTPUTTYPE * output)
{
}

FMOD_RESULT FMOD_System_GetNumDrivers(FMOD_SYSTEM * system, int * numdrivers)
{
}

FMOD_RESULT FMOD_System_GetDriverName(FMOD_SYSTEM * system, int id, char * name, int namelen)
{
}

FMOD_RESULT FMOD_System_GetDriverCaps(FMOD_SYSTEM * system, int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode)
{
}

FMOD_RESULT FMOD_System_SetDriver(FMOD_SYSTEM * system, int driver)
{
}

FMOD_RESULT FMOD_System_GetDriver(FMOD_SYSTEM * system, int * driver)
{
}

FMOD_RESULT FMOD_System_SetHardwareChannels(FMOD_SYSTEM * system, int min2d, int max2d, int min3d, int max3d)
{
}

FMOD_RESULT FMOD_System_SetSoftwareChannels(FMOD_SYSTEM * system, int numsoftwarechannels)
{
}

FMOD_RESULT FMOD_System_GetSoftwareChannels(FMOD_SYSTEM * system, int * numsoftwarechannels)
{
}

FMOD_RESULT FMOD_System_SetSoftwareFormat(FMOD_SYSTEM * system, int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod)
{
}

FMOD_RESULT FMOD_System_GetSoftwareFormat(FMOD_SYSTEM * system, int * samplerate, FMOD_SOUND_FORMAT * format, int * numoutputchannels, int * maxinputchannels, FMOD_DSP_RESAMPLER * resamplemethod, int * bits)
{
}

FMOD_RESULT FMOD_System_SetDSPBufferSize(FMOD_SYSTEM * system, unsigned int bufferlength, int numbuffers)
{
}

FMOD_RESULT FMOD_System_GetDSPBufferSize(FMOD_SYSTEM * system, unsigned int * bufferlength, int * numbuffers)
{
}

FMOD_RESULT FMOD_System_SetFileSystem(FMOD_SYSTEM * system, FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blocksize)
{
}

FMOD_RESULT FMOD_System_AttachFileSystem(FMOD_SYSTEM * system, FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek)
{
}

FMOD_RESULT FMOD_System_SetAdvancedSettings(FMOD_SYSTEM * system, FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT FMOD_System_GetAdvancedSettings(FMOD_SYSTEM * system, FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT FMOD_System_SetSpeakerMode(FMOD_SYSTEM * system, FMOD_SPEAKERMODE speakermode)
{
}

FMOD_RESULT FMOD_System_GetSpeakerMode(FMOD_SYSTEM * system, FMOD_SPEAKERMODE * speakermode)
{
}

FMOD_RESULT FMOD_System_SetPluginPath(FMOD_SYSTEM * system, const char * path)
{
}

FMOD_RESULT FMOD_System_LoadPlugin(FMOD_SYSTEM * system, const char * filename, FMOD_PLUGINTYPE * plugintype, int * index)
{
}

FMOD_RESULT FMOD_System_GetNumPlugins(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int * numplugins)
{
}

FMOD_RESULT FMOD_System_GetPluginInfo(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int index, char * name, int namelen, unsigned int * version)
{
}

FMOD_RESULT FMOD_System_UnloadPlugin(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int index)
{
}

FMOD_RESULT FMOD_System_SetOutputByPlugin(FMOD_SYSTEM * system, int index)
{
}

FMOD_RESULT FMOD_System_GetOutputByPlugin(FMOD_SYSTEM * system, int * index)
{
}

FMOD_RESULT FMOD_System_CreateCodec(FMOD_SYSTEM * system, FMOD_CODEC_DESCRIPTION * description)
{
}

FMOD_RESULT FMOD_System_Init(FMOD_SYSTEM * system, int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata)
{
}

FMOD_RESULT FMOD_System_Close(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_Update(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_Set3DSettings(FMOD_SYSTEM * system, float dopplerscale, float distancefactor, float rolloffscale)
{
}

FMOD_RESULT FMOD_System_Get3DSettings(FMOD_SYSTEM * system, float * dopplerscale, float * distancefactor, float * rolloffscale)
{
}

FMOD_RESULT FMOD_System_Set3DNumListeners(FMOD_SYSTEM * system, int numlisteners)
{
}

FMOD_RESULT FMOD_System_Get3DNumListeners(FMOD_SYSTEM * system, int * numlisteners)
{
}

FMOD_RESULT FMOD_System_Set3DListenerAttributes(FMOD_SYSTEM * system, int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
{
}

FMOD_RESULT FMOD_System_Get3DListenerAttributes(FMOD_SYSTEM * system, int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up)
{
}

FMOD_RESULT FMOD_System_SetSpeakerPosition(FMOD_SYSTEM * system, FMOD_SPEAKER speaker, float x, float y)
{
}

FMOD_RESULT FMOD_System_GetSpeakerPosition(FMOD_SYSTEM * system, FMOD_SPEAKER speaker, float * x, float * y)
{
}

FMOD_RESULT FMOD_System_SetStreamBufferSize(FMOD_SYSTEM * system, unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype)
{
}

FMOD_RESULT FMOD_System_GetStreamBufferSize(FMOD_SYSTEM * system, unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype)
{
}

FMOD_RESULT FMOD_System_GetVersion(FMOD_SYSTEM * system, unsigned int * version)
{
}

FMOD_RESULT FMOD_System_GetOutputHandle(FMOD_SYSTEM * system, void * * handle)
{
}

FMOD_RESULT FMOD_System_GetChannelsPlaying(FMOD_SYSTEM * system, int * channels)
{
}

FMOD_RESULT FMOD_System_GetHardwareChannels(FMOD_SYSTEM * system, int * num2d, int * num3d, int * total)
{
}

FMOD_RESULT FMOD_System_GetCPUUsage(FMOD_SYSTEM * system, float * dsp, float * stream, float * update, float * total)
{
}

FMOD_RESULT FMOD_System_GetSoundRAM(FMOD_SYSTEM * system, int * currentalloced, int * maxalloced, int * total)
{
}

FMOD_RESULT FMOD_System_GetNumCDROMDrives(FMOD_SYSTEM * system, int * numdrives)
{
}

FMOD_RESULT FMOD_System_GetCDROMDriveName(FMOD_SYSTEM * system, int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen)
{
}

FMOD_RESULT FMOD_System_GetSpectrum(FMOD_SYSTEM * system, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT FMOD_System_GetWaveData(FMOD_SYSTEM * system, float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT FMOD_System_CreateSound(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND * * sound)
{
}

FMOD_RESULT FMOD_System_CreateStream(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND * * sound)
{
}

FMOD_RESULT FMOD_System_CreateDSP(FMOD_SYSTEM * system, FMOD_DSP_DESCRIPTION * description, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_System_CreateDSPByType(FMOD_SYSTEM * system, FMOD_DSP_TYPE type, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_System_CreateDSPByIndex(FMOD_SYSTEM * system, int index, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_System_CreateChannelGroup(FMOD_SYSTEM * system, const char * name, FMOD_CHANNELGROUP * * channelgroup)
{
}

FMOD_RESULT FMOD_System_PlaySound(FMOD_SYSTEM * system, FMOD_CHANNELINDEX channelid, FMOD_SOUND * sound, FMOD_BOOL paused, FMOD_CHANNEL * * channel)
{
}

FMOD_RESULT FMOD_System_PlayDSP(FMOD_SYSTEM * system, FMOD_CHANNELINDEX channelid, FMOD_DSP * dsp, FMOD_BOOL paused, FMOD_CHANNEL * * channel)
{
}

FMOD_RESULT FMOD_System_GetChannel(FMOD_SYSTEM * system, int channelid, FMOD_CHANNEL * * channel)
{
}

FMOD_RESULT FMOD_System_GetMasterChannelGroup(FMOD_SYSTEM * system, FMOD_CHANNELGROUP * * channelgroup)
{
}

FMOD_RESULT FMOD_System_SetReverbProperties(FMOD_SYSTEM * system, const FMOD_REVERB_PROPERTIES * prop)
{
}

FMOD_RESULT FMOD_System_GetReverbProperties(FMOD_SYSTEM * system, FMOD_REVERB_PROPERTIES * prop)
{
}

FMOD_RESULT FMOD_System_GetDSPHead(FMOD_SYSTEM * system, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_System_AddDSP(FMOD_SYSTEM * system, FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_System_LockDSP(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_UnlockDSP(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_SetRecordDriver(FMOD_SYSTEM * system, int driver)
{
}

FMOD_RESULT FMOD_System_GetRecordDriver(FMOD_SYSTEM * system, int * driver)
{
}

FMOD_RESULT FMOD_System_GetRecordNumDrivers(FMOD_SYSTEM * system, int * numdrivers)
{
}

FMOD_RESULT FMOD_System_GetRecordDriverName(FMOD_SYSTEM * system, int id, char * name, int namelen)
{
}

FMOD_RESULT FMOD_System_GetRecordPosition(FMOD_SYSTEM * system, unsigned int * position)
{
}

FMOD_RESULT FMOD_System_RecordStart(FMOD_SYSTEM * system, FMOD_SOUND * sound, FMOD_BOOL loop)
{
}

FMOD_RESULT FMOD_System_RecordStop(FMOD_SYSTEM * system)
{
}

FMOD_RESULT FMOD_System_IsRecording(FMOD_SYSTEM * system, FMOD_BOOL * recording)
{
}

FMOD_RESULT FMOD_System_CreateGeometry(FMOD_SYSTEM * system, int maxpolygons, int maxvertices, FMOD_GEOMETRY * * geometry)
{
}

FMOD_RESULT FMOD_System_SetGeometrySettings(FMOD_SYSTEM * system, float maxworldsize)
{
}

FMOD_RESULT FMOD_System_GetGeometrySettings(FMOD_SYSTEM * system, float * maxworldsize)
{
}

FMOD_RESULT FMOD_System_LoadGeometry(FMOD_SYSTEM * system, const void * data, int datasize, FMOD_GEOMETRY * * geometry)
{
}

FMOD_RESULT FMOD_System_SetNetworkProxy(FMOD_SYSTEM * system, const char * proxy)
{
}

FMOD_RESULT FMOD_System_GetNetworkProxy(FMOD_SYSTEM * system, char * proxy, int proxylen)
{
}

FMOD_RESULT FMOD_System_SetNetworkTimeout(FMOD_SYSTEM * system, int timeout)
{
}

FMOD_RESULT FMOD_System_GetNetworkTimeout(FMOD_SYSTEM * system, int * timeout)
{
}

FMOD_RESULT FMOD_System_SetUserData(FMOD_SYSTEM * system, void * userdata)
{
}

FMOD_RESULT FMOD_System_GetUserData(FMOD_SYSTEM * system, void * * userdata)
{
}

FMOD_RESULT FMOD_Sound_Release(FMOD_SOUND * sound)
{
}

FMOD_RESULT FMOD_Sound_GetSystemObject(FMOD_SOUND * sound, FMOD_SYSTEM * * system)
{
}

FMOD_RESULT FMOD_Sound_Lock(FMOD_SOUND * sound, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

FMOD_RESULT FMOD_Sound_Unlock(FMOD_SOUND * sound, void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
}

FMOD_RESULT FMOD_Sound_SetDefaults(FMOD_SOUND * sound, float frequency, float volume, float pan, int priority)
{
}

FMOD_RESULT FMOD_Sound_GetDefaults(FMOD_SOUND * sound, float * frequency, float * volume, float * pan, int * priority)
{
}

FMOD_RESULT FMOD_Sound_SetVariations(FMOD_SOUND * sound, float frequencyvar, float volumevar, float panvar)
{
}

FMOD_RESULT FMOD_Sound_GetVariations(FMOD_SOUND * sound, float * frequencyvar, float * volumevar, float * panvar)
{
}

FMOD_RESULT FMOD_Sound_Set3DMinMaxDistance(FMOD_SOUND * sound, float min, float max)
{
}

FMOD_RESULT FMOD_Sound_Get3DMinMaxDistance(FMOD_SOUND * sound, float * min, float * max)
{
}

FMOD_RESULT FMOD_Sound_Set3DConeSettings(FMOD_SOUND * sound, float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT FMOD_Sound_Get3DConeSettings(FMOD_SOUND * sound, float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
}

FMOD_RESULT FMOD_Sound_Set3DCustomRolloff(FMOD_SOUND * sound, FMOD_VECTOR * points, int numpoints)
{
}

FMOD_RESULT FMOD_Sound_Get3DCustomRolloff(FMOD_SOUND * sound, FMOD_VECTOR * * points, int * numpoints)
{
}

FMOD_RESULT FMOD_Sound_SetSubSound(FMOD_SOUND * sound, int index, FMOD_SOUND * subsound)
{
}

FMOD_RESULT FMOD_Sound_GetSubSound(FMOD_SOUND * sound, int index, FMOD_SOUND * * subsound)
{
}

FMOD_RESULT FMOD_Sound_SetSubSoundSentence(FMOD_SOUND * sound, int * subsoundlist, int numsubsounds)
{
}

FMOD_RESULT FMOD_Sound_GetName(FMOD_SOUND * sound, char * name, int namelen)
{
}

FMOD_RESULT FMOD_Sound_GetLength(FMOD_SOUND * sound, unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
}

FMOD_RESULT FMOD_Sound_GetFormat(FMOD_SOUND * sound, FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits)
{
}

FMOD_RESULT FMOD_Sound_GetNumSubSounds(FMOD_SOUND * sound, int * numsubsounds)
{
}

FMOD_RESULT FMOD_Sound_GetNumTags(FMOD_SOUND * sound, int * numtags, int * numtagsupdated)
{
}

FMOD_RESULT FMOD_Sound_GetTag(FMOD_SOUND * sound, const char * name, int index, FMOD_TAG * tag)
{
}

FMOD_RESULT FMOD_Sound_GetOpenState(FMOD_SOUND * sound, FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, FMOD_BOOL * starving)
{
}

FMOD_RESULT FMOD_Sound_ReadData(FMOD_SOUND * sound, void * buffer, unsigned int lenbytes, unsigned int * read)
{
}

FMOD_RESULT FMOD_Sound_SeekData(FMOD_SOUND * sound, unsigned int pcm)
{
}

FMOD_RESULT FMOD_Sound_GetNumSyncPoints(FMOD_SOUND * sound, int * numsyncpoints)
{
}

FMOD_RESULT FMOD_Sound_GetSyncPoint(FMOD_SOUND * sound, int index, FMOD_SYNCPOINT * * point)
{
}

FMOD_RESULT FMOD_Sound_GetSyncPointInfo(FMOD_SOUND * sound, FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype)
{
}

FMOD_RESULT FMOD_Sound_AddSyncPoint(FMOD_SOUND * sound, unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * point)
{
}

FMOD_RESULT FMOD_Sound_DeleteSyncPoint(FMOD_SOUND * sound, FMOD_SYNCPOINT * point)
{
}

FMOD_RESULT FMOD_Sound_SetMode(FMOD_SOUND * sound, FMOD_MODE mode)
{
}

FMOD_RESULT FMOD_Sound_GetMode(FMOD_SOUND * sound, FMOD_MODE * mode)
{
}

FMOD_RESULT FMOD_Sound_SetLoopCount(FMOD_SOUND * sound, int loopcount)
{
}

FMOD_RESULT FMOD_Sound_GetLoopCount(FMOD_SOUND * sound, int * loopcount)
{
}

FMOD_RESULT FMOD_Sound_SetLoopPoints(FMOD_SOUND * sound, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT FMOD_Sound_GetLoopPoints(FMOD_SOUND * sound, unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT FMOD_Sound_SetUserData(FMOD_SOUND * sound, void * userdata)
{
}

FMOD_RESULT FMOD_Sound_GetUserData(FMOD_SOUND * sound, void * * userdata)
{
}

FMOD_RESULT FMOD_Channel_GetSystemObject(FMOD_CHANNEL * channel, FMOD_SYSTEM * * system)
{
}

FMOD_RESULT FMOD_Channel_Stop(FMOD_CHANNEL * channel)
{
}

FMOD_RESULT FMOD_Channel_SetPaused(FMOD_CHANNEL * channel, FMOD_BOOL paused)
{
}

FMOD_RESULT FMOD_Channel_GetPaused(FMOD_CHANNEL * channel, FMOD_BOOL * paused)
{
}

FMOD_RESULT FMOD_Channel_SetVolume(FMOD_CHANNEL * channel, float volume)
{
}

FMOD_RESULT FMOD_Channel_GetVolume(FMOD_CHANNEL * channel, float * volume)
{
}

FMOD_RESULT FMOD_Channel_SetFrequency(FMOD_CHANNEL * channel, float frequency)
{
}

FMOD_RESULT FMOD_Channel_GetFrequency(FMOD_CHANNEL * channel, float * frequency)
{
}

FMOD_RESULT FMOD_Channel_SetPan(FMOD_CHANNEL * channel, float pan)
{
}

FMOD_RESULT FMOD_Channel_GetPan(FMOD_CHANNEL * channel, float * pan)
{
}

FMOD_RESULT FMOD_Channel_SetDelay(FMOD_CHANNEL * channel, unsigned int startdelay, unsigned int enddelay)
{
}

FMOD_RESULT FMOD_Channel_GetDelay(FMOD_CHANNEL * channel, unsigned int * startdelay, unsigned int * enddelay)
{
}

FMOD_RESULT FMOD_Channel_SetSpeakerMix(FMOD_CHANNEL * channel, float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
}

FMOD_RESULT FMOD_Channel_GetSpeakerMix(FMOD_CHANNEL * channel, float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright)
{
}

FMOD_RESULT FMOD_Channel_SetSpeakerLevels(FMOD_CHANNEL * channel, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_Channel_GetSpeakerLevels(FMOD_CHANNEL * channel, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_Channel_SetMute(FMOD_CHANNEL * channel, FMOD_BOOL mute)
{
}

FMOD_RESULT FMOD_Channel_GetMute(FMOD_CHANNEL * channel, FMOD_BOOL * mute)
{
}

FMOD_RESULT FMOD_Channel_SetPriority(FMOD_CHANNEL * channel, int priority)
{
}

FMOD_RESULT FMOD_Channel_GetPriority(FMOD_CHANNEL * channel, int * priority)
{
}

FMOD_RESULT FMOD_Channel_SetPosition(FMOD_CHANNEL * channel, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT FMOD_Channel_GetPosition(FMOD_CHANNEL * channel, unsigned int * position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT FMOD_Channel_SetReverbProperties(FMOD_CHANNEL * channel, const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT FMOD_Channel_GetReverbProperties(FMOD_CHANNEL * channel, FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT FMOD_Channel_SetChannelGroup(FMOD_CHANNEL * channel, FMOD_CHANNELGROUP * channelgroup)
{
}

FMOD_RESULT FMOD_Channel_GetChannelGroup(FMOD_CHANNEL * channel, FMOD_CHANNELGROUP * * channelgroup)
{
}

FMOD_RESULT FMOD_Channel_SetCallback(FMOD_CHANNEL * channel, FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command)
{
}

FMOD_RESULT FMOD_Channel_Set3DAttributes(FMOD_CHANNEL * channel, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel)
{
}

FMOD_RESULT FMOD_Channel_Get3DAttributes(FMOD_CHANNEL * channel, FMOD_VECTOR * pos, FMOD_VECTOR * vel)
{
}

FMOD_RESULT FMOD_Channel_Set3DMinMaxDistance(FMOD_CHANNEL * channel, float mindistance, float maxdistance)
{
}

FMOD_RESULT FMOD_Channel_Get3DMinMaxDistance(FMOD_CHANNEL * channel, float * mindistance, float * maxdistance)
{
}

FMOD_RESULT FMOD_Channel_Set3DConeSettings(FMOD_CHANNEL * channel, float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT FMOD_Channel_Get3DConeSettings(FMOD_CHANNEL * channel, float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
}

FMOD_RESULT FMOD_Channel_Set3DConeOrientation(FMOD_CHANNEL * channel, FMOD_VECTOR * orientation)
{
}

FMOD_RESULT FMOD_Channel_Get3DConeOrientation(FMOD_CHANNEL * channel, FMOD_VECTOR * orientation)
{
}

FMOD_RESULT FMOD_Channel_Set3DCustomRolloff(FMOD_CHANNEL * channel, FMOD_VECTOR * points, int numpoints)
{
}

FMOD_RESULT FMOD_Channel_Get3DCustomRolloff(FMOD_CHANNEL * channel, FMOD_VECTOR * * points, int * numpoints)
{
}

FMOD_RESULT FMOD_Channel_Set3DOcclusion(FMOD_CHANNEL * channel, float directocclusion, float reverbocclusion)
{
}

FMOD_RESULT FMOD_Channel_Get3DOcclusion(FMOD_CHANNEL * channel, float * directocclusion, float * reverbocclusion)
{
}

FMOD_RESULT FMOD_Channel_Set3DSpread(FMOD_CHANNEL * channel, float angle)
{
}

FMOD_RESULT FMOD_Channel_Get3DSpread(FMOD_CHANNEL * channel, float * angle)
{
}

FMOD_RESULT FMOD_Channel_Set3DPanLevel(FMOD_CHANNEL * channel, float level)
{
}

FMOD_RESULT FMOD_Channel_Get3DPanLevel(FMOD_CHANNEL * channel, float * level)
{
}

FMOD_RESULT FMOD_Channel_Set3DDopplerLevel(FMOD_CHANNEL * channel, float level)
{
}

FMOD_RESULT FMOD_Channel_Get3DDopplerLevel(FMOD_CHANNEL * channel, float * level)
{
}

FMOD_RESULT FMOD_Channel_GetDSPHead(FMOD_CHANNEL * channel, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_Channel_AddDSP(FMOD_CHANNEL * channel, FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_Channel_IsPlaying(FMOD_CHANNEL * channel, FMOD_BOOL * isplaying)
{
}

FMOD_RESULT FMOD_Channel_IsVirtual(FMOD_CHANNEL * channel, FMOD_BOOL * isvirtual)
{
}

FMOD_RESULT FMOD_Channel_GetAudibility(FMOD_CHANNEL * channel, float * audibility)
{
}

FMOD_RESULT FMOD_Channel_GetCurrentSound(FMOD_CHANNEL * channel, FMOD_SOUND * * sound)
{
}

FMOD_RESULT FMOD_Channel_GetSpectrum(FMOD_CHANNEL * channel, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT FMOD_Channel_GetWaveData(FMOD_CHANNEL * channel, float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT FMOD_Channel_GetIndex(FMOD_CHANNEL * channel, int * index)
{
}

FMOD_RESULT FMOD_Channel_SetMode(FMOD_CHANNEL * channel, FMOD_MODE mode)
{
}

FMOD_RESULT FMOD_Channel_GetMode(FMOD_CHANNEL * channel, FMOD_MODE * mode)
{
}

FMOD_RESULT FMOD_Channel_SetLoopCount(FMOD_CHANNEL * channel, int loopcount)
{
}

FMOD_RESULT FMOD_Channel_GetLoopCount(FMOD_CHANNEL * channel, int * loopcount)
{
}

FMOD_RESULT FMOD_Channel_SetLoopPoints(FMOD_CHANNEL * channel, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT FMOD_Channel_GetLoopPoints(FMOD_CHANNEL * channel, unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT FMOD_Channel_SetUserData(FMOD_CHANNEL * channel, void * userdata)
{
}

FMOD_RESULT FMOD_Channel_GetUserData(FMOD_CHANNEL * channel, void * * userdata)
{
}

FMOD_RESULT FMOD_ChannelGroup_Release(FMOD_CHANNELGROUP * channelgroup)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetSystemObject(FMOD_CHANNELGROUP * channelgroup, FMOD_SYSTEM * * system)
{
}

FMOD_RESULT FMOD_ChannelGroup_SetVolume(FMOD_CHANNELGROUP * channelgroup, float volume)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetVolume(FMOD_CHANNELGROUP * channelgroup, float * volume)
{
}

FMOD_RESULT FMOD_ChannelGroup_SetPitch(FMOD_CHANNELGROUP * channelgroup, float pitch)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetPitch(FMOD_CHANNELGROUP * channelgroup, float * pitch)
{
}

FMOD_RESULT FMOD_ChannelGroup_Stop(FMOD_CHANNELGROUP * channelgroup)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverridePaused(FMOD_CHANNELGROUP * channelgroup, FMOD_BOOL paused)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverrideVolume(FMOD_CHANNELGROUP * channelgroup, float volume)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverrideFrequency(FMOD_CHANNELGROUP * channelgroup, float frequency)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverridePan(FMOD_CHANNELGROUP * channelgroup, float pan)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverrideMute(FMOD_CHANNELGROUP * channelgroup, FMOD_BOOL mute)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverrideReverbProperties(FMOD_CHANNELGROUP * channelgroup, const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT FMOD_ChannelGroup_Override3DAttributes(FMOD_CHANNELGROUP * channelgroup, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel)
{
}

FMOD_RESULT FMOD_ChannelGroup_OverrideSpeakerMix(FMOD_CHANNELGROUP * channelgroup, float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
}

FMOD_RESULT FMOD_ChannelGroup_AddGroup(FMOD_CHANNELGROUP * channelgroup, FMOD_CHANNELGROUP * group)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetNumGroups(FMOD_CHANNELGROUP * channelgroup, int * numgroups)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetGroup(FMOD_CHANNELGROUP * channelgroup, int index, FMOD_CHANNELGROUP * * group)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetDSPHead(FMOD_CHANNELGROUP * channelgroup, FMOD_DSP * * dsp)
{
}

FMOD_RESULT FMOD_ChannelGroup_AddDSP(FMOD_CHANNELGROUP * channelgroup, FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetName(FMOD_CHANNELGROUP * channelgroup, char * name, int namelen)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetNumChannels(FMOD_CHANNELGROUP * channelgroup, int * numchannels)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetChannel(FMOD_CHANNELGROUP * channelgroup, int index, FMOD_CHANNEL * * channel)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetSpectrum(FMOD_CHANNELGROUP * channelgroup, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetWaveData(FMOD_CHANNELGROUP * channelgroup, float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT FMOD_ChannelGroup_SetUserData(FMOD_CHANNELGROUP * channelgroup, void * userdata)
{
}

FMOD_RESULT FMOD_ChannelGroup_GetUserData(FMOD_CHANNELGROUP * channelgroup, void * * userdata)
{
}

FMOD_RESULT FMOD_DSP_Release(FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_DSP_GetSystemObject(FMOD_DSP * dsp, FMOD_SYSTEM * * system)
{
}

FMOD_RESULT FMOD_DSP_AddInput(FMOD_DSP * dsp, FMOD_DSP * target)
{
}

FMOD_RESULT FMOD_DSP_DisconnectFrom(FMOD_DSP * dsp, FMOD_DSP * target)
{
}

FMOD_RESULT FMOD_DSP_DisconnectAll(FMOD_DSP * dsp, FMOD_BOOL inputs, FMOD_BOOL outputs)
{
}

FMOD_RESULT FMOD_DSP_Remove(FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_DSP_GetNumInputs(FMOD_DSP * dsp, int * numinputs)
{
}

FMOD_RESULT FMOD_DSP_GetNumOutputs(FMOD_DSP * dsp, int * numoutputs)
{
}

FMOD_RESULT FMOD_DSP_GetInput(FMOD_DSP * dsp, int index, FMOD_DSP * * input)
{
}

FMOD_RESULT FMOD_DSP_GetOutput(FMOD_DSP * dsp, int index, FMOD_DSP * * output)
{
}

FMOD_RESULT FMOD_DSP_SetInputMix(FMOD_DSP * dsp, int index, float volume)
{
}

FMOD_RESULT FMOD_DSP_GetInputMix(FMOD_DSP * dsp, int index, float * volume)
{
}

FMOD_RESULT FMOD_DSP_SetInputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_DSP_GetInputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_DSP_SetOutputMix(FMOD_DSP * dsp, int index, float volume)
{
}

FMOD_RESULT FMOD_DSP_GetOutputMix(FMOD_DSP * dsp, int index, float * volume)
{
}

FMOD_RESULT FMOD_DSP_SetOutputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_DSP_GetOutputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT FMOD_DSP_SetActive(FMOD_DSP * dsp, FMOD_BOOL active)
{
}

FMOD_RESULT FMOD_DSP_GetActive(FMOD_DSP * dsp, FMOD_BOOL * active)
{
}

FMOD_RESULT FMOD_DSP_SetBypass(FMOD_DSP * dsp, FMOD_BOOL bypass)
{
}

FMOD_RESULT FMOD_DSP_GetBypass(FMOD_DSP * dsp, FMOD_BOOL * bypass)
{
}

FMOD_RESULT FMOD_DSP_Reset(FMOD_DSP * dsp)
{
}

FMOD_RESULT FMOD_DSP_SetParameter(FMOD_DSP * dsp, int index, float value)
{
}

FMOD_RESULT FMOD_DSP_GetParameter(FMOD_DSP * dsp, int index, float * value, char * valuestr, int valuestrlen)
{
}

FMOD_RESULT FMOD_DSP_GetNumParameters(FMOD_DSP * dsp, int * numparams)
{
}

FMOD_RESULT FMOD_DSP_GetParameterInfo(FMOD_DSP * dsp, int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max)
{
}

FMOD_RESULT FMOD_DSP_ShowConfigDialog(FMOD_DSP * dsp, void * hwnd, FMOD_BOOL show)
{
}

FMOD_RESULT FMOD_DSP_GetInfo(FMOD_DSP * dsp, char * name, unsigned int * version, int * channels, int * configwidth, int * configheight)
{
}

FMOD_RESULT FMOD_DSP_GetType(FMOD_DSP * dsp, FMOD_DSP_TYPE * type)
{
}

FMOD_RESULT FMOD_DSP_SetDefaults(FMOD_DSP * dsp, float frequency, float volume, float pan, int priority)
{
}

FMOD_RESULT FMOD_DSP_GetDefaults(FMOD_DSP * dsp, float * frequency, float * volume, float * pan, int * priority)
{
}

FMOD_RESULT FMOD_DSP_SetUserData(FMOD_DSP * dsp, void * userdata)
{
}

FMOD_RESULT FMOD_DSP_GetUserData(FMOD_DSP * dsp, void * * userdata)
{
}

FMOD_RESULT FMOD_Geometry_Release(FMOD_GEOMETRY * geometry)
{
}

FMOD_RESULT FMOD_Geometry_AddPolygon(FMOD_GEOMETRY * geometry, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided, int numvertices, const FMOD_VECTOR * vertices, int * polygonindex)
{
}

FMOD_RESULT FMOD_Geometry_GetNumPolygons(FMOD_GEOMETRY * geometry, int * numpolygons)
{
}

FMOD_RESULT FMOD_Geometry_GetMaxPolygons(FMOD_GEOMETRY * geometry, int * maxpolygons, int * maxvertices)
{
}

FMOD_RESULT FMOD_Geometry_GetPolygonNumVertices(FMOD_GEOMETRY * geometry, int index, int * numvertices)
{
}

FMOD_RESULT FMOD_Geometry_SetPolygonVertex(FMOD_GEOMETRY * geometry, int index, int vertexindex, const FMOD_VECTOR * vertex)
{
}

FMOD_RESULT FMOD_Geometry_GetPolygonVertex(FMOD_GEOMETRY * geometry, int index, int vertexindex, FMOD_VECTOR * vertex)
{
}

FMOD_RESULT FMOD_Geometry_SetPolygonAttributes(FMOD_GEOMETRY * geometry, int index, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided)
{
}

FMOD_RESULT FMOD_Geometry_GetPolygonAttributes(FMOD_GEOMETRY * geometry, int index, float * directocclusion, float * reverbocclusion, FMOD_BOOL * doublesided)
{
}

FMOD_RESULT FMOD_Geometry_SetActive(FMOD_GEOMETRY * geometry, FMOD_BOOL active)
{
}

FMOD_RESULT FMOD_Geometry_GetActive(FMOD_GEOMETRY * geometry, FMOD_BOOL * active)
{
}

FMOD_RESULT FMOD_Geometry_SetRotation(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
{
}

FMOD_RESULT FMOD_Geometry_GetRotation(FMOD_GEOMETRY * geometry, FMOD_VECTOR * forward, FMOD_VECTOR * up)
{
}

FMOD_RESULT FMOD_Geometry_SetPosition(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * position)
{
}

FMOD_RESULT FMOD_Geometry_GetPosition(FMOD_GEOMETRY * geometry, FMOD_VECTOR * position)
{
}

FMOD_RESULT FMOD_Geometry_SetScale(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * scale)
{
}

FMOD_RESULT FMOD_Geometry_GetScale(FMOD_GEOMETRY * geometry, FMOD_VECTOR * scale)
{
}

FMOD_RESULT FMOD_Geometry_Save(FMOD_GEOMETRY * geometry, void * data, int * datasize)
{
}

FMOD_RESULT FMOD_Geometry_SetUserData(FMOD_GEOMETRY * geometry, void * userdata)
{
}

FMOD_RESULT FMOD_Geometry_GetUserData(FMOD_GEOMETRY * geometry, void * * userdata)
{
}
