// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNEL_REAL_H
#define _FMOD_CHANNEL_REAL_H

#include "fmod.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
namespace FMOD {
    struct ChannelGroupI;
    struct ChannelI;
    class ChannelPool;
    class ChannelReal;
    class DSPI;
    class Output;
    struct SoundI;
    struct SystemI;
}

namespace FMOD {

enum CHANNELREAL_TYPE {
    CHANNELREAL_TYPE_HW3D = 0,
    CHANNELREAL_TYPE_HW2D = 1,
    CHANNELREAL_TYPE_SW = 2,
    CHANNELREAL_TYPE_MAX = 3
};

typedef unsigned int CHANNELREAL_FLAG;
class ChannelReal
{
protected:
    SystemI * mSystem; // offset 0x4
    int mNumRealChannels; // offset 0x8
    ChannelReal * mRealChannel[8]; // offset 0xC
    int mSubChannelIndex; // offset 0x2C
    ChannelPool * mPool; // offset 0x30
    ChannelI * mParent; // offset 0x34
    Output * mOutput; // offset 0x38
    SoundI * mSound; // offset 0x3C
    DSPI * mDSP; // offset 0x40
    FMOD_MODE mMode; // offset 0x44
    CHANNELREAL_FLAG mFlags; // offset 0x48
    int mIndex; // offset 0x4C
    unsigned int mPosition; // offset 0x50
    int mDirection; // offset 0x54
    int mLoopCount; // offset 0x58
    unsigned int mLoopStart; // offset 0x5C
    unsigned int mLoopLength; // offset 0x60
    unsigned int mLength; // offset 0x64
    unsigned int mStartDelay; // offset 0x68
    unsigned int mEndDelay; // offset 0x6C
    float mMaxFrequency; // offset 0x70
    float mMinFrequency; // offset 0x74
public:
    FMOD_RESULT init();
    FMOD_RESULT calcVolumeAndPitchFor3D();
    virtual bool isStream();
    virtual FMOD_RESULT moveChannelGroup(ChannelGroupI *, ChannelGroupI *);
    virtual ~ChannelReal();
    ChannelReal();
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT close();
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT alloc(DSPI * dsp);
    virtual FMOD_RESULT set2DFreqVolumePanFor3D();
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT updateStream();
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT getPaused(bool * paused);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setDelay(unsigned int startdelay, unsigned int enddelay);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
    virtual FMOD_RESULT updateSpeakerLevels(float volume);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT set3DAttributes();
    virtual FMOD_RESULT set3DMinMaxDistance();
    virtual FMOD_RESULT set3DOcclusion(float directOcclusion, float reverbOcclusion);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT isPlaying(bool * isplaying, bool includethreadlatency);
    virtual FMOD_RESULT isVirtual(bool * isvirtual);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numentries, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
};

} // namespace FMOD

#endif
