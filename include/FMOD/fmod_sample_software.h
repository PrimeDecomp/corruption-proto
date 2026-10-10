// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SAMPLE_SOFTWARE_H
#define _FMOD_SAMPLE_SOFTWARE_H

#include "fmod.h"
#include "fmod_sound_sample.h"

namespace FMOD {
    class SampleSoftware;
}

namespace FMOD {

class SampleSoftware : public Sample
{
    void * mBuffer; // offset 0x100
    void * mBufferMemory; // offset 0x104
    char * mLoopPointDataEnd; // offset 0x108
    char mLoopPointDataEndMemory[8]; // offset 0x10C
    bool mDataEndCopied; // offset 0x114
public:
    SampleSoftware();
    virtual FMOD_RESULT release(bool freethis);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
    virtual FMOD_RESULT lockInternal(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlockInternal(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    virtual FMOD_RESULT setBufferData(void * data);
    FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    FMOD_RESULT setLoopPointData();
    FMOD_RESULT restoreLoopPointData();
};

} // namespace FMOD

#endif
