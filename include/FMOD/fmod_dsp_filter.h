// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_FILTER_H
#define _FMOD_DSP_FILTER_H

#include "fmod.h"
#include "fmod_dspi.h"

namespace FMOD {

class DSPFilter : public DSPI
{
    float * mHistoryBuffer; // offset 0x104
    unsigned int mHistoryPosition; // offset 0x108
    unsigned int mHistoryLength; // offset 0x10C
    int mBufferChannels; // offset 0x110
public:
    virtual FMOD_RESULT release(bool freethis);
    FMOD_RESULT startBuffering(unsigned int length);
    FMOD_RESULT getHistoryBuffer(float * * buffer, unsigned int * position, unsigned int * length);
    FMOD_RESULT stopBuffering();
};

} // namespace FMOD

#endif
