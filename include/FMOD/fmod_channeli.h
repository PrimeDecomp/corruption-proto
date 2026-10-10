// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelI and FMOD_CHANNEL_INFO are the G2MEAB layouts
// (fmod_channeli.cpp 0x805BCCA8..0x805C11FC).

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

// G2MEAB layout (0xAC) from ChannelI::getChannelInfo 0x805BD1A4 and setChannelInfo 0x805BD2AC: the
// speaker levels are embedded (getSpeakerLevels(speaker, &mLevels[speaker * 8], ...) for each output
// speaker), there is no low-pass cutoff, DSP head or mode, and the reverb properties end the record.
struct FMOD_CHANNEL_INFO
{
    float mLevels[16]; // offset 0x0
    unsigned int mPCM; // offset 0x40
    unsigned int mLoopStart; // offset 0x44
    unsigned int mLoopEnd; // offset 0x48
    ChannelReal * mRealChannel; // offset 0x4C
    SoundI * mSound; // offset 0x50
    int mLoopCount; // offset 0x54
    bool mMute; // offset 0x58
    bool mPaused; // offset 0x59
    unsigned int mStartDelay; // offset 0x5C
    unsigned int mEndDelay; // offset 0x60
    FMOD_REVERB_CHANNELPROPERTIES mReverbProperties; // offset 0x64
};

// G2MEAB layout from ChannelI() fn_805BDCB8, ChannelI(int, SystemI *) fn_805BDD48 and init
// fn_805BDE24: 4.06 order with 0x14 nodes, an extra word at +0x5C, and no mLPFCutoff, rolloff
// points, spread, 3D pan level or 3D doppler level. The base node's vptr (+0x10) takes the ChannelI
// vtable __vt__Q24FMOD8ChannelI (destructor 0x805C1164).
struct ChannelI : public LinkedListNode
{
    LinkedListNode mSortedListNode; // offset 0x14
    int mIndex; // offset 0x28
    void * mUserData; // offset 0x2C
    unsigned int mHandleOriginal; // offset 0x30
    SystemI * mSystem; // offset 0x34
    int mNumRealChannels; // offset 0x38
    ChannelReal * mRealChannel[8]; // offset 0x3C
    FMOD_VECTOR * mUnk5C; // offset 0x5C, per-subchannel 3D position offsets (indexed in ChannelRealManual3D 0x805B7BD8); an allocation freed by stopEx 0x805BE878 (line 0x6D9), cleared by init
    unsigned int mHandleCurrent; // offset 0x60
    FMOD_CHANNEL_PANMODE mLastPanMode; // offset 0x64
    bool mLastPaused; // offset 0x68
    int mPriority; // offset 0x6C
    unsigned int mListPosition; // offset 0x70
    bool mJustWentVirtual; // offset 0x74
    unsigned int mSyncPointLastPos; // offset 0x78, Guessed name: PCM position of the last sync-point scan (update 0x805BE380)
    ChannelGroupI * mChannelGroup; // offset 0x7C
    LinkedListNode mChannelGroupNode; // offset 0x80
    float mVolume; // offset 0x94
    float mFrequency; // offset 0x98
    float mPan; // offset 0x9C
    float mSpeakerFL; // offset 0xA0
    float mSpeakerFR; // offset 0xA4
    float mSpeakerC; // offset 0xA8
    float mSpeakerLFE; // offset 0xAC
    float mSpeakerBL; // offset 0xB0
    float mSpeakerBR; // offset 0xB4
    float mSpeakerSL; // offset 0xB8
    float mSpeakerSR; // offset 0xBC
    float * mLevels; // offset 0xC0
    bool mMute; // offset 0xC4
    bool mMoved; // offset 0xC5
    float mVolumeOcclusion; // offset 0xC8
    float mVolume3D; // offset 0xCC
    float mPitch3D; // offset 0xD0
    FMOD_VECTOR mPosition3D; // offset 0xD4
    FMOD_VECTOR mVelocity3D; // offset 0xE0
    float mDistance; // offset 0xEC
    float mMinDistance; // offset 0xF0
    float mMaxDistance; // offset 0xF4
    float mConeVolume3D; // offset 0xF8
    float mConeInsideAngle; // offset 0xFC
    float mConeOutsideAngle; // offset 0x100
    float mConeOutsideVolume; // offset 0x104
    FMOD_VECTOR mConeOrientation; // offset 0x108
    float mDirectOcclusion; // offset 0x114
    float mReverbOcclusion; // offset 0x118
    FMOD_VECTOR * mRolloffPoints; // offset 0x11C, set3DCustomRolloff 0x805BFBA8; read by calcVolumeAndPitchFor3D 0x805BD588
    int mNumRolloffPoints; // offset 0x120
    FMOD_CHANNEL_CALLBACK mCallback[3]; // offset 0x124
    int mCallbackCommand[3]; // offset 0x130
    static FMOD_RESULT validate(Channel * channel, ChannelI * * channeli);
    // Inline in G2MEAB: expanded in isPlaying 0x805C00F0 (no out-of-line copy).
    FMOD_RESULT validateInternal()
    {
        bool valid = false;

        if (mHandleCurrent == mHandleOriginal && mRealChannel)
        {
            valid = true;
        }

        return valid ? FMOD_OK : FMOD_ERR_CHANNEL_STOLEN;
    }
    FMOD_RESULT returnToFreeList();
    // Guessed signature: G2MEAB 0x805BCE30 takes the sound defaults from play 0x805BDF10 (priority,
    // frequency, volume, pan and the three variations) instead of reading them itself.
    FMOD_RESULT setDefaults(int priority, float frequency, float volume, float pan, float frequencyvariation, float volumevariation, float panvariation);
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
    FMOD_RESULT stopEx(bool refstamp, bool updatelist, bool resetcallbacks, bool updateflags, bool callendcallback); // G2MEAB 0x805BE878 reads r4..r8 only (no 4.06 resetchannelgroup)
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
