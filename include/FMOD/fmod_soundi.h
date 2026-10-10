// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SOUNDI_H
#define _FMOD_SOUNDI_H

#include "fmod.h"
#include "fmod_linkedlist.h"

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
struct SoundI : public LinkedListNode
{
    FMOD_SOUND_TYPE mType; // offset 0x10
    FMOD_SOUND_FORMAT mFormat; // offset 0x14
    FMOD_MODE mMode; // offset 0x18
    char * mName; // offset 0x1C
    unsigned int mPosition; // offset 0x20
    unsigned int mLength; // offset 0x24
    unsigned int mLengthBytes; // offset 0x28
    unsigned int mLoopStart; // offset 0x2C
    unsigned int mLoopLength; // offset 0x30
    int mLoopCount; // offset 0x34
    Codec * mCodec; // offset 0x38
    int mChannels; // offset 0x3C
    float mDefaultVolume; // offset 0x40
    float mDefaultFrequency; // offset 0x44
    float mDefaultPan; // offset 0x48
    int mDefaultPriority; // offset 0x4C
    unsigned int mDefaultChannelMask; // offset 0x50
    float mFrequencyVariation; // offset 0x54
    float mVolumeVariation; // offset 0x58
    float mPanVariation; // offset 0x5C
    float mMinDistance; // offset 0x60
    float mMaxDistance; // offset 0x64
    float mConeInsideAngle; // offset 0x68
    float mConeOutsideAngle; // offset 0x6C
    float mConeOutsideVolume; // offset 0x70
    FMOD_VECTOR * mRolloffPoint; // offset 0x74
    int mNumRolloffPoints; // offset 0x78
    SoundI * * mSubSound; // offset 0x7C
    bool mSubSoundShared; // offset 0x80
    int mNumSubSounds; // offset 0x84
    int mNumActiveSubSounds; // offset 0x88
    SoundI * mSubSoundParent; // offset 0x8C
    int mSubSoundIndex; // offset 0x90
    int * mSubSoundList; // offset 0x94
    int mSubSoundListNum; // offset 0x98
    int mSubSoundListCurrent; // offset 0x9C
    SoundI * mSubSampleParent; // offset 0xA0
    void * mUserData; // offset 0xA4
    SystemI * mSystem; // offset 0xA8
    unsigned int mMemoryUsed; // offset 0xAC
    int mNumSyncPoints; // offset 0xB0
    SyncPoint * mSyncPointHead; // offset 0xB4
    AsyncData * mAsyncData; // offset 0xB8
    FMOD_OPENSTATE mOpenState; // offset 0xBC
    FMOD_SOUND_PCMREADCALLBACK mPostReadCallback; // offset 0xC0
    FMOD_SOUND_PCMSETPOSCALLBACK mPostSetPositionCallback; // offset 0xC4
    FMOD_SOUND * mPostCallbackSound; // offset 0xC8
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
