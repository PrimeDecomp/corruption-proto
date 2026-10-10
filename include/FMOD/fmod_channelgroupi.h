// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelGroupI is the G2MEAB layout.

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

// G2MEAB layout (0x150): addGroup 0x805BCA28 and SystemI 0x80620924 calloc 0x150 and inline the
// constructor (node base with vtable 0x806E2AF0 at +0x10, mChannelHead node at +0x28, four 1.0f
// floats at +0x140..+0x14C); SystemI copies up to 0x100 name bytes to +0x40 and stores itself at
// +0x14; releaseInternal 0x805BC8F0 uses +0x18 as a DSP, +0x24 as the child group list head and
// +0x28 as the channel list; addGroup stores the parent at +0x20. The vtable holds only the
// implicit deleting destructor 0x805BCC28.
struct ChannelGroupI : public LinkedListNode
{
    static FMOD_RESULT validate(ChannelGroup * channelgroup, ChannelGroupI * * channelgroupi);
    SystemI * mSystem; // offset 0x14
    DSPI * mDSPHead; // offset 0x18
    int mUnk1C; // offset 0x1C, unresolved (4.06 order suggests mUserData)
    ChannelGroupI * mParent; // offset 0x20
    ChannelGroupI * mGroupHead; // offset 0x24
    LinkedListNode mChannelHead; // offset 0x28
    int mNumChannels; // offset 0x3C, ChannelI::setChannelGroup 0x805BFE88 decrements the old group's and increments the new group's count
    char mName[256]; // offset 0x40
    float mVolume; // offset 0x140
    float mRealVolume; // offset 0x144
    float mPitch; // offset 0x148
    float mRealPitch; // offset 0x14C

    ChannelGroupI()
    {
        mVolume = mRealVolume = 1.0f;
        mPitch = mRealPitch = 1.0f;
    }
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
