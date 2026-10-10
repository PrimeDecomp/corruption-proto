// G2MEAB SystemI layout (sizeof 0x1148, System_Create 0x805B5F60 callocs 0x1148). Offsets come from the
// constructor 0x8061E5B4, the implicit complete destructor 0x8060B0D4 (emitted in fmod_globals for the
// static list head 0x807542DC) and member accesses in other TUs. Differences from 4.06: 0x14-byte list
// node, no DSP connection-request queue or second/third DSP critical section, no plugin index cache, a
// 0x10-byte FMOD_ADVANCEDSETTINGS, two DSPCodecPools (no RAW pool) and two ChannelStream list heads where
// 4.06 has ReverbI members. Method declarations are still the 4.06 set.

#ifndef _FMOD_SYSTEMI_H
#define _FMOD_SYSTEMI_H

#include "fmod.h"
#include "fmod_channel_stream.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_dsp.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_dsp_connectionpool.h"
#include "fmod_dspi.h"
#include "fmod_geometry_mgr.h"
#include "fmod_linkedlist.h"
#include "fmod_listener.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_reverbi.h"
#include "fmod_soundi.h"
#include "fmod_time.h"

struct FMOD_ADVANCEDSETTINGS;
struct FMOD_CODEC_DESCRIPTION;
struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_CREATESOUNDEXINFO;
struct FMOD_DSP_DESCRIPTION;
struct FMOD_REVERB_PROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct Channel;
    struct ChannelGroupI;
    struct ChannelI;
    struct ChannelStream;
    class DSPI;
    struct FMOD_DSP_DESCRIPTION_EX;
    struct FMOD_SPEAKERCONFIG;
    class File;
    class GeometryI;
    class LinkedListNode;
    struct Listener;
    class Output;
    class OutputEmulated;
    class OutputSoftware;
    class PluginFactory;
    class ReverbI;
    struct Sample;
    struct SoundI;
    struct System;
    struct SystemI;
    class Thread;
    struct TimeStamp;
}

