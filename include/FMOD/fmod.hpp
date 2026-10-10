// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_HPP
#define _FMOD_HPP

#include "fmod.h"
#include "fmod_dsp.h"

struct FMOD_ADVANCEDSETTINGS;
struct FMOD_CODEC_DESCRIPTION;
struct FMOD_CREATESOUNDEXINFO;
struct FMOD_DSP_DESCRIPTION;
struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_REVERB_PROPERTIES;
struct FMOD_TAG;
struct FMOD_VECTOR;
namespace FMOD {
    struct Channel;
    struct ChannelGroup;
    struct DSP;
    struct Geometry;
    struct Sound;
    struct System;
}

namespace FMOD {

struct System
{
    System();
    FMOD_RESULT release();
    FMOD_RESULT setOutput(FMOD_OUTPUTTYPE output);
    FMOD_RESULT getOutput(FMOD_OUTPUTTYPE * output);
    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int id, char * name, int namelen);
    FMOD_RESULT getDriverCaps(int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode);
    FMOD_RESULT setDriver(int driver);
    FMOD_RESULT getDriver(int * driver);
    FMOD_RESULT setHardwareChannels(int min2d, int max2d, int min3d, int max3d);
    FMOD_RESULT setSoftwareChannels(int numsoftwarechannels);
    FMOD_RESULT getSoftwareChannels(int * numsoftwarechannels);
    FMOD_RESULT setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod);
    FMOD_RESULT getSoftwareFormat(int * samplerate, FMOD_SOUND_FORMAT * format, int * numoutputchannels, int * maxinputchannels, FMOD_DSP_RESAMPLER * resamplemethod, int * bits);
    FMOD_RESULT setDSPBufferSize(unsigned int bufferlength, int numbuffers);
    FMOD_RESULT getDSPBufferSize(unsigned int * bufferlength, int * numbuffers);
    FMOD_RESULT setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blocksize);
    FMOD_RESULT attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
    FMOD_RESULT setAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings);
    FMOD_RESULT getAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings);
    FMOD_RESULT setSpeakerMode(FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT getSpeakerMode(FMOD_SPEAKERMODE * speakermode);
    FMOD_RESULT setPluginPath(const char * path);
    FMOD_RESULT loadPlugin(const char * filename, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT getNumPlugins(FMOD_PLUGINTYPE plugintype, int * numplugins);
    FMOD_RESULT getPluginInfo(FMOD_PLUGINTYPE plugintype, int index, char * name, int namelen, unsigned int * version);
    FMOD_RESULT unloadPlugin(FMOD_PLUGINTYPE plugintype, int index);
    FMOD_RESULT setOutputByPlugin(int index);
    FMOD_RESULT getOutputByPlugin(int * index);
    FMOD_RESULT createCodec(FMOD_CODEC_DESCRIPTION * description);
    FMOD_RESULT init(int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT update();
    FMOD_RESULT set3DSettings(float dopplerscale, float distancefactor, float rolloffscale);
    FMOD_RESULT get3DSettings(float * dopplerscale, float * distancefactor, float * rolloffscale);
    FMOD_RESULT set3DNumListeners(int numlisteners);
    FMOD_RESULT get3DNumListeners(int * numlisteners);
    FMOD_RESULT set3DListenerAttributes(int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
    FMOD_RESULT get3DListenerAttributes(int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up);
    FMOD_RESULT setSpeakerPosition(FMOD_SPEAKER speaker, float x, float y);
    FMOD_RESULT getSpeakerPosition(FMOD_SPEAKER speaker, float * x, float * y);
    FMOD_RESULT setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
    FMOD_RESULT getStreamBufferSize(unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype);
    FMOD_RESULT getVersion(unsigned int * version);
    FMOD_RESULT getOutputHandle(void * * handle);
    FMOD_RESULT getChannelsPlaying(int * channels);
    FMOD_RESULT getHardwareChannels(int * num2d, int * num3d, int * total);
    FMOD_RESULT getCPUUsage(float * dsp, float * stream, float * update, float * total);
    FMOD_RESULT getSoundRAM(int * currentalloced, int * maxalloced, int * total);
    FMOD_RESULT getNumCDROMDrives(int * numdrives);
    FMOD_RESULT getCDROMDriveName(int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    FMOD_RESULT createSound(const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, Sound * * sound);
    FMOD_RESULT createStream(const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, Sound * * sound);
    FMOD_RESULT createDSP(FMOD_DSP_DESCRIPTION * description, DSP * * dsp);
    FMOD_RESULT createDSPByType(FMOD_DSP_TYPE type, DSP * * dsp);
    FMOD_RESULT createDSPByIndex(int index, DSP * * dsp);
    FMOD_RESULT createChannelGroup(const char * name, ChannelGroup * * channelgroup);
    FMOD_RESULT playSound(FMOD_CHANNELINDEX channelid, Sound * sound, bool paused, Channel * * channel);
    FMOD_RESULT playDSP(FMOD_CHANNELINDEX channelid, DSP * dsp, bool paused, Channel * * channel);
    FMOD_RESULT getChannel(int channelid, Channel * * channel);
    FMOD_RESULT getMasterChannelGroup(ChannelGroup * * channelgroup);
    FMOD_RESULT setReverbProperties(const FMOD_REVERB_PROPERTIES * prop);
    FMOD_RESULT getReverbProperties(FMOD_REVERB_PROPERTIES * prop);
    FMOD_RESULT getDSPHead(DSP * * dsp);
    FMOD_RESULT addDSP(DSP * dsp);
    FMOD_RESULT lockDSP();
    FMOD_RESULT unlockDSP();
    FMOD_RESULT setRecordDriver(int driver);
    FMOD_RESULT getRecordDriver(int * driver);
    FMOD_RESULT getRecordNumDrivers(int * numdrivers);
    FMOD_RESULT getRecordDriverName(int id, char * name, int namelen);
    FMOD_RESULT getRecordPosition(unsigned int * position);
    FMOD_RESULT recordStart(Sound * sound, bool loop);
    FMOD_RESULT recordStop();
    FMOD_RESULT isRecording(bool * recording);
    FMOD_RESULT createGeometry(int maxpolygons, int maxvertices, Geometry * * geometry);
    FMOD_RESULT setGeometrySettings(float maxworldsize);
    FMOD_RESULT getGeometrySettings(float * maxworldsize);
    FMOD_RESULT loadGeometry(const void * data, int datasize, Geometry * * geometry);
    FMOD_RESULT setNetworkProxy(const char * proxy);
    FMOD_RESULT getNetworkProxy(char * proxy, int proxylen);
    FMOD_RESULT setNetworkTimeout(int timeout);
    FMOD_RESULT getNetworkTimeout(int * timeout);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

struct Sound
{
    Sound();
    FMOD_RESULT release();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority);
    FMOD_RESULT getDefaults(float * frequency, float * volume, float * pan, int * priority);
    FMOD_RESULT setVariations(float frequencyvar, float volumevar, float panvar);
    FMOD_RESULT getVariations(float * frequencyvar, float * volumevar, float * panvar);
    FMOD_RESULT set3DMinMaxDistance(float min, float max);
    FMOD_RESULT get3DMinMaxDistance(float * min, float * max);
    FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    FMOD_RESULT get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume);
    FMOD_RESULT set3DCustomRolloff(FMOD_VECTOR * points, int numpoints);
    FMOD_RESULT get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints);
    FMOD_RESULT setSubSound(int index, Sound * subsound);
    FMOD_RESULT getSubSound(int index, Sound * * subsound);
    FMOD_RESULT setSubSoundSentence(int * subsoundlist, int numsubsounds);
    FMOD_RESULT getName(char * name, int namelen);
    FMOD_RESULT getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype);
    FMOD_RESULT getFormat(FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits);
    FMOD_RESULT getNumSubSounds(int * numsubsounds);
    FMOD_RESULT getNumTags(int * numtags, int * numtagsupdated);
    FMOD_RESULT getTag(const char * name, int index, FMOD_TAG * tag);
    FMOD_RESULT getOpenState(FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, bool * starving);
    FMOD_RESULT readData(void * buffer, unsigned int lenbytes, unsigned int * read);
    FMOD_RESULT seekData(unsigned int pcm);
    FMOD_RESULT getNumSyncPoints(int * numsyncpoints);
    FMOD_RESULT getSyncPoint(int index, FMOD_SYNCPOINT * * point);
    FMOD_RESULT getSyncPointInfo(FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype);
    FMOD_RESULT addSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * point);
    FMOD_RESULT deleteSyncPoint(FMOD_SYNCPOINT * point);
    FMOD_RESULT setMode(FMOD_MODE mode);
    FMOD_RESULT getMode(FMOD_MODE * mode);
    FMOD_RESULT setLoopCount(int loopcount);
    FMOD_RESULT getLoopCount(int * loopcount);
    FMOD_RESULT setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

struct Channel
{
    Channel();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT stop();
    FMOD_RESULT setPaused(bool paused);
    FMOD_RESULT getPaused(bool * paused);
    FMOD_RESULT setVolume(float volume);
    FMOD_RESULT getVolume(float * volume);
    FMOD_RESULT setFrequency(float frequency);
    FMOD_RESULT getFrequency(float * frequency);
    FMOD_RESULT setPan(float pan);
    FMOD_RESULT getPan(float * pan);
    FMOD_RESULT setDelay(unsigned int startdelay, unsigned int enddelay);
    FMOD_RESULT getDelay(unsigned int * startdelay, unsigned int * enddelay);
    FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    FMOD_RESULT getSpeakerMix(float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright);
    FMOD_RESULT setSpeakerLevels(FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT getSpeakerLevels(FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT setMute(bool mute);
    FMOD_RESULT getMute(bool * mute);
    FMOD_RESULT setPriority(int priority);
    FMOD_RESULT getPriority(int * priority);
    FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT setChannelGroup(ChannelGroup * channelgroup);
    FMOD_RESULT getChannelGroup(ChannelGroup * * channelgroup);
    FMOD_RESULT setCallback(FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command);
    FMOD_RESULT set3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
    FMOD_RESULT get3DAttributes(FMOD_VECTOR * pos, FMOD_VECTOR * vel);
    FMOD_RESULT set3DMinMaxDistance(float mindistance, float maxdistance);
    FMOD_RESULT get3DMinMaxDistance(float * mindistance, float * maxdistance);
    FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    FMOD_RESULT get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume);
    FMOD_RESULT set3DConeOrientation(FMOD_VECTOR * orientation);
    FMOD_RESULT get3DConeOrientation(FMOD_VECTOR * orientation);
    FMOD_RESULT set3DCustomRolloff(FMOD_VECTOR * points, int numpoints);
    FMOD_RESULT get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints);
    FMOD_RESULT set3DOcclusion(float directocclusion, float reverbocclusion);
    FMOD_RESULT get3DOcclusion(float * directocclusion, float * reverbocclusion);
    FMOD_RESULT set3DSpread(float angle);
    FMOD_RESULT get3DSpread(float * angle);
    FMOD_RESULT set3DPanLevel(float level);
    FMOD_RESULT get3DPanLevel(float * level);
    FMOD_RESULT set3DDopplerLevel(float level);
    FMOD_RESULT get3DDopplerLevel(float * level);
    FMOD_RESULT getDSPHead(DSP * * dsp);
    FMOD_RESULT addDSP(DSP * dsp);
    FMOD_RESULT isPlaying(bool * isplaying);
    FMOD_RESULT isVirtual(bool * isvirtual);
    FMOD_RESULT getAudibility(float * audibility);
    FMOD_RESULT getCurrentSound(Sound * * sound);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    FMOD_RESULT getIndex(int * index);
    FMOD_RESULT setMode(FMOD_MODE mode);
    FMOD_RESULT getMode(FMOD_MODE * mode);
    FMOD_RESULT setLoopCount(int loopcount);
    FMOD_RESULT getLoopCount(int * loopcount);
    FMOD_RESULT setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

struct ChannelGroup
{
    ChannelGroup();
    FMOD_RESULT release();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT setVolume(float volume);
    FMOD_RESULT getVolume(float * volume);
    FMOD_RESULT setPitch(float pitch);
    FMOD_RESULT getPitch(float * pitch);
    FMOD_RESULT stop();
    FMOD_RESULT overridePaused(bool paused);
    FMOD_RESULT overrideVolume(float volume);
    FMOD_RESULT overrideFrequency(float frequency);
    FMOD_RESULT overridePan(float pan);
    FMOD_RESULT overrideMute(bool mute);
    FMOD_RESULT overrideReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT override3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
    FMOD_RESULT overrideSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    FMOD_RESULT addGroup(ChannelGroup * group);
    FMOD_RESULT getNumGroups(int * numgroups);
    FMOD_RESULT getGroup(int index, ChannelGroup * * group);
    FMOD_RESULT getDSPHead(DSP * * dsp);
    FMOD_RESULT addDSP(DSP * dsp);
    FMOD_RESULT getName(char * name, int namelen);
    FMOD_RESULT getNumChannels(int * numchannels);
    FMOD_RESULT getChannel(int index, Channel * * channel);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

struct DSP
{
    DSP();
    FMOD_RESULT release();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT addInput(DSP * target);
    FMOD_RESULT disconnectFrom(DSP * target);
    FMOD_RESULT disconnectAll(bool inputs, bool outputs);
    FMOD_RESULT remove();
    FMOD_RESULT getNumInputs(int * numinputs);
    FMOD_RESULT getNumOutputs(int * numoutputs);
    FMOD_RESULT getInput(int index, DSP * * input);
    FMOD_RESULT getOutput(int index, DSP * * output);
    FMOD_RESULT setInputMix(int index, float volume);
    FMOD_RESULT getInputMix(int index, float * volume);
    FMOD_RESULT setInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT getInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT setOutputMix(int index, float volume);
    FMOD_RESULT getOutputMix(int index, float * volume);
    FMOD_RESULT setOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT getOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT setActive(bool active);
    FMOD_RESULT getActive(bool * active);
    FMOD_RESULT setBypass(bool bypass);
    FMOD_RESULT getBypass(bool * bypass);
    FMOD_RESULT reset();
    FMOD_RESULT setParameter(int index, float value);
    FMOD_RESULT getParameter(int index, float * value, char * valuestr, int valuestrlen);
    FMOD_RESULT getNumParameters(int * numparams);
    FMOD_RESULT getParameterInfo(int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max);
    FMOD_RESULT showConfigDialog(void * hwnd, bool show);
    FMOD_RESULT getInfo(char * name, unsigned int * version, int * channels, int * configwidth, int * configheight);
    FMOD_RESULT getType(FMOD_DSP_TYPE * type);
    FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority);
    FMOD_RESULT getDefaults(float * frequency, float * volume, float * pan, int * priority);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

struct Geometry
{
    Geometry();
    FMOD_RESULT release();
    FMOD_RESULT addPolygon(float directocclusion, float reverbocclusion, bool doublesided, int numvertices, const FMOD_VECTOR * vertices, int * polygonindex);
    FMOD_RESULT getNumPolygons(int * numpolygons);
    FMOD_RESULT getMaxPolygons(int * maxpolygons, int * maxvertices);
    FMOD_RESULT getPolygonNumVertices(int index, int * numvertices);
    FMOD_RESULT setPolygonVertex(int index, int vertexindex, const FMOD_VECTOR * vertex);
    FMOD_RESULT getPolygonVertex(int index, int vertexindex, FMOD_VECTOR * vertex);
    FMOD_RESULT setPolygonAttributes(int index, float directocclusion, float reverbocclusion, bool doublesided);
    FMOD_RESULT getPolygonAttributes(int index, float * directocclusion, float * reverbocclusion, bool * doublesided);
    FMOD_RESULT setActive(bool active);
    FMOD_RESULT getActive(bool * active);
    FMOD_RESULT setRotation(const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
    FMOD_RESULT getRotation(FMOD_VECTOR * forward, FMOD_VECTOR * up);
    FMOD_RESULT setPosition(const FMOD_VECTOR * position);
    FMOD_RESULT getPosition(FMOD_VECTOR * position);
    FMOD_RESULT setScale(const FMOD_VECTOR * scale);
    FMOD_RESULT getScale(FMOD_VECTOR * scale);
    FMOD_RESULT save(void * data, int * datasize);
    FMOD_RESULT setUserData(void * _userdata);
    FMOD_RESULT getUserData(void * * _userdata);
};

} // namespace FMOD

#endif
