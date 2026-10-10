// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. SoundI is the G2MEAB layout.

#ifndef _FMOD_SOUNDI_H
#define _FMOD_SOUNDI_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_syncpoint.h"

struct FMOD_TAG;
struct FMOD_VECTOR;
namespace FMOD {
    struct AsyncData;
    struct Codec;
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
    AsyncData * mAsyncData; // offset 0x2C4
    LinkedListNode mUnk2C8; // offset 0x2C8, unresolved
    int mUnk2DC; // offset 0x2DC, unresolved
    unsigned char mUnk2E0[0x4C]; // offset 0x2E0, unresolved (no access in the Sound TUs)
    bool mUnk32C; // offset 0x32C, unresolved
    FMOD_OPENSTATE mOpenState; // offset 0x330, Guessed name (4.06 constructor store position)
    int mUnk334; // offset 0x334, unresolved
    int mUnk338; // offset 0x338, unresolved
    FMOD_SOUND_PCMREADCALLBACK mPostReadCallback; // offset 0x33C
    FMOD_SOUND_PCMSETPOSCALLBACK mPostSetPositionCallback; // offset 0x340
    FMOD_SOUND * mPostCallbackSound; // offset 0x344
    FMOD_RESULT updateSubSound(int);
    FMOD_RESULT getBytesFromSamples(unsigned int, unsigned int *);
    FMOD_RESULT getBytesFromSamples(unsigned int, unsigned int *, int, FMOD_SOUND_FORMAT);
    FMOD_RESULT getSamplesFromBytes(unsigned int, unsigned int *);
    FMOD_RESULT getSamplesFromBytes(unsigned int, unsigned int *, int, FMOD_SOUND_FORMAT);
    FMOD_RESULT getBitsFromFormat(int *);
    FMOD_RESULT getBitsFromFormat(FMOD_SOUND_FORMAT, int *);
    FMOD_RESULT getFormatFromBits(int, FMOD_SOUND_FORMAT *);
    virtual bool isStream();
    static FMOD_RESULT validate(Sound * sound, SoundI * * soundi);
    SoundI();
    FMOD_RESULT loadSubSound(int index, FMOD_MODE mode);
    FMOD_RESULT read(unsigned int offset, unsigned int numsamples, unsigned int * read);
    FMOD_RESULT seek(int subsound, unsigned int position);
    FMOD_RESULT clear(unsigned int offset, unsigned int numsamples);
    virtual FMOD_RESULT release(bool freethis);
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

} // namespace FMOD

#endif
