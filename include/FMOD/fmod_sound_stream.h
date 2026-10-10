// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SOUND_STREAM_H
#define _FMOD_SOUND_STREAM_H

#include "fmod.h"
#include "fmod_soundi.h"

namespace FMOD {
    struct ChannelStream;
    struct Sample;
    struct Stream;
}

namespace FMOD {

struct Stream : public SoundI
{
    ChannelStream * mChannel; // offset 0xCC
    Sample * mSample; // offset 0xD0
    unsigned int mLastPos; // offset 0xD4
    bool mFinished; // offset 0xD8
    int mBlockSize; // offset 0xDC
    int mLoopCountCurrent; // offset 0xE0
    bool mWantsToFlush; // offset 0xE4
    virtual bool isStream();
    Stream();
    FMOD_RESULT fill(unsigned int offset, unsigned int length);
    FMOD_RESULT flush();
    FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setLoopCount(int loopcount);
};

} // namespace FMOD

#endif
