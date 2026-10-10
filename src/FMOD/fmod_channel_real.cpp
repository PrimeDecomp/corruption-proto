// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805B6E70..0x805B8A3C (37 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Base ctor805B6E70 initializes the common0x78-byte voice layout and vtable806E270C,
// called by emulated805B6C3C, DSP805B8A3C, aggregate805BAB64 and platform80621870 constructors.
// Full vtable slot inventory binds retained setters/getters/stubs/conversion methods through
// mode805B77CC. Integer-angle helper805B7998 is called only by large spatial update805B7AB4;
// vtable806E27A0 replaces base slot+8 with that update and otherwise shares base slots, and the
// following DSP ctor installs that intermediate table before its own table. Keep both spatial
// helper/update in this coherent common/real voice family; a separate original spatial TU remains
// possible, so basename and historical extent are inferred. Next805B8A3C introduces DSP-specific
// fields/vtable and node creation, establishing a concrete implementation transition. Preserve
// every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty
// are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_channel_real.h"
#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_outputi.h"
#include "fmod_systemi.h"

namespace FMOD {

ChannelReal::ChannelReal()
{
}

FMOD_RESULT ChannelReal::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
}

FMOD_RESULT ChannelReal::close()
{
}

FMOD_RESULT ChannelReal::alloc()
{
}

FMOD_RESULT ChannelReal::alloc(DSPI * dsp)
{
}

FMOD_RESULT ChannelReal::set2DFreqVolumePanFor3D()
{
}

FMOD_RESULT ChannelReal::update(int delta)
{
}

FMOD_RESULT ChannelReal::updateStream()
{
}

FMOD_RESULT ChannelReal::start()
{
}

FMOD_RESULT ChannelReal::stop(bool force, bool updateflags)
{
}

FMOD_RESULT ChannelReal::setPaused(bool paused)
{
}

FMOD_RESULT ChannelReal::getPaused(bool * paused)
{
}

FMOD_RESULT ChannelReal::setVolume(float volume)
{
}

FMOD_RESULT ChannelReal::setFrequency(float frequency)
{
}

FMOD_RESULT ChannelReal::setPan(float pan, float fbpan)
{
}

FMOD_RESULT ChannelReal::setDelay(unsigned int startdelay, unsigned int enddelay)
{
}

FMOD_RESULT ChannelReal::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
}

FMOD_RESULT ChannelReal::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
}

FMOD_RESULT ChannelReal::updateSpeakerLevels(float volume)
{
}

FMOD_RESULT ChannelReal::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelReal::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT ChannelReal::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
}

FMOD_RESULT ChannelReal::setLoopCount(int loopcount)
{
}

FMOD_RESULT ChannelReal::set3DAttributes()
{
}

FMOD_RESULT ChannelReal::set3DMinMaxDistance()
{
}

FMOD_RESULT ChannelReal::set3DOcclusion(float directOcclusion, float reverbOcclusion)
{
}

FMOD_RESULT ChannelReal::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelReal::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
}

FMOD_RESULT ChannelReal::isPlaying(bool * isplaying, bool includethreadlatency)
{
}

FMOD_RESULT ChannelReal::isVirtual(bool * isvirtual)
{
}

FMOD_RESULT ChannelReal::getSpectrum(float * spectrumarray, int numentries, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
}

FMOD_RESULT ChannelReal::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
}

FMOD_RESULT ChannelReal::getDSPHead(DSPI * * dsp)
{
}

FMOD_RESULT ChannelReal::setMode(FMOD_MODE mode)
{
}

} // namespace FMOD
