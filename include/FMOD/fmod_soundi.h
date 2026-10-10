// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. SoundI is the G2MEAB layout.

#ifndef _FMOD_SOUNDI_H
#define _FMOD_SOUNDI_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_syncpoint.h"
#include "fmod_types.h"

struct FMOD_TAG;
struct FMOD_VECTOR;
namespace FMOD {
    class AsyncThread;
    struct Codec;
    struct Metadata;
    struct Sound;
    struct SoundI;
    struct SyncPoint;
    struct System;
    struct SystemI;
}

namespace FMOD {

const int DEFAULT_FREQUENCY = 44100;
const unsigned int SOUND_READCHUNKSIZE = 16384;
// G2MEAB layout (0x348) from SoundI() 0x80616098 and accesses in fmod_soundi, fmod_sound_sample,
// fmod_sample_software and fmod_sound_stream. The base node's vptr (+0x10) takes the SoundI vtable
// 0x806EF4DC; its slots follow the 4.06 virtual order after the inherited destructor. Differences from
// 4.06: an embedded 256-byte name, no mDefaultChannelMask, mSubSampleParent (set to this) ahead of the
// subsound block, an embedded SyncPoint head, a second node and several unresolved words.
struct SoundI : public LinkedListNode
{
    FMOD_SOUND_TYPE mType; // offset 0x14
    FMOD_SOUND_FORMAT mFormat; // offset 0x18
    FMOD_MODE mMode; // offset 0x1C
    char mName[256]; // offset 0x20
    unsigned int mPosition; // offset 0x120
    unsigned int mLength; // offset 0x124
    unsigned int mLengthBytes; // offset 0x128
    unsigned int mLoopStart; // offset 0x12C
    unsigned int mLoopLength; // offset 0x130
    int mLoopCount; // offset 0x134
    Codec * mCodec; // offset 0x138
    int mChannels; // offset 0x13C
    float mDefaultVolume; // offset 0x140
    float mDefaultFrequency; // offset 0x144
    float mDefaultPan; // offset 0x148
    int mDefaultPriority; // offset 0x14C
    float mFrequencyVariation; // offset 0x150
    float mVolumeVariation; // offset 0x154
    float mPanVariation; // offset 0x158
    float mMinDistance; // offset 0x15C
    float mMaxDistance; // offset 0x160
    float mConeInsideAngle; // offset 0x164
    float mConeOutsideAngle; // offset 0x168
    float mConeOutsideVolume; // offset 0x16C
    FMOD_VECTOR * mRolloffPoint; // offset 0x170
    int mNumRolloffPoints; // offset 0x174
    SoundI * mSubSampleParent; // offset 0x178, Guessed name (the constructor stores this, as 4.06 does)
    SoundI * * mSubSound; // offset 0x17C
    int mNumSubSounds; // offset 0x180
    int mNumActiveSubSounds; // offset 0x184
    SoundI * mSubSoundParent; // offset 0x188
    int mSubSoundIndex; // offset 0x18C
    int * mSubSoundList; // offset 0x190
    int mSubSoundListNum; // offset 0x194
    int mSubSoundListCurrent; // offset 0x198
    void * mUserData; // offset 0x19C
    SystemI * mSystem; // offset 0x1A0
    int mNumSyncPoints; // offset 0x1A4
    SyncPoint mSyncPointHead; // offset 0x1A8, embedded (4.06 holds a pointer)
    // 0x2C4-0x334 hold the 4.06 AsyncData fields inline (no separate allocation): getAsyncThread
    // 0x805B65F8 stores the thread at +0x2C4 and queues +0x2C8; threadFunc 0x805B6468 opens +0x2DC
    // (OPENMEMORY) or mName, passes +0x2E0 when +0x32C is set, stores the result at +0x334, sets
    // +0x330 to READY or ERROR and calls the exinfo nonblock callback. Names follow AsyncData.
    AsyncThread * mAsyncThread; // offset 0x2C4, Guessed name (AsyncData::mThread)
    LinkedListNode mAsyncNode; // offset 0x2C8, Guessed name (AsyncData::mNode)
    void * mAsyncNameData; // offset 0x2DC, Guessed name (AsyncData::mNameData)
    FMOD_CREATESOUNDEXINFO mExInfo; // offset 0x2E0, Guessed name (AsyncData::mExInfo)
    bool mExInfoExists; // offset 0x32C, Guessed name (AsyncData::mExInfoExists)
    FMOD_OPENSTATE mOpenState; // offset 0x330, Guessed name; Sound wrappers return NOTREADY while nonzero
    FMOD_RESULT mAsyncResult; // offset 0x334, Guessed name (AsyncData::mResult)
    Metadata * mMetadata; // offset 0x338; release 0x806175A8 calls Metadata::release on it, getNumTags 0x80618568 and getTag 0x806185D4 forward to it
    FMOD_SOUND_PCMREADCALLBACK mPostReadCallback; // offset 0x33C
    FMOD_SOUND_PCMSETPOSCALLBACK mPostSetPositionCallback; // offset 0x340
    FMOD_SOUND * mPostCallbackSound; // offset 0x344
    FMOD_RESULT updateSubSound(int);
    FMOD_RESULT getBytesFromSamples(unsigned int, unsigned int *);
    static FMOD_RESULT getBytesFromSamples(unsigned int samples, unsigned int * bytes, int channels, FMOD_SOUND_FORMAT format);
    FMOD_RESULT getSamplesFromBytes(unsigned int, unsigned int *);
    static FMOD_RESULT getSamplesFromBytes(unsigned int bytes, unsigned int * samples, int channels, FMOD_SOUND_FORMAT format);
    FMOD_RESULT getBitsFromFormat(int *);
    static FMOD_RESULT getBitsFromFormat(FMOD_SOUND_FORMAT format, int * bits);
    static FMOD_RESULT getFormatFromBits(int bits, FMOD_SOUND_FORMAT * format);
    virtual bool isStream() { return false; } // inline: weak copy 0x805BE2A8 in fmod_channeli after ChannelI::alloc (group B)
    static FMOD_RESULT validate(Sound * sound, SoundI * * soundi);
    SoundI();
    FMOD_RESULT loadSubSound(int index, FMOD_MODE mode);
    FMOD_RESULT read(unsigned int offset, unsigned int numsamples, unsigned int * read);
    FMOD_RESULT seek(int subsound, unsigned int position);
    FMOD_RESULT clear(unsigned int offset, unsigned int numsamples);
    FMOD_RESULT downmix(void * dest, void * src, FMOD_SOUND_FORMAT format, int channels, unsigned int length); // Guessed name, 0x806161BC; readData averages the codec channels into a mono sound
    virtual FMOD_RESULT release();
    virtual FMOD_RESULT getSystemObject(System * * system);
    virtual FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    virtual FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority);
    virtual FMOD_RESULT getDefaults(float * frequency, float * volume, float * pan, int * priority);
    virtual FMOD_RESULT setVariations(float frequencyvar, float volumevar, float panvar);
    virtual FMOD_RESULT getVariations(float * frequencyvar, float * volumevar, float * panvar);
    virtual FMOD_RESULT set3DMinMaxDistance(float min, float max);
    virtual FMOD_RESULT get3DMinMaxDistance(float * min, float * max);
    virtual FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    virtual FMOD_RESULT get3DConeSettings(float * insideconeangle, float * outsideconeangle, float * outsidevolume);
    virtual FMOD_RESULT set3DCustomRolloff(FMOD_VECTOR * points, int numpoints);
    virtual FMOD_RESULT get3DCustomRolloff(FMOD_VECTOR * * points, int * numpoints);
    virtual FMOD_RESULT setSubSound(int index, SoundI * subsound);
    virtual FMOD_RESULT getSubSound(int index, SoundI * * subsound);
    virtual FMOD_RESULT setSubSoundSentence(int * subsoundlist, int numsubsounds);
    virtual FMOD_RESULT getName(char * name, int namelen);
    virtual FMOD_RESULT getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype);
    virtual FMOD_RESULT getFormat(FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits);
    virtual FMOD_RESULT getNumSubSounds(int * numsubsounds);
    virtual FMOD_RESULT getNumTags(int * numtags, int * numtagsupdated);
    virtual FMOD_RESULT getTag(const char * name, int index, FMOD_TAG * tag);
    virtual FMOD_RESULT getOpenState(FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, bool * starving);
    virtual FMOD_RESULT readData(void * buffer, unsigned int numbytes, unsigned int * read);
    virtual FMOD_RESULT seekData(unsigned int position);
    virtual FMOD_RESULT getNumSyncPoints(int * numsyncpoints);
    virtual FMOD_RESULT getSyncPoint(int index, FMOD_SYNCPOINT * * point);
    virtual FMOD_RESULT getSyncPointInfo(FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype);
    virtual FMOD_RESULT addSyncPoint(unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * syncpoint);
    virtual FMOD_RESULT deleteSyncPoint(FMOD_SYNCPOINT * point);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
    virtual FMOD_RESULT getMode(FMOD_MODE * mode);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT getLoopCount(int * loopcount);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
    virtual FMOD_RESULT getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
    virtual FMOD_RESULT setPosition(unsigned int pos);
    FMOD_RESULT setPositionInternal(unsigned int pcm);
    virtual FMOD_RESULT getPosition(unsigned int * pcm);
    virtual FMOD_RESULT setUserData(void * userdata);
    virtual FMOD_RESULT getUserData(void * * userdata);
};

