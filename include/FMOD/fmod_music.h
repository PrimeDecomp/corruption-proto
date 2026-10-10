// Reconstructed from a later FMOD Ex (Gormiti, Wii/MWCC) debug information. Member layout and offsets are the Gormiti reference, not yet verified against G2MEAB.

#ifndef _FMOD_MUSIC_H
#define _FMOD_MUSIC_H

#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_codeci.h"
#include "fmod_linkedlist.h"

struct FMOD_CODEC_STATE;
namespace FMOD {
    struct ChannelGroupI;
    struct ChannelI;
    struct ChannelMusic;
    class ChannelPool;
    class ChannelReal;
    class ChannelSoftware;
    struct Codec;
    class DSPI;
    class LinkedListNode;
    struct MusicChannel;
    struct MusicEnvelopeState;
    struct MusicInstrument;
    struct MusicNote;
    struct MusicPattern;
    struct MusicSample;
    struct MusicSong;
    struct MusicVirtualChannel;
    struct SoundI;
    struct _SNDMIXPLUGIN;
    struct _SNDMIXPLUGININFO;
}

namespace FMOD {

struct MusicSample
{
    SoundI * mSound; // offset 0x0
    unsigned int mMiddleC; // offset 0x4
    unsigned char mDefaultVolume; // offset 0x8
    unsigned char mDefaultPan; // offset 0x9
    unsigned int mLoopStart; // offset 0xC
    unsigned int mLoopLength; // offset 0x10
    int mRelative; // offset 0x14
    int mFineTune; // offset 0x18
    unsigned int mRawLength; // offset 0x1C
    unsigned char mFlags; // offset 0x20
    unsigned char mGlobalVolume; // offset 0x21
    unsigned int mSusLoopBegin; // offset 0x24
    unsigned int mSusLoopEnd; // offset 0x28
    unsigned char mVibSpeed; // offset 0x2C
    unsigned char mVibDepth; // offset 0x2D
    unsigned char mVibType; // offset 0x2E
    unsigned char mVibRate; // offset 0x2F
    FMOD_SOUND_FORMAT mOriginalFormat; // offset 0x30
};

struct MusicEnvelopeState
{
    int mTick; // offset 0x0
    int mPosition; // offset 0x4
    int mFraction; // offset 0x8
    int mValue; // offset 0xC
    int mDelta; // offset 0x10
    bool mStopped; // offset 0x14
};

struct MusicVirtualChannel : public LinkedListNode
{
    int mIndex; // offset 0xC
    bool mAllocated; // offset 0x10
    bool mFlip; // offset 0x11
    ChannelI mChannel; // offset 0x18
    MusicSample * mSample; // offset 0x1C8
    MusicSong * mSong; // offset 0x1CC
    unsigned char mLastInstrument; // offset 0x1D0
    unsigned char mLastNote; // offset 0x1D1
    unsigned char mLastSample; // offset 0x1D2
    bool mBackground; // offset 0x1D3
    unsigned char mNoteControl; // offset 0x1D4
    unsigned char mNNA; // offset 0x1D5
    unsigned char mVolType; // offset 0x1D6
    int mFrequency; // offset 0x1D8
    int mVolume; // offset 0x1DC
    int mPan; // offset 0x1E0
    int mVolumeDelta; // offset 0x1E4
    int mFrequencyDelta; // offset 0x1E8
    int mPanDelta; // offset 0x1EC
    unsigned int mSampleOffset; // offset 0x1F0
    int mDirection; // offset 0x1F4
    int mSampGlobalVol; // offset 0x1F8
    MusicEnvelopeState mEnvVolume; // offset 0x1FC
    MusicEnvelopeState mEnvPan; // offset 0x214
    int mEnvPitchTick; // offset 0x22C
    int mEnvPitchPos; // offset 0x230
    int mEnvPitchFrac; // offset 0x234
    int mEnvPitch; // offset 0x238
    int mEnvPitchDelta; // offset 0x23C
    bool mEnvPitchStopped; // offset 0x240
    bool mFade; // offset 0x241
    int mFadeOutVolume; // offset 0x244
    int mIVibPos; // offset 0x248
    int mIVibSweepPos; // offset 0x24C
    bool mKeyOff; // offset 0x250
    bool mRamping; // offset 0x251
    int mTicksToDie; // offset 0x254
};

struct MusicPattern
{
    int mRows; // offset 0x0
    MusicNote * mData; // offset 0x4
};

struct MusicNote
{
    unsigned char mNote; // offset 0x0
    unsigned char mNumber; // offset 0x1
    unsigned char mVolume; // offset 0x2
    unsigned char mEffect; // offset 0x3
    unsigned char mEffectParam; // offset 0x4
};

struct MusicChannel
{
    MusicVirtualChannel mVirtualChannelHead; // offset 0x0
    unsigned char mInstrument; // offset 0x258
    unsigned char mNote; // offset 0x259
    unsigned char mSample; // offset 0x25A
    unsigned char mRealNote; // offset 0x25B
    int mPeriod; // offset 0x25C
    unsigned char mRecentEffect; // offset 0x260
    int mVolume; // offset 0x264
    int mPan; // offset 0x268
    int mVolumeDelta; // offset 0x26C
    unsigned int mSampleOffset; // offset 0x270
    int mGlobalVolume; // offset 0x274
    float mMasterVolume; // offset 0x278
    unsigned char mPortaUpDown; // offset 0x27C
    unsigned char mPortaDown; // offset 0x27D
    unsigned char mPortaUp; // offset 0x27E
    unsigned char mXtraPortaDown; // offset 0x27F
    unsigned char mXtraPortaUp; // offset 0x280
    unsigned char mVolumeSlide; // offset 0x281
    unsigned char mPanSlide; // offset 0x282
    unsigned char mRetrigX; // offset 0x283
    unsigned char mRetrigY; // offset 0x284
    unsigned char mRetrigCount; // offset 0x285
    int mPortaTarget; // offset 0x288
    unsigned char mPortaSpeed; // offset 0x28C
    unsigned char mPortaReached; // offset 0x28D
    signed char mVibPos; // offset 0x28E
    unsigned char mVibSpeed; // offset 0x28F
    unsigned char mVibDepth; // offset 0x290
    unsigned char mVibType; // offset 0x291
    signed char mTremoloPosition; // offset 0x292
    unsigned char mTremoloSpeed; // offset 0x293
    unsigned char mTremoloDepth; // offset 0x294
    int mPanbrelloPos; // offset 0x298
    unsigned char mPanbrelloSpeed; // offset 0x29C
    unsigned char mPanbrelloDepth; // offset 0x29D
    unsigned char mTremorPosition; // offset 0x29E
    unsigned char mTremorOn; // offset 0x29F
    unsigned char mTremorOff; // offset 0x2A0
    unsigned char mArpeggio; // offset 0x2A1
    int mPatternLoopRow; // offset 0x2A4
    int mPatternLoopNumber; // offset 0x2A8
    unsigned char mChannelVolumeSlide; // offset 0x2AC
    unsigned char mSpecialParam; // offset 0x2AD
    unsigned char mWaveControl; // offset 0x2AE
    unsigned char mWaveControlVibrato; // offset 0x2AF
    unsigned char mWaveControlTremolo; // offset 0x2B0
    unsigned char mWaveControlPan; // offset 0x2B1
    unsigned char mFineVolumeSlideDown; // offset 0x2B2
    unsigned char mFineVolumeSlideUp; // offset 0x2B3
    unsigned char mFinePortaUp; // offset 0x2B4
    unsigned char mFinePortaDown; // offset 0x2B5
    unsigned char mHighOffset; // offset 0x2B6
    unsigned char mVolumeColumnVolumeSlide; // offset 0x2B7
};

struct MusicInstrument
{
    signed char mName[28]; // offset 0x0
    int mNumSamples; // offset 0x1C
    MusicSample mSample[16]; // offset 0x20
    unsigned char mKeyMap[96]; // offset 0x360
    unsigned char mVolumeType; // offset 0x3C0
    unsigned char mVolumeNumPoints; // offset 0x3C1
    unsigned short mVolumePoints[40]; // offset 0x3C2
    unsigned char mVolumeSustain; // offset 0x412
    unsigned char mVolumeLoopStart; // offset 0x413
    unsigned char mVolumeLoopEnd; // offset 0x414
    unsigned char mVolumeSustainLoopStart; // offset 0x415
    unsigned char mVolumeSustainLoopEnd; // offset 0x416
    unsigned char mPanType; // offset 0x417
    unsigned char mPanNumPoints; // offset 0x418
    unsigned short mPanPoints[40]; // offset 0x41A
    unsigned char mPanSustain; // offset 0x46A
    unsigned char mPanLoopStart; // offset 0x46B
    unsigned char mPanLoopEnd; // offset 0x46C
    unsigned char mPanSustainLoopStart; // offset 0x46D
    unsigned char mPanSustainLoopEnd; // offset 0x46E
    unsigned char mPitchType; // offset 0x46F
    unsigned char mPitchNumpoints; // offset 0x470
    unsigned short mPitchPoints[40]; // offset 0x472
    unsigned char mPitchSustain; // offset 0x4C2
    unsigned char mPitchLoopStart; // offset 0x4C3
    unsigned char mPitchLoopEnd; // offset 0x4C4
    unsigned char mPitchSustainLoopStart; // offset 0x4C5
    unsigned char mPitchSustainLoopEnd; // offset 0x4C6
    unsigned char mVibratoType; // offset 0x4C7
    unsigned char mVibratoSweep; // offset 0x4C8
    unsigned char mVibratoDepth; // offset 0x4C9
    unsigned char mVibratoRate; // offset 0x4CA
    unsigned short mVolumeFade; // offset 0x4CC
    unsigned char mGlobalVolume; // offset 0x4CE
    unsigned char mDefaultPan; // offset 0x4CF
    unsigned char mNNA; // offset 0x4D0
    unsigned char mDupCheckType; // offset 0x4D1
    unsigned char mDupCheckAction; // offset 0x4D2
    unsigned char mPitchPanSep; // offset 0x4D3
    unsigned char mPitchPanCenter; // offset 0x4D4
    unsigned char mVolumeVariation; // offset 0x4D5
    unsigned char mPanVariation; // offset 0x4D6
    unsigned char mNoteTable[240]; // offset 0x4D7
    unsigned int mFilterCutOff; // offset 0x5C8
    unsigned int mFilterResonance; // offset 0x5CC
    unsigned char mMIDIOutput; // offset 0x5D0
};

struct _SNDMIXPLUGININFO
{
    unsigned int dwPluginId1; // offset 0x0
    unsigned int dwPluginId2; // offset 0x4
    unsigned int dwInputRouting; // offset 0x8
    unsigned int dwOutputRouting; // offset 0xC
    unsigned int dwReserved[4]; // offset 0x10
    char szName[32]; // offset 0x20
    char szLibraryName[64]; // offset 0x40
};

struct _SNDMIXPLUGIN
{
    ChannelGroupI mChannelGroup; // offset 0x0
    void * pMixPlugin; // offset 0x5C
    _SNDMIXPLUGININFO Info; // offset 0x60
};

struct ChannelMusic : public ChannelReal
{
    FMOD_RESULT updateStream();
    FMOD_RESULT stop(bool force, bool updateflags);
    FMOD_RESULT start();
    FMOD_RESULT setPaused(bool paused);
    FMOD_RESULT setVolume(float volume);
    MusicSong * mMusic; // offset 0x64
};

struct MusicSong : public Codec
{
    FMOD_RESULT play(bool fromopen);
    FMOD_RESULT spawnNewVirtualChannel();
    FMOD_RESULT setBPM();
    FMOD_RESULT stop();
    FMOD_RESULT playSound(MusicSample * sample, MusicVirtualChannel * vcptr, bool addfilter, _SNDMIXPLUGIN * plugin);
    static FMOD_RESULT getLengthCallback(FMOD_CODEC_STATE * codec_state, unsigned int * length, unsigned int lengthtype);
    static FMOD_RESULT getPositionCallback(FMOD_CODEC_STATE * codec_state, unsigned int * position, unsigned int postype);
    static FMOD_RESULT getMusicNumChannelsCallback(FMOD_CODEC_STATE * codec, int * numchannels);
    static FMOD_RESULT setMusicChannelVolumeCallback(FMOD_CODEC_STATE * codec, int channel, float volume);
    static FMOD_RESULT getMusicChannelVolumeCallback(FMOD_CODEC_STATE * codec, int channel, float * volume);
    static FMOD_RESULT getHardwareMusicChannelCallback(FMOD_CODEC_STATE * codec, ChannelReal * * realchannel);
    char mSongName[256]; // offset 0xFD
    MusicPattern * mPattern; // offset 0x200
    DSPI * mDSPHead; // offset 0x204
    bool * mVisited; // offset 0x208
    unsigned char mOrderList[256]; // offset 0x20C
    int mNumChannels; // offset 0x30C
    MusicChannel * mMusicChannel[64]; // offset 0x310
    int mNumVirtualChannels; // offset 0x410
    MusicVirtualChannel * mVirtualChannel; // offset 0x414
    ChannelPool * mChannelPool; // offset 0x418
    ChannelSoftware * mChannelSoftware; // offset 0x41C
    DSPI * * mLowPass; // offset 0x420
    ChannelGroupI mChannelGroup; // offset 0x424
    ChannelMusic mHardwareMusicChannel; // offset 0x480
    int mMixerSamplesLeft; // offset 0x4E8
    int mMixerSamplesPerTick; // offset 0x4EC
    unsigned int mPCMOffset; // offset 0x4F0
    unsigned int mDSPTick; // offset 0x4F4
    int mDefaultSpeed; // offset 0x4F8
    unsigned int mDefaultBPM; // offset 0x4FC
    unsigned char mDefaultPan[64]; // offset 0x500
    unsigned char mDefaultVolume[64]; // offset 0x540
    unsigned char mDefaultGlobalVolume; // offset 0x580
    int mNumOrders; // offset 0x584
    int mNumPatterns; // offset 0x588
    int mNumPatternsMem; // offset 0x58C
    int mNumInstruments; // offset 0x590
    int mNumSamples; // offset 0x594
    MusicInstrument * mInstrument; // offset 0x598
    signed char * mPatternPtr; // offset 0x59C
    unsigned char mLastNote[64]; // offset 0x5A0
    unsigned char mLastNumber[64]; // offset 0x5E0
    unsigned char mLastVolume[64]; // offset 0x620
    unsigned char mLastEffect[64]; // offset 0x660
    unsigned char mLastEffectParam[64]; // offset 0x6A0
    unsigned char mPreviousMaskVariable[64]; // offset 0x6E0
    MusicNote mNote[64]; // offset 0x720
    int mRestart; // offset 0x860
    float mMasterSpeed; // offset 0x864
    float mPanSeparation; // offset 0x868
    int mMasterVolume; // offset 0x86C
    int mGlobalVolume; // offset 0x870
    unsigned char mGlobalVolumeSlide; // offset 0x874
    unsigned short mMusicFlags; // offset 0x876
    bool mPlaying; // offset 0x878
    bool mFinished; // offset 0x879
    bool mLooping; // offset 0x87A
    int mTick; // offset 0x87C
    int mSpeed; // offset 0x880
    int mBPM; // offset 0x884
    int mRow; // offset 0x888
    int mOrder; // offset 0x88C
    int mPatternDelay; // offset 0x890
    int mPatternDelayTicks; // offset 0x894
    int mNextRow; // offset 0x898
    int mNextOrder; // offset 0x89C
};

} // namespace FMOD

#endif
