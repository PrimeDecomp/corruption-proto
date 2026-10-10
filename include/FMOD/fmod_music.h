// Member set from a later FMOD Ex (Gormiti, Wii/MWCC) debug information, adapted to the G2MEAB layouts
// (see the per-struct notes; offsets marked unverified still follow the Gormiti order).

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

// G2MEAB: 0x30 bytes (__construct_array of gDummyInstrument.mSample in __sinit_fmod_music_cpp); Gormiti's
// trailing mOriginalFormat is absent. Inline constructor emitted out of line at 0x8060D400.
struct MusicSample
{
    MusicSample()
    {
        mSound = 0;
        mMiddleC = 8363;
        mDefaultVolume = 0xFF;
        mLoopStart = 0;
        mLoopLength = 0;
        mRelative = 0;
    }

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

// G2MEAB: 0x1E8 bytes (memset in MusicSong::play 0x8060CA08); own vtable 0x806E3450 holding only the
// implicit destructor (0x805D0538, emitted in fmod_codec_it). The embedded ChannelI is 0x13C, so every
// member after it sits 0x70 below Gormiti (spawnNewVirtualChannel 0x8060CBF4 init stores).
struct MusicVirtualChannel : public LinkedListNode
{
    int mIndex; // offset 0x14
    bool mAllocated; // offset 0x18
    bool mFlip; // offset 0x19
    ChannelI mChannel; // offset 0x1C
    MusicSample * mSample; // offset 0x158
    MusicSong * mSong; // offset 0x15C
    unsigned char mLastInstrument; // offset 0x160
    unsigned char mLastNote; // offset 0x161
    unsigned char mLastSample; // offset 0x162
    bool mBackground; // offset 0x163
    unsigned char mNoteControl; // offset 0x164
    unsigned char mNNA; // offset 0x165
    unsigned char mVolType; // offset 0x166
    int mFrequency; // offset 0x168
    int mVolume; // offset 0x16C
    int mPan; // offset 0x170
    int mVolumeDelta; // offset 0x174
    int mFrequencyDelta; // offset 0x178
    int mPanDelta; // offset 0x17C
    unsigned int mSampleOffset; // offset 0x180
    int mDirection; // offset 0x184
    int mSampGlobalVol; // offset 0x188
    MusicEnvelopeState mEnvVolume; // offset 0x18C
    MusicEnvelopeState mEnvPan; // offset 0x1A4
    int mEnvPitchTick; // offset 0x1BC
    int mEnvPitchPos; // offset 0x1C0
    int mEnvPitchFrac; // offset 0x1C4
    int mEnvPitch; // offset 0x1C8
    int mEnvPitchDelta; // offset 0x1CC
    bool mEnvPitchStopped; // offset 0x1D0
    bool mFade; // offset 0x1D1
    int mFadeOutVolume; // offset 0x1D4
    int mIVibPos; // offset 0x1D8
    int mIVibSweepPos; // offset 0x1DC
    bool mKeyOff; // offset 0x1E0
    bool mRamping; // offset 0x1E1
    int mTicksToDie; // offset 0x1E4

