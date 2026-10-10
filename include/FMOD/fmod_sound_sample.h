// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Sample is the G2MEAB layout.

#ifndef _FMOD_SOUND_SAMPLE_H
#define _FMOD_SOUND_SAMPLE_H

#include "fmod.h"
#include "fmod_soundi.h"

namespace FMOD {
    struct Sample;
}

namespace FMOD {

// G2MEAB: 0x37C bytes, vtable 0x806EEF18. Sample() 0x80612B54 clears mNumSubSamples and sets
// mLockCanRead. The vtable ends with lockInternal and unlockInternal; 4.06 setBufferData is absent.
struct Sample : public SoundI
{
    int mNumSubSamples; // offset 0x348
    Sample * mSubSample[8]; // offset 0x34C
    void * mLockBuffer; // offset 0x36C
    unsigned int mLockLength; // offset 0x370
    unsigned int mLockOffset; // offset 0x374
    bool mLockCanRead; // offset 0x378
    virtual FMOD_RESULT lockInternal(unsigned int, unsigned int, void * *, void * *, unsigned int *, unsigned int *);
    virtual FMOD_RESULT unlockInternal(void *, void *, unsigned int, unsigned int);
    Sample();
    virtual FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    virtual FMOD_RESULT release();
    virtual FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority);
    virtual FMOD_RESULT setVariations(float frequencyvar, float volumevar, float panvar);
    virtual FMOD_RESULT set3DMinMaxDistance(float min, float max);
    virtual FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
};

} // namespace FMOD

#endif
