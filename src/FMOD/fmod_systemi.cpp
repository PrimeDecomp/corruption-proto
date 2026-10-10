// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8061A6E0..0x80621208 (48 native functions).
// Source identity: asserted original basename. Extent confidence: medium-high.
// Complete native inventory retained, including callbacks and emitted helpers.
// Function bodies are empty placeholders from the 4.06 reference inventory.
// 0x8061A6E0 +0x11C: fixed-point azimuth helper, called by system3D update61FC4C
// 0x8061A7FC +0x54: system registry lookup by instance identifier
// 0x8061A850 +0x68: system handle validation used by public wrapper TU
// 0x8061A8B8 +0x68: retained native; no unsupported symbol identity assigned
// 0x8061A920 +0x8C: retained native; no unsupported symbol identity assigned
// 0x8061A9AC +0xC54: retained native; no unsupported symbol identity assigned
// 0x8061B600 +0x72C: retained native; no unsupported symbol identity assigned
// 0x8061BD2C +0x494: retained native; no unsupported symbol identity assigned
// 0x8061C1C0 +0x1EE8: retained native; no unsupported symbol identity assigned
// 0x8061E0A8 +0x3A0: retained native; no unsupported symbol identity assigned
// 0x8061E448 +0x11C: retained native; no unsupported symbol identity assigned
// 0x8061E564 +0x50: retained native; no unsupported symbol identity assigned
// 0x8061E5B4 +0x478: retained native; no unsupported symbol identity assigned
// 0x8061EA2C +0x9C: retained native; no unsupported symbol identity assigned
// 0x8061EAC8 +0x154: retained native; no unsupported symbol identity assigned
// 0x8061EC1C +0x54: retained native; no unsupported symbol identity assigned
// 0x8061EC70 +0xE4: retained native; no unsupported symbol identity assigned
// 0x8061ED54 +0x30: retained native; no unsupported symbol identity assigned
// 0x8061ED84 +0xAC: retained native; no unsupported symbol identity assigned
// 0x8061EE30 +0x834: retained native; no unsupported symbol identity assigned
// 0x8061F664 +0x24: retained native; no unsupported symbol identity assigned
// 0x8061F688 +0x3F8: retained native; no unsupported symbol identity assigned
// 0x8061FA80 +0x1CC: retained native; no unsupported symbol identity assigned
// 0x8061FC4C +0x84: retained native; no unsupported symbol identity assigned
// 0x8061FCD0 +0x4C: retained native; no unsupported symbol identity assigned
// 0x8061FD1C +0x38: retained native; no unsupported symbol identity assigned
// 0x8061FD54 +0x24: retained native; no unsupported symbol identity assigned
// 0x8061FD78 +0x20: retained native; no unsupported symbol identity assigned
// 0x8061FD98 +0x2F4: retained native; no unsupported symbol identity assigned
// 0x8062008C +0x3C: retained native; no unsupported symbol identity assigned
// 0x806200C8 +0xD8: retained native; no unsupported symbol identity assigned
// 0x806201A0 +0x2A8: retained native; no unsupported symbol identity assigned
// 0x80620448 +0x1B8: retained native; no unsupported symbol identity assigned
// 0x80620600 +0x1C4: retained native; no unsupported symbol identity assigned
// 0x806207C4 +0x124: retained native; no unsupported symbol identity assigned
// 0x806208E8 +0x26C: retained native; no unsupported symbol identity assigned
// 0x80620B54 +0x190: retained native; no unsupported symbol identity assigned
// 0x80620CE4 +0x4C: retained native; no unsupported symbol identity assigned
// 0x80620D30 +0x20: retained native; no unsupported symbol identity assigned
// 0x80620D50 +0x28: retained native; no unsupported symbol identity assigned
// 0x80620D78 +0x28: retained native; no unsupported symbol identity assigned
// 0x80620DA0 +0x8: retained native; no unsupported symbol identity assigned
// 0x80620DA8 +0x1D4: retained native; no unsupported symbol identity assigned
// 0x80620F7C +0x80: retained native; no unsupported symbol identity assigned
// 0x80620FFC +0x3C: retained native; no unsupported symbol identity assigned
// 0x80621038 +0x90: retained native; no unsupported symbol identity assigned
// 0x806210C8 +0xCC: retained native; no unsupported symbol identity assigned
// 0x80621194 +0x74: registered system static initializer; calls shared thread/profiling
// constructors 0x80621208 +0xAC: thread entry callback loop with semaphore wait and optional yield

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_systemi.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_codec.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_file.h"
#include "fmod_geometryi.h"
#include "fmod_globals.h"
#include "fmod_linkedlist.h"
#include "fmod_listener.h"
#include "fmod_os_misc.h"
#include "fmod_reverbi.h"
#include "fmod_sound_sample.h"
#include "fmod_soundi.h"
#include "fmod_thread.h"
#include "fmod_time.h"

namespace FMOD {

FMOD_OS_CRITICALSECTION * SystemI::gSoundListCrit;
TimeStamp SystemI::gStreamTimeStamp;
FMOD_OS_CRITICALSECTION * SystemI::gStreamListCrit;
FMOD_OS_CRITICALSECTION * SystemI::gStreamFillCrit;
FMOD_OS_CRITICALSECTION * SystemI::gStreamCrit;
bool SystemI::gStreamThreadActive;
Thread SystemI::gStreamThread;
LinkedListNode SystemI::gStreamHead;

} // namespace FMOD

