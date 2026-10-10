// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8061A228..0x8061A6E0 (13 native functions).
// Source identity: inferred public-system basename. Extent confidence: medium-high.
// Complete native inventory retained, including callbacks and emitted helpers.
// Function bodies are empty placeholders from the 4.06 reference inventory.
// 0x8061A228 +0x38: retained native; no unsupported symbol identity assigned
// 0x8061A260 +0x78: retained native; no unsupported symbol identity assigned
// 0x8061A2D8 +0x48: retained native; no unsupported symbol identity assigned
// 0x8061A320 +0x68: retained native; no unsupported symbol identity assigned
// 0x8061A388 +0x38: retained native; no unsupported symbol identity assigned
// 0x8061A3C0 +0x68: retained native; no unsupported symbol identity assigned
// 0x8061A428 +0x48: retained native; no unsupported symbol identity assigned
// 0x8061A470 +0x68: retained native; no unsupported symbol identity assigned
// 0x8061A4D8 +0x48: retained native; no unsupported symbol identity assigned
// 0x8061A520 +0x78: retained native; no unsupported symbol identity assigned
// 0x8061A598 +0x78: retained native; no unsupported symbol identity assigned
// 0x8061A610 +0x78: retained native; no unsupported symbol identity assigned
// 0x8061A688 +0x58: retained native; no unsupported symbol identity assigned

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_codec.h"
#include "fmod_dsp.h"

namespace FMOD {

FMOD_RESULT System::release()
{
}

FMOD_RESULT System::setOutput(FMOD_OUTPUTTYPE output)
{
}

FMOD_RESULT System::getOutput(FMOD_OUTPUTTYPE * output)
{
}

FMOD_RESULT System::getNumDrivers(int * numdrivers)
{
}

FMOD_RESULT System::getDriverName(int id, char * name, int namelen)
{
}

FMOD_RESULT System::getDriverCaps(int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode)
{
}

FMOD_RESULT System::setDriver(int driver)
{
}

FMOD_RESULT System::getDriver(int * driver)
{
}

FMOD_RESULT System::setHardwareChannels(int min2d, int max2d, int min3d, int max3d)
{
}

FMOD_RESULT System::setSoftwareChannels(int numsoftwarechannels)
{
}

FMOD_RESULT System::getSoftwareChannels(int * numsoftwarechannels)
{
}

FMOD_RESULT System::setSoftwareFormat(int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod)
{
}

FMOD_RESULT System::getSoftwareFormat(int * samplerate, FMOD_SOUND_FORMAT * format, int * numoutputchannels, int * maxinputchannels, FMOD_DSP_RESAMPLER * resamplemethod, int * bits)
{
}

FMOD_RESULT System::setDSPBufferSize(unsigned int bufferlength, int numbuffers)
{
}

FMOD_RESULT System::getDSPBufferSize(unsigned int * bufferlength, int * numbuffers)
{
}

FMOD_RESULT System::setFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blocksize)
{
}

FMOD_RESULT System::attachFileSystem(FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek)
{
}

FMOD_RESULT System::setAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT System::getAdvancedSettings(FMOD_ADVANCEDSETTINGS * settings)
{
}

FMOD_RESULT System::setSpeakerMode(FMOD_SPEAKERMODE speakermode)
{
}

FMOD_RESULT System::getSpeakerMode(FMOD_SPEAKERMODE * speakermode)
{
}

FMOD_RESULT System::setPluginPath(const char * path)
{
}

FMOD_RESULT System::loadPlugin(const char * filename, FMOD_PLUGINTYPE * plugintype, int * index)
{
}

FMOD_RESULT System::getNumPlugins(FMOD_PLUGINTYPE plugintype, int * numplugins)
{
}

FMOD_RESULT System::getPluginInfo(FMOD_PLUGINTYPE plugintype, int index, char * name, int namelen, unsigned int * version)
{
}

FMOD_RESULT System::unloadPlugin(FMOD_PLUGINTYPE plugintype, int index)
{
}

FMOD_RESULT System::setOutputByPlugin(int index)
{
}

FMOD_RESULT System::getOutputByPlugin(int * index)
{
}

FMOD_RESULT System::createCodec(FMOD_CODEC_DESCRIPTION * description)
{
}

FMOD_RESULT System::init(int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata)
{
}

FMOD_RESULT System::close()
{
}

FMOD_RESULT System::update()
{
}

FMOD_RESULT System::set3DSettings(float dopplerscale, float distancefactor, float rolloffscale)
{
}

FMOD_RESULT System::get3DSettings(float * dopplerscale, float * distancefactor, float * rolloffscale)
{
}

