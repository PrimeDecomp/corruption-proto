// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNEL_STREAM_H
#define _FMOD_CHANNEL_STREAM_H

#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_linkedlist.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct ChannelGroupI;
    class ChannelReal;
    struct ChannelStream;
    class DSPI;
}

namespace FMOD {

struct ChannelStream : public ChannelReal, public LinkedListNode
{
    volatile bool mFinished; // offset 0x84
    volatile bool mBusy; // offset 0x85
    unsigned int mLastPCM; // offset 0x88
    unsigned int mDecodeOffset; // offset 0x8C
    unsigned int mSamplesProcessed; // offset 0x90
    unsigned int mSamplesProcessedLast; // offset 0x94
    LinkedListNode mStreamNode; // offset 0x98
    virtual bool isStream();
    FMOD_RESULT set3DConeSettings(float, float, float);
    FMOD_RESULT set3DConeOrientation(FMOD_VECTOR *);
    ChannelStream();
    FMOD_RESULT setRealChannel(ChannelReal * realchan);
    virtual FMOD_RESULT set2DFreqVolumePanFor3D();
    virtual FMOD_RESULT moveChannelGroup(ChannelGroupI * oldchannelgroup, ChannelGroupI * newchannelgroup);
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT updateStream();
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setDelay(unsigned int startdelay, unsigned int enddelay);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT set3DAttributes();
    virtual FMOD_RESULT set3DMinMaxDistance();
    virtual FMOD_RESULT set3DOcclusion(float directocclusion, float reverbocclusion);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT isPlaying(bool * isplaying, bool includethreadlatency);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
};

} // namespace FMOD

#endif