// Header inlines: expanded in Output::mix 0x8060EA70 and OutputNoSound::init 0x8060F30C (same case order
// as the 4.06 copies).
inline FMOD_RESULT SoundI::getBitsFromFormat(FMOD_SOUND_FORMAT format, int * bits)
{
    switch (format)
    {
        case FMOD_SOUND_FORMAT_PCM8:
            *bits = 8;
            break;
        case FMOD_SOUND_FORMAT_PCM16:
            *bits = 16;
            break;
        case FMOD_SOUND_FORMAT_PCM24:
            *bits = 24;
            break;
        case FMOD_SOUND_FORMAT_PCM32:
            *bits = 32;
            break;
        case FMOD_SOUND_FORMAT_PCMFLOAT:
            *bits = 32;
            break;
        case FMOD_SOUND_FORMAT_GCADPCM:
            *bits = 0;
            break;
        case FMOD_SOUND_FORMAT_IMAADPCM:
            *bits = 0;
            break;
        case FMOD_SOUND_FORMAT_XMA:
            *bits = 0;
            break;
        case FMOD_SOUND_FORMAT_VAG:
            *bits = 0;
            break;
        case FMOD_SOUND_FORMAT_MPEG:
            *bits = 0;
            break;
        case FMOD_SOUND_FORMAT_NONE:
            *bits = 0;
            break;
        default:
            return FMOD_ERR_FORMAT;
    }

    return FMOD_OK;
}