FMOD_RESULT System::set3DNumListeners(int numlisteners)
{
}

FMOD_RESULT System::get3DNumListeners(int * numlisteners)
{
}

FMOD_RESULT System::set3DListenerAttributes(int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
{
}

FMOD_RESULT System::get3DListenerAttributes(int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up)
{
}

FMOD_RESULT System::setSpeakerPosition(FMOD_SPEAKER speaker, float x, float y)
{
}

FMOD_RESULT System::getSpeakerPosition(FMOD_SPEAKER speaker, float * x, float * y)
{
}

FMOD_RESULT System::setStreamBufferSize(unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype)
{
}

FMOD_RESULT System::getStreamBufferSize(unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype)
{
}

FMOD_RESULT System::getVersion(unsigned int * version)
{
}

FMOD_RESULT System::getOutputHandle(void * * handle)
{
}

FMOD_RESULT System::getChannelsPlaying(int * channels)
{
}

FMOD_RESULT System::getHardwareChannels(int * num2d, int * num3d, int * total)
{
}

FMOD_RESULT System::getCPUUsage(float * dsp, float * stream, float * update, float * total)
{
}

FMOD_RESULT System::getSoundRAM(int * currentalloced, int * maxalloced, int * total)
{
}

FMOD_RESULT System::getNumCDROMDrives(int * numdrives)
{
}

FMOD_RESULT System::getCDROMDriveName(int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen)
{
}

FMOD_RESULT System::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT System::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT System::createSound(const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, Sound * * sound)
{
}

FMOD_RESULT System::createStream(const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, Sound * * sound)
{
}

FMOD_RESULT System::createDSP(FMOD_DSP_DESCRIPTION * description, DSP * * dsp)
{
}

FMOD_RESULT System::createDSPByType(FMOD_DSP_TYPE type, DSP * * dsp)
{
}

FMOD_RESULT System::createDSPByIndex(int index, DSP * * dsp)
{
}

FMOD_RESULT System::createChannelGroup(const char * name, ChannelGroup * * channelgroup)
{
}

FMOD_RESULT System::playSound(FMOD_CHANNELINDEX channelid, Sound * sound, bool paused, Channel * * channel)
{
}

FMOD_RESULT System::playDSP(FMOD_CHANNELINDEX channelid, DSP * dsp, bool paused, Channel * * channel)
{
}

FMOD_RESULT System::getChannel(int channelid, Channel * * channel)
{
}

FMOD_RESULT System::getMasterChannelGroup(ChannelGroup * * channelgroup)
{
}

FMOD_RESULT System::setReverbProperties(const FMOD_REVERB_PROPERTIES * prop)
{
}

FMOD_RESULT System::getReverbProperties(FMOD_REVERB_PROPERTIES * prop)
{
}

FMOD_RESULT System::getDSPHead(DSP * * dsp)
{
}

FMOD_RESULT System::addDSP(DSP * dsp)
{
}

FMOD_RESULT System::lockDSP()
{
}

FMOD_RESULT System::unlockDSP()
{
}

FMOD_RESULT System::setRecordDriver(int driver)
{
}

FMOD_RESULT System::getRecordDriver(int * driver)
{
}

FMOD_RESULT System::getRecordNumDrivers(int * numdrivers)
{
}

FMOD_RESULT System::getRecordDriverName(int id, char * name, int namelen)
{
}

FMOD_RESULT System::getRecordPosition(unsigned int * position)
{
}

FMOD_RESULT System::recordStart(Sound * sound, bool loop)
{
}

FMOD_RESULT System::recordStop()
{
}

FMOD_RESULT System::isRecording(bool * recording)
{
}

FMOD_RESULT System::createGeometry(int maxpolygons, int maxvertices, Geometry * * geometry)
{
}

FMOD_RESULT System::setGeometrySettings(float maxworldsize)
{
}

FMOD_RESULT System::getGeometrySettings(float * maxworldsize)
{
}

FMOD_RESULT System::loadGeometry(const void * data, int datasize, Geometry * * geometry)
{
}

FMOD_RESULT System::setNetworkProxy(const char * proxy)
{
}

FMOD_RESULT System::getNetworkProxy(char * proxy, int proxylen)
{
}

FMOD_RESULT System::setNetworkTimeout(int timeout)
{
}

FMOD_RESULT System::getNetworkTimeout(int * timeout)
{
}

FMOD_RESULT System::setUserData(void * _userdata)
{
}

FMOD_RESULT System::getUserData(void * * _userdata)
{
}

} // namespace FMOD
