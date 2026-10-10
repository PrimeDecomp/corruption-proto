// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNELGROUPI_H
#define _FMOD_CHANNELGROUPI_H

#include "fmod.h"
#include "fmod_linkedlist.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct Channel;
    struct ChannelGroup;
    struct ChannelGroupI;
    class DSPI;
    struct System;
    struct SystemI;
}

namespace FMOD {

struct ChannelGroupI : public LinkedListNode
{
    static FMOD_RESULT validate(ChannelGroup * channelgroup, ChannelGroupI * * channelgroupi);
    SystemI * mSystem; // offset 0xC
    DSPI * mDSPHead; // offset 0x10
    void * mUserData; // offset 0x14
    ChannelGroupI * mParent; // offset 0x18
    ChannelGroupI * mGroupHead; // offset 0x1C
    LinkedListNode mChannelHead; // offset 0x20
    int mNumChannels; // offset 0x2C
    char * mName; // offset 0x30
    float mVolume; // offset 0x34
    float mRealVolume; // offset 0x38
    float mPitch; // offset 0x3C
    float mRealPitch; // offset 0x40
    bool mMute; // offset 0x44
    bool mPaused; // offset 0x45
    ChannelGroupI();
    FMOD_RESULT release();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT setVolume(float volume);
    FMOD_RESULT setVolumeInternal();
    FMOD_RESULT getVolume(float * volume);
    FMOD_RESULT setPitch(float pitch);
    FMOD_RESULT setPitchInternal();
    FMOD_RESULT getPitch(float * pitch);
    FMOD_RESULT stop();
    FMOD_RESULT overridePaused(bool paused);
    FMOD_RESULT overrideVolume(float volume);
    FMOD_RESULT overrideFrequency(float frequency);
    FMOD_RESULT overridePan(float pan);
    FMOD_RESULT overrideMute(bool mute);
    FMOD_RESULT overrideReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT override3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
    FMOD_RESULT overrideSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    FMOD_RESULT addGroup(ChannelGroupI * group);
    FMOD_RESULT getNumGroups(int * numgroups);
    FMOD_RESULT getGroup(int index, ChannelGroupI * * group);
    FMOD_RESULT getDSPHead(DSPI * * dsp);
    FMOD_RESULT addDSP(DSPI * dsp);
    FMOD_RESULT getName(char * name, int namelen);
    FMOD_RESULT getNumChannels(int * numchannels);
    FMOD_RESULT getChannel(int index, Channel * * channel);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    FMOD_RESULT setUserData(void * userdata);
    FMOD_RESULT getUserData(void * * userdata);
};

} // namespace FMOD

#endif
