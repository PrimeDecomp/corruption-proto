// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805BCCA8..0x805C11FC (55 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: Leading805BCCA8 validates system-ID/index/generation handle fields and returns SystemI
// channel-array+index*0x13C; all13 public wrappers and SystemI playback call it. Channel
// stop/release805BE878 and speaker-level allocation805BF364 directly use fmod_channeli.cpp
// lines681/667/1167. Same object layout/system+34,voice count+38,voice slots+3C,handle+60 and
// parameter block+94 onward closes leading recycle/state/spatial helpers, middle
// constructors/reset/start, complete channel control operations, group/DSP/loop methods and final
// destructor805C1164. Destructor restores vtable806E2C20 and nested list base tables, ending
// exactly805C11FC. Following805C11FC is already assigned channelpool.cpp constructor, separately
// asserted by its retained create/free functions. No channeli helper is dropped merely because its
// body is short or generic. Preserve every retained stub, emitted helper and adjustor thunk; full
// inventory and inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_channeli.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_channel_real.h"
#include "fmod_channelgroupi.h"
#include "fmod_dspi.h"
#include "fmod_soundi.h"
#include "fmod_systemi.h"

static FMOD_RESULT FMOD_CHECKFLOAT(float value)
{
}

namespace FMOD {

FMOD_RESULT ChannelI::validate(Channel * channel, ChannelI * * channeli)
{
}

FMOD_RESULT ChannelI::returnToFreeList()
{
}

FMOD_RESULT ChannelI::setFrequency(float frequency)
{
}

FMOD_RESULT ChannelI::setPan(float pan, bool calldriver)
{
}

FMOD_RESULT ChannelI::referenceStamp(bool newstamp)
{
}

FMOD_RESULT ChannelI::getAudibility(float * audibility)
{
}

FMOD_RESULT ChannelI::updatePosition()
{
}

FMOD_RESULT ChannelI::setVolume(float volume)
{
}

FMOD_RESULT ChannelI::getMode(FMOD_MODE * mode)
{
}

FMOD_RESULT ChannelI::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelI::getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT ChannelI::getCurrentSound(SoundI * * sound)
{
}

FMOD_RESULT ChannelI::getLoopCount(int * loopcount)
{
}

FMOD_RESULT ChannelI::getMute(bool * mute)
{
}

FMOD_RESULT ChannelI::getPaused(bool * paused)
{
}

FMOD_RESULT ChannelI::getDelay(unsigned int * startdelay, unsigned int * enddelay)
{
}

FMOD_RESULT ChannelI::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelI::getDSPHead(DSPI * * dsp)
{
}

FMOD_RESULT ChannelI::getChannelInfo(FMOD_CHANNEL_INFO * info)
{
}

FMOD_RESULT ChannelI::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, bool calldriver)
{
}

FMOD_RESULT ChannelI::setDelay(unsigned int startdelay, unsigned int enddelay)
{
}

FMOD_RESULT ChannelI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT ChannelI::setLoopCount(int loopcount)
{
}

FMOD_RESULT ChannelI::setMute(bool mute)
{
}

FMOD_RESULT ChannelI::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelI::isVirtual(bool * isvirtual)
{
}

FMOD_RESULT ChannelI::getRealChannel(ChannelReal * * realchan, int * subchannels)
{
}

FMOD_RESULT ChannelI::calcVolumeAndPitchFor3D()
{
}

FMOD_RESULT ChannelI::init()
{
}

ChannelI::ChannelI()
{
}

ChannelI::ChannelI(int index, SystemI * system)
{
}

FMOD_RESULT ChannelI::getSystemObject(System * * system)
{
}

FMOD_RESULT ChannelI::alloc(SoundI * sound, bool reset)
{
}

FMOD_RESULT ChannelI::setPaused(bool paused)
{
}

FMOD_RESULT ChannelI::start()
{
}

FMOD_RESULT ChannelI::alloc(DSPI * dsp, bool reset)
{
}

FMOD_RESULT ChannelI::updateSyncPoints(bool seeking)
{
}

FMOD_RESULT ChannelI::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelI::set3DOcclusionInternal(float direct, float reverb, bool resettarget)
{
}

FMOD_RESULT ChannelI::update(int delta, bool callrealupdate)
{
}

