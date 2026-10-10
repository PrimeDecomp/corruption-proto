// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805B8A3C..0x805BAB64 (21 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Ctor805B8A3C installs DSP voice table806E28D4 after shared/intermediate base tables,
// and initializes+78..+8C DSP handles. Init805B8A9C creates FMOD Channel DSPHead, SubChannel
// DSPHead and WaveTable units;805B9440 creates Resampler and graph connections. All following
// overrides operate on those graph nodes, volume/frequency/mix/position/status/spectrum/waveform
// state. Final805BAB5C is an8-byte -0x5C adjustor branch to descriptor/list-node destructor805B8DC4
// and remains with its emitting family. Next805BAB64 installs distinct aggregate table806E2A4C and
// intrusive child-list state. Basename inferred from this closed DSP-specific voice implementation.
// Preserve every retained stub, emitted helper and adjustor thunk; full inventory and inlining
// uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_channel_software.h"
#include "fmod.h"
#include "fmod_channelgroupi.h"
#include "fmod_dspi.h"
#include "fmod_outputi.h"
#include "fmod_reverbi.h"
#include "fmod_systemi.h"

namespace FMOD {

ChannelSoftware::ChannelSoftware()
{
}

FMOD_RESULT ChannelSoftware::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
}

FMOD_RESULT ChannelSoftware::close()
{
}

FMOD_RESULT ChannelSoftware::setupDSPCodec(DSPI * dsp)
{
}

FMOD_RESULT ChannelSoftware::start()
{
}

FMOD_RESULT ChannelSoftware::stop(bool force, bool updateflags)
{
}

FMOD_RESULT ChannelSoftware::setPaused(bool paused)
{
}

FMOD_RESULT ChannelSoftware::addToReverbs()
{
}

FMOD_RESULT ChannelSoftware::alloc(DSPI * dsp)
{
}

FMOD_RESULT ChannelSoftware::alloc()
{
}

FMOD_RESULT ChannelSoftware::getMultiReverbProperties(ReverbI * reverb, FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelSoftware::updateReverbMix(ReverbI * reverb, float volume)
{
}

FMOD_RESULT ChannelSoftware::setMultiReverbProperties(ReverbI * reverb, const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelSoftware::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelSoftware::updateDirectMix(float volume)
{
}

FMOD_RESULT ChannelSoftware::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelSoftware::set3DOcclusion(float directOcclusion, float reverbOcclusion)
{
}

FMOD_RESULT ChannelSoftware::setVolume(float volume)
{
}

FMOD_RESULT ChannelSoftware::setFrequency(float frequency)
{
}

FMOD_RESULT ChannelSoftware::setPan(float pan, float fbpan)
{
}

FMOD_RESULT ChannelSoftware::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
}

FMOD_RESULT ChannelSoftware::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
}

FMOD_RESULT ChannelSoftware::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelSoftware::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelSoftware::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
}

FMOD_RESULT ChannelSoftware::setLoopCount(int loopcount)
{
}

FMOD_RESULT ChannelSoftware::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT ChannelSoftware::isPlaying(bool * isplaying, bool includethreadlatency)
{
}

FMOD_RESULT ChannelSoftware::getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT ChannelSoftware::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT ChannelSoftware::getDSPHead(DSPI * * dsp)
{
}

FMOD_RESULT ChannelSoftware::moveChannelGroup(ChannelGroupI * oldchannelgroup, ChannelGroupI * newchannelgroup)
{
}

} // namespace FMOD
