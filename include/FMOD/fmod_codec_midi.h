// G2MEAB MIDI codec (DLS-driven software synth). No FMOD Ex 4.06 or Gormiti reference names these classes;
// class, member and method names are descriptive.
// G2MEAB: sizeof(CodecMIDI) = 0x2B74 (descriptor mSize, 0x805D12C8). Open 0x805D3C20 allocates mNumTracks
// 0x20-byte tracks (+0x2948) and mNumVoices 0x210-byte voices (+0x354); the codec embeds 16 0x25C-byte
// channels at +0x358 (reset 0x805D3A64).

#ifndef _FMOD_CODEC_MIDI_H
#define _FMOD_CODEC_MIDI_H

#include "fmod.h"
#include "fmod_channelgroupi.h"
#include "fmod_channeli.h"
#include "fmod_codeci.h"
#include "fmod_linkedlist.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    class ChannelPool;
    class ChannelSoftware;
    struct CodecMIDI;
    class DSPI;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct MIDIChannel;
    struct SoundI;
}

namespace FMOD {

// 0x20 bytes (open 0x805D3FF4 fills mMIDI/mData/mPosition/mLength/mIndex, reset 0x805D3A64 clears the rest).
struct MIDITrack // Guessed name
{
    FMOD_RESULT readByte(unsigned char * value); // Guessed name
    FMOD_RESULT readVarLen(unsigned int * value); // Guessed name
    FMOD_RESULT read(void * buffer, unsigned int length); // Guessed name
    FMOD_RESULT readMetaData(const char * name, unsigned int length, bool store); // Guessed name
    FMOD_RESULT process(bool audible); // Guessed name (0x805D3304)

    CodecMIDI * mMIDI; // offset 0x0, Guessed name
    unsigned char * mData; // offset 0x4, Guessed name
    unsigned int mPosition; // offset 0x8, Guessed name
    unsigned int mLength; // offset 0xC, Guessed name
    int mIndex; // offset 0x10, Guessed name
    unsigned char mUnk14; // offset 0x14
    float mUnk18; // offset 0x18
    bool mFinished; // offset 0x1C, Guessed name
    unsigned char mUnk1D; // offset 0x1D
    unsigned char mUnk1E; // offset 0x1E
};

// One 0xC-byte envelope segment; MIDIVoice update 0x805D1DEC interpolates mStart..mEnd over mTime.
struct MIDIEnvelopeStage // Guessed name
{
    float mTime; // offset 0x0
    float mStart; // offset 0x4
    float mEnd; // offset 0x8
};

// 0x38 bytes, two embedded in MIDIVoice (+0x164 volume, +0x19C pitch); 0x805D17EC initializes both.
struct MIDIEnvelope // Guessed name
{
    MIDIEnvelopeStage mStage[3]; // offset 0x0, attack/decay/release
    int mState; // offset 0x24, current stage, 3 when finished
    float mTime; // offset 0x28
    float mSustain; // offset 0x2C
    float mDepth; // offset 0x30, pitch envelope range (0x805D2374)
    bool mActive; // offset 0x34
};

// 0x210 bytes. The node links the voice into its MIDIChannel's voice list or the codec's free list.
struct MIDIVoice : public LinkedListNode // Guessed name
{
    float timeCentsToTime(int timecents); // Guessed name (0x805D1758)
    FMOD_RESULT stop(); // Guessed name
    FMOD_RESULT updateVolume(); // Guessed name (0x805D1DEC)
    FMOD_RESULT updatePitch(); // Guessed name (0x805D2190)
    FMOD_RESULT updatePan(); // Guessed name