FMOD_RESULT ChannelI::set3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel)
{
}

FMOD_RESULT ChannelI::updateStream()
{
}

FMOD_RESULT ChannelI::getVolume(float * volume)
{
}

FMOD_RESULT ChannelI::getFrequency(float * frequency)
{
}

FMOD_RESULT ChannelI::getPan(float * pan)
{
}

FMOD_RESULT ChannelI::getSpeakerMix(float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright)
{
}

FMOD_RESULT ChannelI::setSpeakerLevels(int speaker, float * levels, int numlevels, bool calldriver)
{
}

FMOD_RESULT ChannelI::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT ChannelI::setDefaults()
{
}

FMOD_RESULT ChannelI::play(DSPI * dsp, bool paused, bool reset)
{
}

FMOD_RESULT ChannelI::play(SoundI * sound, bool paused, bool reset)
{
}

FMOD_RESULT ChannelI::getSpeakerLevels(int speaker, float * levels, int numlevels)
{
}

FMOD_RESULT ChannelI::setChannelGroupInternal(ChannelGroupI * channelgroup, bool resetattributes)
{
}

FMOD_RESULT ChannelI::stopEx(bool refstamp, bool updatelist, bool resetcallbacks, bool updateflags, bool callendcallback, bool resetchannelgroup)
{
}

FMOD_RESULT ChannelI::stop()
{
}

FMOD_RESULT ChannelI::setPriority(int priority)
{
}

FMOD_RESULT ChannelI::getPriority(int * priority)
{
}

FMOD_RESULT ChannelI::get3DAttributes(FMOD_VECTOR * pos, FMOD_VECTOR * vel)
{
}

FMOD_RESULT ChannelI::set3DMinMaxDistance(float mindistance, float maxdistance)
{
}

FMOD_RESULT ChannelI::get3DMinMaxDistance(float * mindistance, float * maxdistance)
{
}

FMOD_RESULT ChannelI::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT ChannelI::get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
}

FMOD_RESULT ChannelI::set3DConeOrientation(FMOD_VECTOR * orientation)
{
}

FMOD_RESULT ChannelI::get3DConeOrientation(FMOD_VECTOR * orientation)
{
}

FMOD_RESULT ChannelI::set3DCustomRolloff(FMOD_VECTOR * points, int numpoints)
{
}

FMOD_RESULT ChannelI::get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints)
{
}

FMOD_RESULT ChannelI::set3DOcclusion(float direct, float reverb)
{
}

FMOD_RESULT ChannelI::get3DOcclusion(float * directOcclusion, float * reverbOcclusion)
{
}

FMOD_RESULT ChannelI::set3DSpread(float angle)
{
}

FMOD_RESULT ChannelI::get3DSpread(float * angle)
{
}

FMOD_RESULT ChannelI::set3DPanLevel(float level)
{
}

FMOD_RESULT ChannelI::get3DPanLevel(float * level)
{
}

FMOD_RESULT ChannelI::set3DDopplerLevel(float level)
{
}

FMOD_RESULT ChannelI::get3DDopplerLevel(float * level)
{
}

FMOD_RESULT ChannelI::setChannelGroup(ChannelGroupI * channelgroup)
{
}

FMOD_RESULT ChannelI::getChannelGroup(ChannelGroupI * * channelgroup)
{
}

FMOD_RESULT ChannelI::isPlaying(bool * isplaying)
{
}

FMOD_RESULT ChannelI::getCurrentDSP(DSPI * * dsp)
{
}

FMOD_RESULT ChannelI::getSpectrum(float * spectrumarray, int numvalues, int channel, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT ChannelI::getWaveData(float * wavearray, int numvalues, int channel)
{
}

FMOD_RESULT ChannelI::getIndex(int * index)
{
}

FMOD_RESULT ChannelI::setCallback(FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command)
{
}

FMOD_RESULT ChannelI::addDSPChain(DSPI * dsp)
{
}

FMOD_RESULT ChannelI::setChannelInfo(FMOD_CHANNEL_INFO * info)
{
}

FMOD_RESULT ChannelI::addDSP(DSPI * dsp)
{
}

FMOD_RESULT ChannelI::setUserData(void * userdata)
{
}

FMOD_RESULT ChannelI::getUserData(void * * userdata)
{
}

} // namespace FMOD