inline FMOD_RESULT SoundI::getBytesFromSamples(unsigned int samples, unsigned int * bytes, int channels, FMOD_SOUND_FORMAT format)
{
    int bits;

    getBitsFromFormat(format, &bits);

    if (bits)
    {
        *bytes = samples * bits / 8;
    }
    else
    {
        switch (format)
        {
            case FMOD_SOUND_FORMAT_GCADPCM:
                *bytes = (samples + 13) / 14 * 14 * 8 / 14;
                break;
            case FMOD_SOUND_FORMAT_IMAADPCM:
                *bytes = (samples + 63) / 64 * 64 * 36 / 64;
                break;
            case FMOD_SOUND_FORMAT_VAG:
                *bytes = (samples + 27) / 28 * 28 * 16 / 28;
                break;
            case FMOD_SOUND_FORMAT_XMA:
                *bytes = samples;
                return FMOD_OK;
            case FMOD_SOUND_FORMAT_MPEG:
                *bytes = samples;
                return FMOD_OK;
            case FMOD_SOUND_FORMAT_NONE:
                *bytes = 0;
                break;
            default:
                return FMOD_ERR_FORMAT;
        }
    }

    *bytes *= channels;

    return FMOD_OK;
}

// Expanded in ChannelReal::setPosition 0x805B7108 (group B): zero-channel guard, 64-bit __div2u for PCM
// formats, no MPEG case.
inline FMOD_RESULT SoundI::getSamplesFromBytes(unsigned int bytes, unsigned int * samples, int channels, FMOD_SOUND_FORMAT format)
{
    int bits;

    if (!channels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    getBitsFromFormat(format, &bits);

    if (bits)
    {
        *samples = (unsigned int)(((FMOD_UINT64)bytes * 8) / bits);
    }
    else
    {
        switch (format)
        {
            case FMOD_SOUND_FORMAT_GCADPCM:
                *samples = bytes * 14 / 8;
                break;
            case FMOD_SOUND_FORMAT_IMAADPCM:
                *samples = bytes * 64 / 36;
                break;
            case FMOD_SOUND_FORMAT_VAG:
                *samples = bytes * 28 / 16;
                break;
            case FMOD_SOUND_FORMAT_XMA:
                *samples = bytes;
                return FMOD_OK;
            case FMOD_SOUND_FORMAT_NONE:
                *samples = 0;
                break;
            default:
                return FMOD_ERR_FORMAT;
        }
    }

    *samples /= channels;

    return FMOD_OK;
}

// Expanded in SoundI::clear 0x806171D4, getLength 0x806181CC and getLoopPoints 0x8061975C (loads mFormat
// then mChannels).
inline FMOD_RESULT SoundI::getBytesFromSamples(unsigned int samples, unsigned int * bytes)
{
    return getBytesFromSamples(samples, bytes, mChannels, mFormat);
}

// Expanded in ChannelStream::setPosition 0x805BBA78 (group B; loads mFormat then mChannels).
inline FMOD_RESULT SoundI::getSamplesFromBytes(unsigned int bytes, unsigned int * samples)
{
    return getSamplesFromBytes(bytes, samples, mChannels, mFormat);
}

// Header inline (group F): expanded in CodecWav::openInternal 0x805E8940/0x805E8A30 and CodecAIFF
// openInternal 0x805C29E8/0x805C2A7C (jump table over bits 8..32).
inline FMOD_RESULT SoundI::getFormatFromBits(int bits, FMOD_SOUND_FORMAT * format)
{
    switch (bits)
    {
        case 8:
            *format = FMOD_SOUND_FORMAT_PCM8;
            break;
        case 16:
            *format = FMOD_SOUND_FORMAT_PCM16;
            break;
        case 24:
            *format = FMOD_SOUND_FORMAT_PCM24;
            break;
        case 32:
            *format = FMOD_SOUND_FORMAT_PCM32;
            break;
        default:
            return FMOD_ERR_FORMAT;
    }

    return FMOD_OK;
}

} // namespace FMOD

#endif
