// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNEL_SOFTWARE_H
#define _FMOD_CHANNEL_SOFTWARE_H

#include "fmod.h"
#include "fmod_channel_realmanual3d.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_wavetable.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
namespace FMOD {
    struct ChannelGroupI;
    class ChannelSoftware;
    struct DSPCodec;
    class DSPConnection;
    class DSPI;
    class Output;
    class ReverbI;
    struct SystemI;
}

namespace FMOD {

class ChannelSoftware : public ChannelRealManual3D
{
protected:
    DSPI * mDSPHead; // offset 0x78
    DSPFilter mDSPHeadMemory; // offset 0x7C
    char mDSPHeadMemoryPad[16]; // offset 0x190
    DSPI * mDSPWaveTable; // offset 0x1A0
    DSPWaveTable mDSPWaveTableMemory; // offset 0x1A8
    char mDSPWaveTableMemoryPad[16]; // offset 0x2D0
    DSPI * mDSPResampler; // offset 0x2E0
    DSPI * mDSPLowPass; // offset 0x2E4
    DSPCodec * mDSPCodec; // offset 0x2E8
    DSPConnection * mDSPConnection; // offset 0x2EC
public:
    FMOD_RESULT setReverbMix(ReverbI *, float);
    ChannelSoftware();
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT close();
    FMOD_RESULT setupDSPCodec(DSPI * dsp);
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT alloc(DSPI * dsp);
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT setMultiReverbProperties(ReverbI * reverb, const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT getMultiReverbProperties(ReverbI * reverb, FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT updateDirectMix(float volume);
    FMOD_RESULT updateReverbMix(ReverbI * reverb, float volume);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT set3DOcclusion(float directOcclusion, float reverbOcclusion);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
    virtual FMOD_RESULT isPlaying(bool * isplaying, bool includethreadlatency);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
    FMOD_RESULT addToReverbs();
    virtual FMOD_RESULT moveChannelGroup(ChannelGroupI * oldchannelgroup, ChannelGroupI * newchannelgroup);
};

} // namespace FMOD

#endif
