// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Stream is the G2MEAB layout.

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

// G2MEAB: vtable 0x806EF030. Stream() 0x80615750 clears mSample and the bytes at +0x350/+0x35C, sets
// +0x354 to 1 and +0x358 to -1. Fields follow the 4.06 order without mLastPos.
struct Stream : public SoundI
{
    ChannelStream * mChannel; // offset 0x348
    Sample * mSample; // offset 0x34C
    bool mFinished; // offset 0x350
    int mBlockSize; // offset 0x354
    int mLoopCountCurrent; // offset 0x358
    bool mWantsToFlush; // offset 0x35C
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
