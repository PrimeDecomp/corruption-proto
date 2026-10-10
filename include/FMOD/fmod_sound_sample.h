// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SOUND_SAMPLE_H
#define _FMOD_SOUND_SAMPLE_H

#include "fmod.h"
#include "fmod_soundi.h"

namespace FMOD {
    struct Sample;
}

namespace FMOD {

struct Sample : public SoundI
{
    int mNumSubSamples; // offset 0xCC
    Sample * mSubSample[8]; // offset 0xD0
    void * mLockBuffer; // offset 0xF0
    unsigned int mLockLength; // offset 0xF4
    unsigned int mLockOffset; // offset 0xF8
    bool mLockCanRead; // offset 0xFC
    virtual FMOD_RESULT lockInternal(unsigned int, unsigned int, void * *, void * *, unsigned int *, unsigned int *);
    virtual FMOD_RESULT unlockInternal(void *, void *, unsigned int, unsigned int);
    virtual FMOD_RESULT setBufferData(void *);
    Sample();
    virtual FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    virtual FMOD_RESULT release(bool freethis);
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
