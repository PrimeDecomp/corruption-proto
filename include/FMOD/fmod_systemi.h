// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SYSTEMI_H
#define _FMOD_SYSTEMI_H

#include "fmod.h"
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
    struct ChannelGroupI;
    struct ChannelI;
    class DSPI;
    struct FMOD_DSP_DESCRIPTION_EX;
    struct FMOD_SPEAKERCONFIG;
    class File;
    class GeometryI;
    struct Global;
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
struct SystemI : public LinkedListNode
{
    bool mInitialized; // offset 0xC
    bool mPluginsLoaded; // offset 0xD
    unsigned int mMainThreadID; // offset 0x10
    FMOD_INITFLAGS mFlags; // offset 0x14
    SoundI mSoundListHead; // offset 0x18
    static FMOD_OS_CRITICALSECTION * gSoundListCrit;
    int mNumChannels; // offset 0xE4
    ChannelI * mChannel; // offset 0xE8
    ChannelI mChannelUsedListHead; // offset 0xEC
    ChannelI mChannelFreeListHead; // offset 0x228
    LinkedListNode mChannelSortedListHead; // offset 0x364
    Output * mOutput; // offset 0x374
    FMOD_OUTPUTTYPE mOutputType; // offset 0x378
    FMOD_SOUND_FORMAT mOutputFormat; // offset 0x37C
    int mOutputRate; // offset 0x380
    int mOutputIndex; // offset 0x384
    int mMaxInputChannels; // offset 0x388
    int mMaxOutputChannels; // offset 0x38C
    int mSelectedDriver; // offset 0x390
    OutputEmulated * mEmulated; // offset 0x394
    int mSelectedRecordDriver; // offset 0x398
    unsigned int mDSPBlockSize; // offset 0x39C
    unsigned int mDSPBufferSize; // offset 0x3A0
    float * mDSPReadBuff[2]; // offset 0x3A4
    float * mDSPReadBuffMem[2]; // offset 0x3AC
    float * mDSPMixBuff[128]; // offset 0x3B4
    DSPConnectionPool mDSPConnectionPool; // offset 0x5B4
    FMOD_OS_CRITICALSECTION * mDSPCrit; // offset 0xE64
    FMOD_OS_CRITICALSECTION * mDSPConnectionCrit; // offset 0xE68
    FMOD_OS_CRITICALSECTION * mDSPQueueCrit; // offset 0xE6C
    bool mDSPActive; // offset 0xE70
    DSPI * mDSPSoundCard; // offset 0xE74
    DSPI * mDSPChannelGroupTarget; // offset 0xE78
    TimeStamp mDSPTimeStamp; // offset 0xE7C
    int mDSPReadBuffIndex; // offset 0xEB4
    DSPConnectionRequest mConnectionRequest[512]; // offset 0xEB8
    DSPConnectionRequest mConnectionRequestUsedHead; // offset 0x46B8
    DSPConnectionRequest mConnectionRequestFreeHead; // offset 0x46D4
    bool mConnectionRequestFlushing; // offset 0x46F0
    Listener mListener[4]; // offset 0x46F4
    int mNumListeners; // offset 0x48B4
    float mDistanceScale; // offset 0x48B8
    float mRolloffScale; // offset 0x48BC
    float mDopplerScale; // offset 0x48C0
    PluginFactory * mPluginFactory; // offset 0x48C4
    char mPluginPath[256]; // offset 0x48C8
    int mFSBPluginIndex; // offset 0x49C8
    int mWAVPluginIndex; // offset 0x49CC
    int mMPEGPluginIndex; // offset 0x49D0
    FMOD_ADVANCEDSETTINGS mAdvancedSettings; // offset 0x49D4
    void * mUserData; // offset 0x49EC
    TimeStamp mUpdateTimeStamp; // offset 0x49F0
    unsigned int mLastTimeStamp; // offset 0x4A28
    unsigned int mIndex; // offset 0x4A2C
    int mNumSoftwareChannels; // offset 0x4A30
    int mMinHardwareChannels2D; // offset 0x4A34
    int mMaxHardwareChannels2D; // offset 0x4A38
    int mMinHardwareChannels3D; // offset 0x4A3C
    int mMaxHardwareChannels3D; // offset 0x4A40
    ChannelGroupI * mChannelGroup; // offset 0x4A44
    ChannelGroupI mChannelGroupHead; // offset 0x4A48
    FMOD_DSP_RESAMPLER mResampleMethod; // offset 0x4A90
    MemSingleton mMultiSubSampleLockBuffer; // offset 0x4A94
    FMOD_FILE_OPENCALLBACK mOpenRiderCallback; // offset 0x4A9C
    FMOD_FILE_CLOSECALLBACK mCloseRiderCallback; // offset 0x4AA0
    FMOD_FILE_READCALLBACK mReadRiderCallback; // offset 0x4AA4
    FMOD_FILE_SEEKCALLBACK mSeekRiderCallback; // offset 0x4AA8
    FMOD_REVERB_PROPERTIES mReverbProperties; // offset 0x4AAC
    unsigned int mStreamFileBufferSize; // offset 0x4B28
    FMOD_TIMEUNIT mStreamFileBufferSizeType; // offset 0x4B2C
    FMOD_SPEAKERMODE mSpeakerMode; // offset 0x4B30
    FMOD_SPEAKERCONFIG mSpeaker[8]; // offset 0x4B34
    FMOD_SPEAKERCONFIG * mSpeakerList[8]; // offset 0x4BF4
    static void streamThread(void * data);
    static LinkedListNode gStreamHead;
    static Thread gStreamThread;
    static bool gStreamThreadActive;
    static FMOD_OS_CRITICALSECTION * gStreamCrit;
    static FMOD_OS_CRITICALSECTION * gStreamFillCrit;
    static FMOD_OS_CRITICALSECTION * gStreamListCrit;
    static TimeStamp gStreamTimeStamp;
    OutputSoftware * mSoftware; // offset 0x4C14
    DSPCodecPool mDSPCodecPool_MPEG; // offset 0x4C18
    DSPCodecPool mDSPCodecPool_ADPCM; // offset 0x4D28
    DSPCodecPool mDSPCodecPool_RAW; // offset 0x4E38
    GeometryI * mGeometryList; // offset 0x4F48
    GeometryMgr mGeometryMgr; // offset 0x4F4C
    ReverbI mListenerReverb; // offset 0x4F64
    ReverbI mReverbHead; // offset 0x500C
    unsigned int mNumReverbs; // offset 0x50B4
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
    FMOD_RESULT createDSP(FMOD_DSP_DESCRIPTION_EX * description, DSPI * * dsp, bool allocate);
    FMOD_RESULT createSoundInternal(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound);
    FMOD_RESULT setUpPlugins();
    FMOD_RESULT sortSpeakerList();
    FMOD_RESULT allocDSPCodec(FMOD_SOUND_FORMAT format, DSPI * * dsp);
    FMOD_RESULT getSoftwareFormat(int *, FMOD_SOUND_FORMAT *, int *, int *, FMOD_DSP_RESAMPLER *, int *);
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
    FMOD_RESULT playSound(FMOD_CHANNELINDEX channelid, SoundI * sound, bool paused, ChannelI * * channel);
    FMOD_RESULT playDSP(FMOD_CHANNELINDEX channelid, DSPI * dsp, bool paused, ChannelI * * channel);
    FMOD_RESULT getChannel(int id, ChannelI * * channel);
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
    static FMOD_RESULT getGlobals(Global * * global);
};

} // namespace FMOD

#endif