namespace FMOD {

// G2MEAB: 0x18 bytes, SystemI+0xEC0 [8]; setSpeakerPosition 0x8061FC4C.
struct FMOD_SPEAKERCONFIG
{
    FMOD_SPEAKER mSpeaker; // offset 0x0
    FMOD_VECTOR mPosition; // offset 0x4
    int mXZAngle; // offset 0x10
    float mDistance; // offset 0x14
};

const FMOD_SPEAKERMODE FMOD_SPEAKERMODE_STEREO_LINEAR = (FMOD_SPEAKERMODE)1000;
const int FMOD_MAXPRIORITIES = 256;
const int FMOD_MAXAUDIBIILITY = 1000;
const int FMOD_STREAMDECODEBUFFERSIZE_DEFAULT = 400;
const int FMOD_ADVANCEDSETTINGS_MAXXMACODECS = 32;
const int FMOD_ADVANCEDSETTINGS_MAXADPCMCODECS = 32;
const int FMOD_ADVANCEDSETTINGS_MAXMPEGCODECS = 16;
// vtable 0x806EE00C (only the node destructor slot).
struct SystemI : public LinkedListNode
{
    bool mInitialized; // offset 0x14
    bool mPluginsLoaded; // offset 0x15
    unsigned int mMainThreadID; // offset 0x18
    FMOD_INITFLAGS mFlags; // offset 0x1C
    SoundI mSoundListHead; // offset 0x20
    static FMOD_OS_CRITICALSECTION * gSoundListCrit;
    int mNumChannels; // offset 0x368
    ChannelI * mChannel; // offset 0x36C
    ChannelI mChannelUsedListHead; // offset 0x370
    ChannelI mChannelFreeListHead; // offset 0x4AC
    LinkedListNode mChannelSortedListHead; // offset 0x5E8
    Output * mOutput; // offset 0x5FC
    FMOD_OUTPUTTYPE mOutputType; // offset 0x600
    FMOD_SOUND_FORMAT mOutputFormat; // offset 0x604, ctor 2
    int mOutputRate; // offset 0x608, ctor 48000
    int mOutputIndex; // offset 0x60C
    int mMaxOutputChannels; // offset 0x610, setSpeakerMode 0x8061ED84 writes 1/2
    int mSelectedDriver; // offset 0x614, ctor -1
    int mMaxInputChannels; // offset 0x618, ctor 8
    OutputEmulated * mEmulated; // offset 0x61C
    int mSelectedRecordDriver; // offset 0x620, ctor -1
    unsigned int mDSPBlockSize; // offset 0x624, ctor 0x400
    unsigned int mDSPBufferSize; // offset 0x628, ctor 0x1000
    float * mDSPReadBuff[2]; // offset 0x62C
    float * mDSPMixBuff[128]; // offset 0x634
    DSPConnectionPool mDSPConnectionPool; // offset 0x834
    FMOD_OS_CRITICALSECTION * mDSPCrit; // offset 0x934
    bool mDSPActive; // offset 0x938
    DSPI * mDSPSoundCard; // offset 0x93C
    int mUnk940; // offset 0x940, not accessed in FMOD code
    DSPI * mDSPChannelGroupTarget; // offset 0x944, released in closeEx 0x8061F844
    TimeStamp mDSPTimeStamp; // offset 0x948
    int mDSPReadBuffIndex; // offset 0x980
    Listener mListener[4]; // offset 0x984, __construct_array with 0x8060B50C
    int mNumListeners; // offset 0xB44, ctor 1
    float mDistanceScale; // offset 0xB48, ctor 1.0
    float mRolloffScale; // offset 0xB4C, ctor 1.0
    float mDopplerScale; // offset 0xB50, ctor 1.0
    int mUnkB54; // offset 0xB54, not accessed in FMOD code
    int mUnkB58; // offset 0xB58, ctor 1
    PluginFactory * mPluginFactory; // offset 0xB5C
    char mPluginPath[256]; // offset 0xB60
    FMOD_ADVANCEDSETTINGS mAdvancedSettings; // offset 0xC60 (0x10; ctor 16/32/32)
    void * mUserData; // offset 0xC70
    TimeStamp mUpdateTimeStamp; // offset 0xC74
    unsigned int mLastTimeStamp; // offset 0xCAC
    unsigned int mIndex; // offset 0xCB0, 1..15 from System_Create
    int mNumSoftwareChannels; // offset 0xCB4, ctor 64
    int mMinHardwareChannels2D; // offset 0xCB8
    int mMaxHardwareChannels2D; // offset 0xCBC, ctor 1000
    int mMinHardwareChannels3D; // offset 0xCC0
    int mMaxHardwareChannels3D; // offset 0xCC4, ctor 1000
    ChannelGroupI * mChannelGroup; // offset 0xCC8
    ChannelGroupI mChannelGroupHead; // offset 0xCCC
    FMOD_DSP_RESAMPLER mResampleMethod; // offset 0xE1C, ctor 1
    MemSingleton mMultiSubSampleLockBuffer; // offset 0xE20 (Sample::release 0x80615224)
    FMOD_REVERB_PROPERTIES mReverbProperties; // offset 0xE28, ctor copies lbl_806B06F8
    FMOD_FILE_OPENCALLBACK mOpenRiderCallback; // offset 0xEA4 (fmod_file 0x806089EC)
    FMOD_FILE_CLOSECALLBACK mCloseRiderCallback; // offset 0xEA8
    FMOD_FILE_READCALLBACK mReadRiderCallback; // offset 0xEAC
    FMOD_FILE_SEEKCALLBACK mSeekRiderCallback; // offset 0xEB0
    unsigned int mStreamFileBufferSize; // offset 0xEB4, ctor 0x4000
    FMOD_TIMEUNIT mStreamFileBufferSizeType; // offset 0xEB8, ctor 8 (RAWBYTES)
    FMOD_SPEAKERMODE mSpeakerMode; // offset 0xEBC
    FMOD_SPEAKERCONFIG mSpeaker[8]; // offset 0xEC0
    FMOD_SPEAKERCONFIG * mSpeakerList[8]; // offset 0xF80
    static void streamThread(void * data);
    static LinkedListNode gStreamHead;
    static Thread gStreamThread;
    static bool gStreamThreadActive;
    static FMOD_OS_CRITICALSECTION * gStreamCrit;
    static FMOD_OS_CRITICALSECTION * gStreamFillCrit;
    static FMOD_OS_CRITICALSECTION * gStreamListCrit;
    static TimeStamp gStreamTimeStamp;
    OutputSoftware * mSoftware; // offset 0xFA0
    DSPCodecPool mDSPCodecPool_MPEG; // offset 0xFA4 (0x10; ctor stores mSystem, codec_mpeg reads +0x4)
    DSPCodecPool mDSPCodecPool_ADPCM; // offset 0xFB4
    GeometryI * mGeometryList; // offset 0xFC4
    GeometryMgr mGeometryMgr; // offset 0xFC8, ctor 0x8060A3C8, dtor 0x8060A3E8
    ChannelStreamPool mStreamPool; // offset 0xFE0 (0x168, ends 0x1148), Guessed name; used list walked via mUsedHead's +0x78 node (0x80620E28)
    FMOD_RESULT getReverbProperties(unsigned int, FMOD_REVERB_PROPERTIES *);
    FMOD_RESULT setReverbProperties(unsigned int, const FMOD_REVERB_PROPERTIES *);
    unsigned int getReverbMaxInstances();
    FMOD_RESULT createReverb(unsigned int * instance, ReverbI * * reverb);
    FMOD_RESULT deleteReverb(unsigned int instance);
    static FMOD_RESULT getInstance(unsigned int id, SystemI * * sys);
    static FMOD_RESULT validate(System * system, SystemI * * systemi);
    FMOD_RESULT updateStreams();
    FMOD_RESULT updateChannels(int delta);
    FMOD_RESULT getLowestPriorityUsedChannel(FMOD_MODE, ChannelI * *, LinkedListNode * *, int *, int);
    FMOD_RESULT getLowestPriorityUsedChannel(FMOD_MODE, ChannelI * *);
    FMOD_RESULT findChannel(FMOD_CHANNELINDEX id, SoundI * sound, ChannelI * * channel);
    FMOD_RESULT findChannel(FMOD_CHANNELINDEX id, DSPI * dsp, ChannelI * * channel);
    FMOD_RESULT createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample_out);
    FMOD_RESULT createDSP(FMOD_DSP_DESCRIPTION_EX * description, DSPI * * dsp); // G2MEAB 0x80620600 takes no allocate flag (r6 unread)
    FMOD_RESULT createSoundInternal(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound);
    FMOD_RESULT setUpPlugins();
    FMOD_RESULT sortSpeakerList();
    FMOD_RESULT allocDSPCodec(FMOD_SOUND_FORMAT format, DSPI * * dsp);
    // Inline in G2MEAB: expanded in OutputPolled::threadFunc 0x8060F890 / start 0x8060FF30,
    // OutputNoSound::getPosition 0x8060F584 and OutputNoSound_NRT::init 0x80626454 (4.06 store order).
    FMOD_RESULT getSoftwareFormat(int * samplerate, FMOD_SOUND_FORMAT * format, int * numoutputchannels, int * maxinputchannels, FMOD_DSP_RESAMPLER * resamplemethod, int * bits)
    {
        if (samplerate)
        {
            *samplerate = mOutputRate;
        }
        if (format)
        {
            *format = mOutputFormat;
        }
        if (numoutputchannels)
        {
            *numoutputchannels = mMaxOutputChannels;
        }
        if (maxinputchannels)
        {
            *maxinputchannels = mMaxInputChannels;
        }
        if (resamplemethod)
        {
            *resamplemethod = mResampleMethod;
        }
        if (bits)
        {
            SoundI::getBitsFromFormat(mOutputFormat, bits);
        }
        return FMOD_OK;
    }
    SystemI();
    FMOD_RESULT release();
    FMOD_RESULT setOutput(FMOD_OUTPUTTYPE outputtype);
    FMOD_RESULT getOutput(FMOD_OUTPUTTYPE * output);
    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int id, char * name, int namelen);
    FMOD_RESULT getDriverCaps(int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode);
    FMOD_RESULT setDriver(int driver);
    FMOD_RESULT getDriver(int * driver);
    FMOD_RESULT setHardwareChannels(int min2d, int max2d, int min3d, int max3d);
    FMOD_RESULT getHardwareChannels(int * num2d, int * num3d, int * total);
    FMOD_RESULT setSoftwareChannels(int numsoftwarechannels);
    FMOD_RESULT getSoftwareChannels(int * numsoftwarechannels);
    FMOD_RESULT setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplermethod);
    FMOD_RESULT setDSPBufferSize(unsigned int bufferlength, int numbuffers);
    FMOD_RESULT getDSPBufferSize(unsigned int * bufferlength, int * numbuffers);
    FMOD_RESULT setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int buffersize);
    FMOD_RESULT attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
    FMOD_RESULT setAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings);
    FMOD_RESULT getAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings);
    FMOD_RESULT setSpeakerMode(FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT getSpeakerMode(FMOD_SPEAKERMODE *);
    FMOD_RESULT setPluginPath(const char * path);
    FMOD_RESULT loadPlugin(const char * filename, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT getNumPlugins(FMOD_PLUGINTYPE type, int * numplugins);
    FMOD_RESULT getPluginInfo(FMOD_PLUGINTYPE type, int index, char * name, int namelen, unsigned int * version);
    FMOD_RESULT unloadPlugin(FMOD_PLUGINTYPE type, int index);
    FMOD_RESULT setOutputByPlugin(int index);
    FMOD_RESULT getOutputByPlugin(int * index);
    FMOD_RESULT createCodec(FMOD_CODEC_DESCRIPTION * description);
    FMOD_RESULT init(int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT closeEx(bool calledfrominit);
    FMOD_RESULT update();
    FMOD_RESULT updateFinished();
    FMOD_RESULT setSpeakerPosition(FMOD_SPEAKER speaker, float x, float y);
    FMOD_RESULT getSpeakerPosition(FMOD_SPEAKER speaker, float * x, float * y);
    FMOD_RESULT set3DSettings(float dopplerscale, float distancescale, float rolloffscale);
    FMOD_RESULT get3DSettings(float * dopplerscale, float * distancescale, float * rolloffscale);
    FMOD_RESULT set3DNumListeners(int numlisteners);
    FMOD_RESULT get3DNumListeners(int * numlisteners);
    FMOD_RESULT set3DListenerAttributes(int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
    FMOD_RESULT get3DListenerAttributes(int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up);
    FMOD_RESULT set3DReverbProperties(int reverbid, const FMOD_REVERB_PROPERTIES * prop, const FMOD_VECTOR * pos, float mindistance, float maxdistance);
    FMOD_RESULT get3DReverbProperties(int reverbid, FMOD_REVERB_PROPERTIES * prop, const FMOD_VECTOR * pos, float * mindistance, float * maxdistance);
    FMOD_RESULT setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
    FMOD_RESULT getStreamBufferSize(unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype);
    FMOD_RESULT getVersion(unsigned int * version);
    FMOD_RESULT getOutputHandle(void * * handle);
    FMOD_RESULT getChannelsPlaying(int * channels);
    FMOD_RESULT getCPUUsage(float * dsp, float * stream, float * update, float * total);
    FMOD_RESULT getSoundRAM(int * currentalloced, int * maxalloced, int * total);
    FMOD_RESULT getNumCDROMDrives(int * numdrives);
    FMOD_RESULT getCDROMDriveName(int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    FMOD_RESULT createSound(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound);
    FMOD_RESULT createStream(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound);
    FMOD_RESULT createDSP(FMOD_DSP_DESCRIPTION * description, DSPI * * dsp);
    FMOD_RESULT createDSPByType(FMOD_DSP_TYPE type, DSPI * * dsp);
    FMOD_RESULT createDSPByIndex(int index, DSPI * * dsp);
    FMOD_RESULT createChannelGroup(const char * name, ChannelGroupI * * channelgroup);
    FMOD_RESULT playSound(FMOD_CHANNELINDEX channelid, SoundI * sound, bool paused, Channel * * channel); // G2MEAB 0x80620B54 validates *channel as a handle and returns one
    FMOD_RESULT playDSP(FMOD_CHANNELINDEX channelid, DSPI * dsp, bool paused, ChannelI * * channel);
    FMOD_RESULT getChannel(int id, Channel * * channel); // G2MEAB 0x80620CE4 writes a handle, not a ChannelI *
    FMOD_RESULT getMasterChannelGroup(ChannelGroupI * * channelgroup);
    FMOD_RESULT setReverbProperties(const FMOD_REVERB_PROPERTIES * prop, bool force_create);
    FMOD_RESULT getReverbProperties(FMOD_REVERB_PROPERTIES * prop);
    FMOD_RESULT getDSPHead(DSPI * * dsp);
    FMOD_RESULT addDSP(DSPI * dsp);
    FMOD_RESULT lockDSP();
    FMOD_RESULT unlockDSP();
    FMOD_RESULT setRecordDriver(int driver);
    FMOD_RESULT getRecordDriver(int * driver);
    FMOD_RESULT getRecordNumDrivers(int * numdrivers);
    FMOD_RESULT getRecordDriverName(int id, char * name, int namelen);
    FMOD_RESULT getRecordPosition(unsigned int * position);
    FMOD_RESULT recordStart(SoundI * sound, bool loop);
    FMOD_RESULT recordStop();
    FMOD_RESULT isRecording(bool * recording);
    FMOD_RESULT createGeometry(int maxNumPolygons, int maxNumVertices, GeometryI * * geometry);
    FMOD_RESULT setGeometrySettings(float maxWorldSize);
    FMOD_RESULT getGeometrySettings(float * maxWorldSize);
    FMOD_RESULT loadGeometry(const void * data, int dataSize, GeometryI * * geometry);
    FMOD_RESULT setNetworkProxy(const char * proxy);
    FMOD_RESULT getNetworkProxy(char * proxy, int proxylen);
    FMOD_RESULT setNetworkTimeout(int timeout);
    FMOD_RESULT getNetworkTimeout(int * timeout);
    FMOD_RESULT setUserData(void * userdata);
    FMOD_RESULT getUserData(void * * userdata);
    FMOD_RESULT getSoundList(SoundI * * sound);
    FMOD_RESULT getChannelList(ChannelI * * channel);
    FMOD_RESULT stopSound(SoundI * sound);
    FMOD_RESULT stopDSP(DSPI * dsp);
    FMOD_RESULT getListenerObject(int listener, Listener * * listenerobject);
    FMOD_RESULT flushDSPConnectionRequests(bool calledfrommainthread);
    static FMOD_RESULT createFile(File * * file);
};

} // namespace FMOD

#endif