static int FMOD_Cart2Angle(int y, int x)
{
}

static FMOD_RESULT FMOD_CHECKFLOAT(float value)
{
}

namespace FMOD {

FMOD_RESULT SystemI::getInstance(unsigned int id, SystemI * * sys)
{
}

FMOD_RESULT SystemI::validate(System * system, SystemI * * systemi)
{
}

FMOD_RESULT SystemI::updateStreams()
{
}

void SystemI::streamThread(void * data)
{
}

FMOD_RESULT SystemI::updateChannels(int delta)
{
}

FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, DSPI * dsp, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::setUpPlugins()
{
}

FMOD_RESULT SystemI::sortSpeakerList()
{
}

FMOD_RESULT SystemI::allocDSPCodec(FMOD_SOUND_FORMAT format, DSPI * * dsp)
{
}

FMOD_RESULT SystemI::setSpeakerPosition(FMOD_SPEAKER speaker, float x, float y)
{
}

FMOD_RESULT SystemI::setSpeakerMode(FMOD_SPEAKERMODE speakermode)
{
}

SystemI::SystemI()
{
}

FMOD_RESULT SystemI::setOutput(FMOD_OUTPUTTYPE outputtype)
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

FMOD_RESULT SystemI::setHardwareChannels(int min2d, int max2d, int min3d, int max3d)
{
}

FMOD_RESULT SystemI::getHardwareChannels(int * num2d, int * num3d, int * total)
{
}

FMOD_RESULT SystemI::createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample_out)
{
}

FMOD_RESULT SystemI::createSoundInternal(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
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

FMOD_RESULT SystemI::getDSPBufferSize(unsigned int * bufferlength, int * numbuffers)
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

FMOD_RESULT SystemI::recordStop()
{
}

FMOD_RESULT SystemI::update()
{
}

FMOD_RESULT SystemI::closeEx(bool calledfrominit)
{
}

FMOD_RESULT SystemI::close()
{
}

FMOD_RESULT SystemI::release()
{
}

FMOD_RESULT SystemI::updateFinished()
{
}

FMOD_RESULT SystemI::getSpeakerPosition(FMOD_SPEAKER speaker, float * x, float * y)
{
}

FMOD_RESULT SystemI::set3DSettings(float dopplerscale, float distancescale, float rolloffscale)
{
}

FMOD_RESULT SystemI::get3DSettings(float * dopplerscale, float * distancescale, float * rolloffscale)
{
}

FMOD_RESULT SystemI::set3DNumListeners(int numlisteners)
{
}

FMOD_RESULT SystemI::get3DNumListeners(int * numlisteners)
{
}

FMOD_RESULT SystemI::set3DListenerAttributes(int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
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

FMOD_RESULT SystemI::getChannelsPlaying(int * channels)
{
}

FMOD_RESULT SystemI::getCPUUsage(float * dsp, float * stream, float * update, float * total)
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

FMOD_RESULT SystemI::createSound(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
{
}

FMOD_RESULT SystemI::createStream(const char * name_or_data, FMOD_MODE mode_in, FMOD_CREATESOUNDEXINFO * exinfo, SoundI * * sound)
{
}

FMOD_RESULT SystemI::createCodec(FMOD_CODEC_DESCRIPTION * description)
{
}

FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION_EX * description, DSPI * * dsp, bool allocate)
{
}

FMOD_RESULT SystemI::createDSP(FMOD_DSP_DESCRIPTION * description, DSPI * * dsp)
{
}

FMOD_RESULT SystemI::createDSPByType(FMOD_DSP_TYPE type, DSPI * * dsp)
{
}

FMOD_RESULT SystemI::createDSPByIndex(int index, DSPI * * dsp)
{
}

FMOD_RESULT SystemI::createChannelGroup(const char * name, ChannelGroupI * * channelgroup)
{
}

FMOD_RESULT SystemI::playDSP(FMOD_CHANNELINDEX channelid, DSPI * dsp, bool paused, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::getChannel(int id, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::getMasterChannelGroup(ChannelGroupI * * channelgroup)
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

FMOD_RESULT SystemI::init(int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata)
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

FMOD_RESULT SystemI::lockDSP()
{
}

FMOD_RESULT SystemI::unlockDSP()
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

FMOD_RESULT SystemI::stopSound(SoundI * sound)
{
}

FMOD_RESULT SystemI::findChannel(FMOD_CHANNELINDEX id, SoundI * sound, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::playSound(FMOD_CHANNELINDEX channelid, SoundI * sound, bool paused, ChannelI * * channel)
{
}

FMOD_RESULT SystemI::stopDSP(DSPI * dsp)
{
}

FMOD_RESULT SystemI::getListenerObject(int listener, Listener * * listenerobject)
{
}

FMOD_RESULT SystemI::flushDSPConnectionRequests(bool calledfrommainthread)
{
}

FMOD_RESULT SystemI::createFile(File * * file)
{
}

FMOD_RESULT SystemI::getGlobals(Global * * global)
{
}

} // namespace FMOD
