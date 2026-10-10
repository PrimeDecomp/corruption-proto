// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805B67C0..0x805B6C3C (13 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: All13 natives validate opaque channel handles through805BCCA8, then forward to ChannelI
// stop/pause/volume/frequency/speaker-mix/mute/3D/status/sound operations. Failure paths explicitly
// clear selected outputs, preserving nontrivial wrappers. First805B67C0 calls stop805BE844;
// last805B6BE4 calls current-sound805C0330. Next805B6C3C constructs a low-level voice, writes
// vtable806E25F0 at+74 and never validates an opaque handle. Basename inferred from the complete
// public wrapper family, not an original assertion. Preserve every retained stub, emitted helper
// and adjustor thunk; full inventory and inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod.h"
#include "fmod.hpp"

namespace FMOD {

FMOD_RESULT Channel::getSystemObject(System * * system)
{
}

FMOD_RESULT Channel::stop()
{
}

FMOD_RESULT Channel::setPaused(bool paused)
{
}

FMOD_RESULT Channel::getPaused(bool * paused)
{
}

FMOD_RESULT Channel::setVolume(float volume)
{
}

FMOD_RESULT Channel::getVolume(float * volume)
{
}

FMOD_RESULT Channel::setFrequency(float frequency)
{
}

FMOD_RESULT Channel::getFrequency(float * frequency)
{
}

FMOD_RESULT Channel::setPan(float pan)
{
}

FMOD_RESULT Channel::getPan(float * pan)
{
}

FMOD_RESULT Channel::setDelay(unsigned int startdelay, unsigned int enddelay)
{
}

FMOD_RESULT Channel::getDelay(unsigned int * startdelay, unsigned int * enddelay)
{
}

FMOD_RESULT Channel::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
}

FMOD_RESULT Channel::getSpeakerMix(float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright)
{
}

FMOD_RESULT Channel::setSpeakerLevels(FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT Channel::getSpeakerLevels(FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT Channel::setMute(bool mute)
{
}

FMOD_RESULT Channel::getMute(bool * mute)
{
}

FMOD_RESULT Channel::setPriority(int priority)
{
}

FMOD_RESULT Channel::getPriority(int * priority)
{
}

FMOD_RESULT Channel::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT Channel::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT Channel::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT Channel::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT Channel::setChannelGroup(ChannelGroup * channelgroup)
{
}

FMOD_RESULT Channel::getChannelGroup(ChannelGroup * * channelgroup)
{
}

FMOD_RESULT Channel::setCallback(FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command)
{
}

FMOD_RESULT Channel::set3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel)
{
}

FMOD_RESULT Channel::get3DAttributes(FMOD_VECTOR * pos, FMOD_VECTOR * vel)
{
}

FMOD_RESULT Channel::set3DMinMaxDistance(float mindistance, float maxdistance)
{
}

FMOD_RESULT Channel::get3DMinMaxDistance(float * mindistance, float * maxdistance)
{
}

FMOD_RESULT Channel::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT Channel::get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume)
{
}

FMOD_RESULT Channel::set3DConeOrientation(FMOD_VECTOR * orientation)
{
}

FMOD_RESULT Channel::get3DConeOrientation(FMOD_VECTOR * orientation)
{
}

FMOD_RESULT Channel::set3DCustomRolloff(FMOD_VECTOR * points, int numpoints)
{
}

FMOD_RESULT Channel::get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints)
{
}

FMOD_RESULT Channel::set3DOcclusion(float directocclusion, float reverbocclusion)
{
}

FMOD_RESULT Channel::get3DOcclusion(float * directocclusion, float * reverbocclusion)
{
}

FMOD_RESULT Channel::set3DSpread(float angle)
{
}

FMOD_RESULT Channel::get3DSpread(float * angle)
{
}

FMOD_RESULT Channel::set3DPanLevel(float level)
{
}

FMOD_RESULT Channel::get3DPanLevel(float * level)
{
}

FMOD_RESULT Channel::set3DDopplerLevel(float level)
{
}

FMOD_RESULT Channel::get3DDopplerLevel(float * level)
{
}

FMOD_RESULT Channel::getDSPHead(DSP * * dsp)
{
}

FMOD_RESULT Channel::addDSP(DSP * dsp)
{
}

FMOD_RESULT Channel::isPlaying(bool * isplaying)
{
}

FMOD_RESULT Channel::isVirtual(bool * isvirtual)
{
}

FMOD_RESULT Channel::getAudibility(float * audibility)
{
}

FMOD_RESULT Channel::getCurrentSound(Sound * * sound)
{
}

FMOD_RESULT Channel::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT Channel::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT Channel::getIndex(int * index)
{
}

FMOD_RESULT Channel::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT Channel::getMode(FMOD_MODE * mode)
{
}

FMOD_RESULT Channel::setLoopCount(int loopcount)
{
}

FMOD_RESULT Channel::getLoopCount(int * loopcount)
{
}

FMOD_RESULT Channel::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT Channel::getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
}

FMOD_RESULT Channel::setUserData(void * _userdata)
{
}

FMOD_RESULT Channel::getUserData(void * * _userdata)
{
}

} // namespace FMOD
