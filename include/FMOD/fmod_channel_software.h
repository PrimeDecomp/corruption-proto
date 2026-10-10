// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelSoftware is the G2MEAB layout.

#ifndef _FMOD_CHANNEL_SOFTWARE_H
#define _FMOD_CHANNEL_SOFTWARE_H

#include "fmod.h"
#include "fmod_channel_realmanual3d.h"

namespace FMOD {
    struct DSPCodec;
    class DSPI;
    class Output;
    struct SystemI;
}

namespace FMOD {

// G2MEAB layout (0x90; OutputSoftware::init 0x806102C4 callocs count * 0x90). Ctor 0x805B8A3C runs the
// ChannelReal ctor, stores the ChannelRealManual3D vtable 0x806E27A0 and then 0x806E28D4 at +0x74 and clears
// +0x84/+0x78/+0x88/+0x8C and mDSP. The 4.06 embedded DSPFilter/DSPWaveTable memory, the low-pass unit, the
// reverb connection and all reverb/moveChannelGroup members are absent: init 0x805B8A9C creates the head,
// sub-channel head and wavetable units through SystemI::createDSP. Overrides follow the ChannelReal slot order.
class ChannelSoftware : public ChannelRealManual3D
{
protected:
    DSPI * mDSPHead; // offset 0x78, "FMOD Channel DSPHead Unit" (a DSPFilter: getSpectrum 0x805BA954 startBuffering)
    DSPI * mSubChannelDSPHead; // offset 0x7C, Guessed name: "FMOD SubChannel DSPHead Unit" (init 0x805B8BA4)
    DSPI * mSubChannelDSPHeadTarget; // offset 0x80, Guessed name: own +0x7C, or sub-channel 0's +0x80 (alloc 0x805B9098)
    DSPI * mDSPWaveTable; // offset 0x84
    DSPI * mDSPResampler; // offset 0x88
    DSPCodec * mDSPCodec; // offset 0x8C
public:
    ChannelSoftware();

    // ChannelReal overrides
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT close();
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT alloc(DSPI * dsp);
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT isPlaying(bool * isplaying);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
};

} // namespace FMOD

#endif