    FMOD_RESULT cleanUp(); // Guessed name
};

// G2: MusicVirtualChannel::mNoteControl bits. Static const ints (each user TU pools them in .sdata2, e.g. XM
// 0x807A2E80..0x807A2E90); names guessed from the playSound/setVolume/setPan/setFrequency/stop uses in updateFlags.
static const int FMUSIC_FREQ = 1; // Guessed name
static const int FMUSIC_VOLUME = 2; // Guessed name
static const int FMUSIC_PAN = 4; // Guessed name
static const int FMUSIC_TRIGGER = 8; // Guessed name
static const int FMUSIC_STOP = 32; // Guessed name

// G2: music tables/dummies defined in fmod_music.cpp (Gormiti names): 0x806EE048, 0x806EE068, 0x806EE168,
// 0x80755428 (0x30), 0x80755464 (0x1E8).
extern unsigned char gSineTable[32];
extern signed char gFineSineTable[256];
extern unsigned int gPeriodTable[134];
extern unsigned int gITPeriodTable[144]; // Guessed name, 0x806EE380
extern MusicSample gDummySample;
extern MusicVirtualChannel gDummyVirtualChannel;
extern MusicInstrument gDummyInstrument; // 0x8075589C (G2)

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

// G2MEAB: 0x244 bytes (memset in MusicSong::play 0x8060CA08, gDummyChannel 0x80755658). The head node is the
// 0x1E8 MusicVirtualChannel; play stores mPan +0x1F8 and mGlobalVolume +0x204. The remaining members keep the
// Gormiti order without its mMasterVolume (one word shorter); offsets past +0x204 are not yet verified.
struct MusicChannel
{
    MusicVirtualChannel mVirtualChannelHead; // offset 0x0
    unsigned char mInstrument; // offset 0x1E8
    unsigned char mNote; // offset 0x1E9
    unsigned char mSample; // offset 0x1EA
    unsigned char mRealNote; // offset 0x1EB
    int mPeriod; // offset 0x1EC
    unsigned char mRecentEffect; // offset 0x1F0
    int mVolume; // offset 0x1F4
    int mPan; // offset 0x1F8
    int mVolumeDelta; // offset 0x1FC
    unsigned int mSampleOffset; // offset 0x200
    int mGlobalVolume; // offset 0x204
    unsigned char mPortaUpDown; // offset 0x208
    unsigned char mPortaDown; // offset 0x209
    unsigned char mPortaUp; // offset 0x20A
    unsigned char mXtraPortaDown; // offset 0x20B
    unsigned char mXtraPortaUp; // offset 0x20C
    unsigned char mVolumeSlide; // offset 0x20D
    unsigned char mPanSlide; // offset 0x20E
    unsigned char mRetrigX; // offset 0x20F
    unsigned char mRetrigY; // offset 0x210
    unsigned char mRetrigCount; // offset 0x211
    int mPortaTarget; // offset 0x214
    unsigned char mPortaSpeed; // offset 0x218
    unsigned char mPortaReached; // offset 0x219
    signed char mVibPos; // offset 0x21A
    unsigned char mVibSpeed; // offset 0x21B
    unsigned char mVibDepth; // offset 0x21C
    unsigned char mVibType; // offset 0x21D
    signed char mTremoloPosition; // offset 0x21E
    unsigned char mTremoloSpeed; // offset 0x21F
    unsigned char mTremoloDepth; // offset 0x220
    int mPanbrelloPos; // offset 0x224
    unsigned char mPanbrelloSpeed; // offset 0x228
    unsigned char mPanbrelloDepth; // offset 0x229
    unsigned char mTremorPosition; // offset 0x22A
    unsigned char mTremorOn; // offset 0x22B
    unsigned char mTremorOff; // offset 0x22C
    unsigned char mArpeggio; // offset 0x22D
    int mPatternLoopRow; // offset 0x230
    int mPatternLoopNumber; // offset 0x234
    unsigned char mChannelVolumeSlide; // offset 0x238
    unsigned char mSpecialParam; // offset 0x239
    unsigned char mWaveControl; // offset 0x23A
    unsigned char mWaveControlVibrato; // offset 0x23B
    unsigned char mWaveControlTremolo; // offset 0x23C
    unsigned char mWaveControlPan; // offset 0x23D
    unsigned char mFineVolumeSlideDown; // offset 0x23E
    unsigned char mFineVolumeSlideUp; // offset 0x23F
    unsigned char mFinePortaUp; // offset 0x240
    unsigned char mFinePortaDown; // offset 0x241
    unsigned char mHighOffset; // offset 0x242
    unsigned char mVolumeColumnVolumeSlide; // offset 0x243
};

// G2MEAB: 0x594 bytes (gDummyInstrument 0x8075589C); Gormiti order with the 0x30 MusicSample, offsets
// past mSample are not yet verified.
struct MusicInstrument
{
    signed char mName[28]; // offset 0x0
    int mNumSamples; // offset 0x1C
    MusicSample mSample[16]; // offset 0x20
    unsigned char mKeyMap[96]; // offset 0x320
    unsigned char mVolumeType; // offset 0x380
    unsigned char mVolumeNumPoints; // offset 0x381
    unsigned short mVolumePoints[40]; // offset 0x382
    unsigned char mVolumeSustain; // offset 0x3D2
    unsigned char mVolumeLoopStart; // offset 0x3D3
    unsigned char mVolumeLoopEnd; // offset 0x3D4
    unsigned char mVolumeSustainLoopStart; // offset 0x3D5
    unsigned char mVolumeSustainLoopEnd; // offset 0x3D6
    unsigned char mPanType; // offset 0x3D7
    unsigned char mPanNumPoints; // offset 0x3D8
    unsigned short mPanPoints[40]; // offset 0x3DA
    unsigned char mPanSustain; // offset 0x42A
    unsigned char mPanLoopStart; // offset 0x42B
    unsigned char mPanLoopEnd; // offset 0x42C
    unsigned char mPanSustainLoopStart; // offset 0x42D
    unsigned char mPanSustainLoopEnd; // offset 0x42E
    unsigned char mPitchType; // offset 0x42F
    unsigned char mPitchNumpoints; // offset 0x430
    unsigned short mPitchPoints[40]; // offset 0x432
    unsigned char mPitchSustain; // offset 0x482
    unsigned char mPitchLoopStart; // offset 0x483
    unsigned char mPitchLoopEnd; // offset 0x484
    unsigned char mPitchSustainLoopStart; // offset 0x485
    unsigned char mPitchSustainLoopEnd; // offset 0x486
    unsigned char mVibratoType; // offset 0x487
    unsigned char mVibratoSweep; // offset 0x488
    unsigned char mVibratoDepth; // offset 0x489
    unsigned char mVibratoRate; // offset 0x48A
    unsigned short mVolumeFade; // offset 0x48C
    unsigned char mGlobalVolume; // offset 0x48E
    unsigned char mDefaultPan; // offset 0x48F
    unsigned char mNNA; // offset 0x490
    unsigned char mDupCheckType; // offset 0x491
    unsigned char mDupCheckAction; // offset 0x492
    unsigned char mPitchPanSep; // offset 0x493
    unsigned char mPitchPanCenter; // offset 0x494
    unsigned char mVolumeVariation; // offset 0x495
    unsigned char mPanVariation; // offset 0x496
    unsigned char mNoteTable[240]; // offset 0x497
    unsigned int mFilterCutOff; // offset 0x588
    unsigned int mFilterResonance; // offset 0x58C
    unsigned char mMIDIOutput; // offset 0x590
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
    void * pMixPlugin; // offset 0x150 (with the G2MEAB 0x150 ChannelGroupI)
    _SNDMIXPLUGININFO Info; // offset 0x154
};

// G2MEAB: MusicSong is the shared tracker base of the IT/XM/S3M/MOD codecs, 0xA1C bytes (CodecXM and CodecIT
// data start at +0xA1C). Members follow the Gormiti order with sizeof(Codec) 0x1F4, a 0x150 ChannelGroupI and
// without Gormiti's mHardwareMusicChannel and mDSPTick: play 0x8060CA08, setBPM 0x8060CD9C, stop 0x8060CE2C,
// getLength/getPosition 0x8060D1C4/0x8060D228 and the IT open defaults 0x805CD074.
// The Gormiti ChannelMusic class and music-channel callbacks are absent: the G2MEAB codec description has no
// slots for them and the song has no hardware music channel.
struct MusicSong : public Codec
{
    FMOD_RESULT play();
    FMOD_RESULT spawnNewVirtualChannel(MusicChannel * cptr, MusicSample * sptr, MusicVirtualChannel * * newvcptr);
    FMOD_RESULT setBPM(int bpm);
    FMOD_RESULT stop();
    FMOD_RESULT playSound(MusicSample * sample, MusicVirtualChannel * vcptr, bool addfilter, _SNDMIXPLUGIN * plugin);
    FMOD_RESULT fineTune2Hz(unsigned char finetune, unsigned int * hz); // Guessed name
    FMOD_RESULT getLengthInternal(unsigned int * length, FMOD_TIMEUNIT lengthtype); // Guessed name
    FMOD_RESULT getPositionInternal(unsigned int * position, FMOD_TIMEUNIT postype); // Guessed name
    static FMOD_RESULT getLengthCallback(FMOD_CODEC_STATE * codec_state, unsigned int * length, FMOD_TIMEUNIT lengthtype);
    static FMOD_RESULT getPositionCallback(FMOD_CODEC_STATE * codec_state, unsigned int * position, FMOD_TIMEUNIT postype);