    CodecMIDI * mMIDI; // offset 0x14, Guessed name
    ChannelI mChannel; // offset 0x18, Guessed name
    void * mUnk154; // offset 0x154, non-zero while the voice sounds
    float mLFODelay; // offset 0x158, Guessed name
    float mLFOTime; // offset 0x15C, advanced by the tick length each update, Guessed name
    float mLFOFrequency; // offset 0x160, Guessed name
    MIDIEnvelope mVolumeEnvelope; // offset 0x164, DLS EG1 (updateVolume 0x805D1DEC), Guessed name
    MIDIEnvelope mPitchEnvelope; // offset 0x19C, DLS EG2 (updatePitch 0x805D2190), Guessed name
    int mUnk1D4; // offset 0x1D4, -1 when stopped
    SoundI * mSound; // offset 0x1D8, getDefaults supplies the base frequency, Guessed name
    unsigned char mKey; // offset 0x1DC, Guessed name
    unsigned char mUnityNote; // offset 0x1DD, Guessed name
    int mFineTune; // offset 0x1E0, cents, Guessed name
    unsigned char mVelocity; // offset 0x1E4, Guessed name
    unsigned char mUnk1E5[0x1F0 - 0x1E5]; // offset 0x1E5
    float mLFOVolumeDepth; // offset 0x1F0, Guessed name
    float mLFOPitchDepth; // offset 0x1F4, cents, Guessed name
    unsigned char mUnk1F8[0x1FC - 0x1F8]; // offset 0x1F8
    bool mKeyOff; // offset 0x1FC, Guessed name
    int mScaleTuning; // offset 0x200, 0x3200 at note start (0x805D17EC), Guessed name
    unsigned char mUnk204[0x208 - 0x204]; // offset 0x204
    float mGain; // offset 0x208, Guessed name
    MIDIChannel * mMIDIChannel; // offset 0x20C, Guessed name
};

// 0x25C bytes, 16 embedded in CodecMIDI.
struct MIDIChannel // Guessed name
{
    FMOD_RESULT update(); // Guessed name

    unsigned char mUnk0[0x14]; // offset 0x0
    MIDITrack * mTrack; // offset 0x14, Guessed name
    LinkedListNode mVoiceHead; // offset 0x18, Guessed name
    unsigned char mUnk2C[0x228 - 0x2C]; // offset 0x2C
    unsigned char mUnk228; // offset 0x228, reset to channel index + 1
    int mUnk22C; // offset 0x22C
    int mUnk230; // offset 0x230
    unsigned char mUnk234; // offset 0x234
    int mUnk238; // offset 0x238, reset -1
    unsigned char mUnk23C[0x240 - 0x23C]; // offset 0x23C
    int mPitchBend; // offset 0x240, -8192..8191, Guessed name
    int mPitchBendRange; // offset 0x244, reset 0x200 (semitones * 256), Guessed name
    bool mSustainPedal; // offset 0x248, Guessed name
    unsigned char mUnk249[0x250 - 0x249]; // offset 0x249
    int mVolume; // offset 0x250, reset 100, Guessed name
    int mPan; // offset 0x254, reset 64, Guessed name
    int mExpression; // offset 0x258, reset 127, Guessed name
};

struct CodecMIDI : public Codec // Guessed name
{
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT update(bool audible); // Guessed name
    FMOD_RESULT play(); // Guessed name
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec_state, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec_state);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec_state, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec_state, int subsound, unsigned int position, FMOD_TIMEUNIT postype);

    DSPI * mDSPHead; // offset 0x1F4, Guessed name
    ChannelPool * mChannelPool; // offset 0x1F8, Guessed name
    ChannelSoftware * mChannelSoftware; // offset 0x1FC, Guessed name
    int mNumVoices; // offset 0x200, Guessed name
    ChannelGroupI mChannelGroup; // offset 0x204, Guessed name
    MIDIVoice * mVoice; // offset 0x354, Guessed name
    MIDIChannel mMIDIChannel[16]; // offset 0x358, Guessed name
    unsigned char mUnk2918[0x2920 - 0x2918]; // offset 0x2918
    unsigned int mMixerSamplesLeft; // offset 0x2920, readInternal 0x805D48B8, Guessed name
    unsigned int mMixerSamplesPerTick; // offset 0x2924, Guessed name
    float mUnk2928; // offset 0x2928, added to mUnk2950 each update
    unsigned int mPCMOffset; // offset 0x292C, Guessed name
    int mNumTracks; // offset 0x2930, Guessed name
    unsigned char mUnk2934[0x2948 - 0x2934]; // offset 0x2934
    MIDITrack * mTrack; // offset 0x2948, Guessed name
    unsigned char mUnk294C[0x2950 - 0x294C]; // offset 0x294C
    float mUnk2950; // offset 0x2950
    float mTickLength; // offset 0x2954, added to the voice timers each update, Guessed name
    SoundI * mDLS; // offset 0x2958, released through SoundI::release, Guessed name
    void * mUnk295C; // offset 0x295C, mDLS +0x138 (open 0x805D3E9C)
    unsigned char mUnk2960[0x2964 - 0x2960]; // offset 0x2960
    LinkedListNode mFreeVoiceHead; // offset 0x2964, Guessed name
    unsigned char mUnk2978[0x2B74 - 0x2978]; // offset 0x2978
};

} // namespace FMOD

#endif
