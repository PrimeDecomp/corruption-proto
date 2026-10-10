// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. SampleSoftware is the G2MEAB layout.

#ifndef _FMOD_SAMPLE_SOFTWARE_H
#define _FMOD_SAMPLE_SOFTWARE_H

#include "fmod.h"
#include "fmod_sound_sample.h"

namespace FMOD {
    class SampleSoftware;
}

namespace FMOD {

// G2MEAB: 0x484 bytes (OutputSoftware::createSample allocation), vtable 0x806EECE4. The constructor
// clears mBuffer and mBufferMemory. setLoopPointData saves bytes past the loop end into +0x404 and
// restoreLoopPointData copies them back. No setMode or setBufferData override (vtable slots).
class SampleSoftware : public Sample
{
    void * mBuffer; // offset 0x37C
    void * mBufferMemory; // offset 0x380
    unsigned char mUnk384[0x80]; // offset 0x384, unresolved (no access in fmod_sample_software)
    char mLoopPointDataEndMemory[0x80]; // offset 0x404
public:
    SampleSoftware();
    virtual FMOD_RESULT release(bool freethis);
    virtual FMOD_RESULT lockInternal(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlockInternal(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
    FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    FMOD_RESULT setLoopPointData();
    FMOD_RESULT restoreLoopPointData();
};

} // namespace FMOD

#endif