    char mSongName[256]; // offset 0x1F4
    MusicPattern * mPattern; // offset 0x2F4
    DSPI * mDSPHead; // offset 0x2F8
    bool * mVisited; // offset 0x2FC
    unsigned char mOrderList[256]; // offset 0x300
    int mNumChannels; // offset 0x400
    MusicChannel * mMusicChannel[64]; // offset 0x404
    int mNumVirtualChannels; // offset 0x504
    MusicVirtualChannel * mVirtualChannel; // offset 0x508
    ChannelPool * mChannelPool; // offset 0x50C
    ChannelSoftware * mChannelSoftware; // offset 0x510
    DSPI * * mLowPass; // offset 0x514
    ChannelGroupI mChannelGroup; // offset 0x518
    int mMixerSamplesLeft; // offset 0x668
    int mMixerSamplesPerTick; // offset 0x66C
    unsigned int mPCMOffset; // offset 0x670
    int mDefaultSpeed; // offset 0x674
    unsigned int mDefaultBPM; // offset 0x678
    unsigned char mDefaultPan[64]; // offset 0x67C
    unsigned char mDefaultVolume[64]; // offset 0x6BC
    unsigned char mDefaultGlobalVolume; // offset 0x6FC
    int mNumOrders; // offset 0x700
    int mNumPatterns; // offset 0x704
    int mNumPatternsMem; // offset 0x708
    int mNumInstruments; // offset 0x70C
    int mNumSamples; // offset 0x710
    MusicInstrument * mInstrument; // offset 0x714
    signed char * mPatternPtr; // offset 0x718
    unsigned char mLastNote[64]; // offset 0x71C
    unsigned char mLastNumber[64]; // offset 0x75C
    unsigned char mLastVolume[64]; // offset 0x79C
    unsigned char mLastEffect[64]; // offset 0x7DC
    unsigned char mLastEffectParam[64]; // offset 0x81C
    unsigned char mPreviousMaskVariable[64]; // offset 0x85C
    MusicNote mNote[64]; // offset 0x89C
    int mRestart; // offset 0x9DC
    float mMasterSpeed; // offset 0x9E0
    float mPanSeparation; // offset 0x9E4
    int mMasterVolume; // offset 0x9E8
    int mGlobalVolume; // offset 0x9EC
    unsigned char mGlobalVolumeSlide; // offset 0x9F0
    unsigned short mMusicFlags; // offset 0x9F2
    bool mPlaying; // offset 0x9F4
    bool mFinished; // offset 0x9F5
    bool mLooping; // offset 0x9F6
    int mTick; // offset 0x9F8
    int mSpeed; // offset 0x9FC
    int mBPM; // offset 0xA00
    int mRow; // offset 0xA04
    int mOrder; // offset 0xA08
    int mPatternDelay; // offset 0xA0C
    int mPatternDelayTicks; // offset 0xA10
    int mNextRow; // offset 0xA14
    int mNextOrder; // offset 0xA18
};

} // namespace FMOD

#endif
