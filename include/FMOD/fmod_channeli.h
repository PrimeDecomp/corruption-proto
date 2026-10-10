// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNELI_H
#define _FMOD_CHANNELI_H

#include "fmod.h"
#include "fmod_linkedlist.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct Channel;
    struct ChannelGroupI;
    struct ChannelI;
    class ChannelReal;
    class DSPI;
    struct FMOD_CHANNEL_INFO;
    struct SoundI;
    struct SyncPoint;
    struct System;
    struct SystemI;
}

namespace FMOD {

const unsigned int SYSTEMID_BITS = 4;
const unsigned int CHANINDEX_BITS = 12;
const unsigned int REFCOUNT_BITS = 16;
const unsigned int SYSTEMID_SHIFT = 28;
const unsigned int CHANINDEX_SHIFT = 16;
const unsigned int REFCOUNT_SHIFT = 0;
const unsigned int SYSTEMID_MASK = 15;
const unsigned int CHANINDEX_MASK = 4095;
const unsigned int REFCOUNT_MASK = 65535;
const unsigned int FMOD_CHANNEL_DEFAULTPRIORITY = 128;
const int FMOD_CHANNEL_MAXREALSUBCHANNELS = 8;
enum FMOD_CHANNEL_PANMODE {
    FMOD_CHANNEL_PANMODE_PAN = 0,
    FMOD_CHANNEL_PANMODE_SPEAKERMIX = 1,
    FMOD_CHANNEL_PANMODE_SPEAKERLEVELS = 2
};

struct FMOD_CHANNEL_INFO
{
    float * mLevels; // offset 0x0
    unsigned int mPCM; // offset 0x4
    unsigned int mLoopStart; // offset 0x8
    unsigned int mLoopEnd; // offset 0xC
    ChannelReal * mRealChannel; // offset 0x10
    SoundI * mSound; // offset 0x14
    int mLoopCount; // offset 0x18
    bool mMute; // offset 0x1C
    bool mPaused; // offset 0x1D
    unsigned int mStartDelay; // offset 0x20
    unsigned int mEndDelay; // offset 0x24
    FMOD_REVERB_CHANNELPROPERTIES mReverbProperties; // offset 0x28
    int mLowPassCutoff; // offset 0x70
    DSPI * mDSPHead; // offset 0x74
    FMOD_MODE mMode; // offset 0x78
};

struct ChannelI : public LinkedListNode
{
    SortedLinkedListNode mSortedListNode; // offset 0xC
    int mIndex; // offset 0x1C
    void * mUserData; // offset 0x20
    unsigned int mHandleOriginal; // offset 0x24
    SystemI * mSystem; // offset 0x28
    int mNumRealChannels; // offset 0x2C
    ChannelReal * mRealChannel[8]; // offset 0x30
    unsigned int mHandleCurrent; // offset 0x50
    FMOD_CHANNEL_PANMODE mLastPanMode; // offset 0x54
    bool mLastPaused; // offset 0x58
    int mPriority; // offset 0x5C
    unsigned int mListPosition; // offset 0x60
    bool mJustWentVirtual; // offset 0x64
    SyncPoint * mLastSyncPoint; // offset 0x68
    ChannelGroupI * mChannelGroup; // offset 0x6C
    LinkedListNode mChannelGroupNode; // offset 0x70
    int mLPFCutoff; // offset 0x7C
    float mVolume; // offset 0x80
    float mFrequency; // offset 0x84
    float mPan; // offset 0x88
    float mSpeakerFL; // offset 0x8C
    float mSpeakerFR; // offset 0x90
    float mSpeakerC; // offset 0x94
    float mSpeakerLFE; // offset 0x98
    float mSpeakerBL; // offset 0x9C
    float mSpeakerBR; // offset 0xA0
    float mSpeakerSL; // offset 0xA4
    float mSpeakerSR; // offset 0xA8
    float * mLevels; // offset 0xAC
    bool mMute; // offset 0xB0
    bool mMoved; // offset 0xB1
    float mVolumeOcclusion; // offset 0xB4
    float mVolume3D; // offset 0xB8
    float mPitch3D; // offset 0xBC
    FMOD_VECTOR mPosition3D; // offset 0xC0
    FMOD_VECTOR mVelocity3D; // offset 0xCC
    float mDistance; // offset 0xD8
    float mMinDistance; // offset 0xDC
    float mMaxDistance; // offset 0xE0
    float mConeVolume3D; // offset 0xE4
    float mConeInsideAngle; // offset 0xE8
    float mConeOutsideAngle; // offset 0xEC
    float mConeOutsideVolume; // offset 0xF0
    FMOD_VECTOR mConeOrientation; // offset 0xF4
    float mDirectOcclusion; // offset 0x100
    float mReverbOcclusion; // offset 0x104
    float mDirectOcclusionTarget; // offset 0x108
    float mReverbOcclusionTarget; // offset 0x10C
    FMOD_VECTOR * mRolloffPoint; // offset 0x110
    int mNumRolloffPoints; // offset 0x114
    float mSpread; // offset 0x118
    float m3DPanLevel; // offset 0x11C
    float m3DDopplerLevel; // offset 0x120
    FMOD_CHANNEL_CALLBACK mCallback[3]; // offset 0x124
    int mCallbackCommand[3]; // offset 0x130
    static FMOD_RESULT validate(Channel * channel, ChannelI * * channeli);
    FMOD_RESULT validateInternal();
    FMOD_RESULT returnToFreeList();
    FMOD_RESULT setDefaults();
    FMOD_RESULT referenceStamp(bool newstamp);
    FMOD_RESULT updatePosition();
    FMOD_RESULT getChannelInfo(FMOD_CHANNEL_INFO * info);
    FMOD_RESULT setChannelInfo(FMOD_CHANNEL_INFO * info);
    FMOD_RESULT getRealChannel(ChannelReal * * realchan, int * subchannels);
    FMOD_RESULT calcVolumeAndPitchFor3D();
    FMOD_RESULT setChannelGroupInternal(ChannelGroupI * channelgroup, bool resetattributes);
    FMOD_RESULT set3DOcclusionInternal(float direct, float reverb, bool resettarget);
    ChannelI();
    ChannelI(int index, SystemI * system);
    FMOD_RESULT init();
    FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT play(SoundI * sound, bool paused, bool reset);
    FMOD_RESULT play(DSPI * dsp, bool paused, bool reset);
    FMOD_RESULT alloc(SoundI * sound, bool reset);
    FMOD_RESULT alloc(DSPI * dsp, bool reset);
    FMOD_RESULT start();
    FMOD_RESULT updateSyncPoints(bool seeking);
    FMOD_RESULT update(int delta, bool callrealupdate);
    FMOD_RESULT updateStream();
    FMOD_RESULT stop();
    FMOD_RESULT stopEx(bool refstamp, bool updatelist, bool resetcallbacks, bool updateflags, bool callendcallback, bool resetchannelgroup);
    FMOD_RESULT setPaused(bool paused);
    FMOD_RESULT getPaused(bool * paused);
    FMOD_RESULT setVolume(float volume);
    FMOD_RESULT getVolume(float * volume);
    FMOD_RESULT setFrequency(float frequency);
    FMOD_RESULT getFrequency(float * frequency);
    FMOD_RESULT setPan(float pan, bool calldriver);
    FMOD_RESULT getPan(float * pan);
    FMOD_RESULT setDelay(unsigned int startdelay, unsigned int enddelay);
    FMOD_RESULT getDelay(unsigned int * startdelay, unsigned int * enddelay);
    FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, bool calldriver);
    FMOD_RESULT getSpeakerMix(float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright);
    FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels, bool calldriver);
    FMOD_RESULT getSpeakerLevels(int speaker, float * levels, int numlevels);
    FMOD_RESULT setMute(bool mute);
    FMOD_RESULT getMute(bool * mute);
    FMOD_RESULT setPriority(int priority);
    FMOD_RESULT getPriority(int * priority);
    FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    FMOD_RESULT setChannelGroup(ChannelGroupI * channelgroup);
    FMOD_RESULT getChannelGroup(ChannelGroupI * * channelgroup);
    FMOD_RESULT setCallback(FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command);
    FMOD_RESULT set3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
    FMOD_RESULT get3DAttributes(FMOD_VECTOR * pos, FMOD_VECTOR * vel);
    FMOD_RESULT set3DMinMaxDistance(float mindistance, float maxdistance);
    FMOD_RESULT get3DMinMaxDistance(float * mindistance, float * maxdistance);
    FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    FMOD_RESULT get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume);
    FMOD_RESULT set3DConeOrientation(FMOD_VECTOR * orientation);
    FMOD_RESULT get3DConeOrientation(FMOD_VECTOR * orientation);
    FMOD_RESULT set3DCustomRolloff(FMOD_VECTOR * points, int numpoints);
    FMOD_RESULT get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints);
    FMOD_RESULT set3DOcclusion(float direct, float reverb);
    FMOD_RESULT get3DOcclusion(float * directOcclusion, float * reverbOcclusion);
    FMOD_RESULT set3DSpread(float angle);
    FMOD_RESULT get3DSpread(float * angle);
    FMOD_RESULT set3DPanLevel(float level);
    FMOD_RESULT get3DPanLevel(float * level);
    FMOD_RESULT set3DDopplerLevel(float level);
    FMOD_RESULT get3DDopplerLevel(float * level);
    FMOD_RESULT getDSPHead(DSPI * * dsp);
    FMOD_RESULT addDSP(DSPI * dsp);
    FMOD_RESULT addDSPChain(DSPI * dsp);
    FMOD_RESULT isPlaying(bool * isplaying);
    FMOD_RESULT isVirtual(bool * isvirtual);
    FMOD_RESULT getAudibility(float * audibility);
    FMOD_RESULT getCurrentSound(SoundI * * sound);
    FMOD_RESULT getCurrentDSP(DSPI * * dsp);
    FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channel, FMOD_DSP_FFT_WINDOW windowtype);
    FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channel);
    FMOD_RESULT getIndex(int * index);
    FMOD_RESULT setMode(FMOD_MODE mode);
    FMOD_RESULT getMode(FMOD_MODE * mode);
    FMOD_RESULT setLoopCount(int loopcount);
    FMOD_RESULT getLoopCount(int * loopcount);
    FMOD_RESULT setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
    FMOD_RESULT setUserData(void * userdata);
    FMOD_RESULT getUserData(void * * userdata);
};

} // namespace FMOD

#endif
