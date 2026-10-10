// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNELPOOL_H
#define _FMOD_CHANNELPOOL_H

#include "fmod.h"

namespace FMOD {
    class ChannelPool;
    class ChannelReal;
    class DSPI;
    class Output;
    struct Sound;
    struct SystemI;
}

namespace FMOD {

class ChannelPool
{
protected:
    int mNumChannels; // offset 0x0
    int mChannelsUsed; // offset 0x4
    SystemI * mSystem; // offset 0x8
    Output * mOutput; // offset 0xC
public:
    ChannelReal * * mChannel; // offset 0x10
    ChannelPool();
    FMOD_RESULT init(SystemI * system, Output * output, int numchannels);
    FMOD_RESULT release();
    FMOD_RESULT find(int, Sound *, ChannelReal * *, int, bool);
    FMOD_RESULT allocateChannel(ChannelReal * * realchannel, int index, int numchannels, int * found_out);
    FMOD_RESULT getNumChannels(int * numchannels);
    FMOD_RESULT getChannelsUsed(int * numchannels);
    FMOD_RESULT setChannel(int index, ChannelReal * channel, DSPI * dspmixtarget);
    FMOD_RESULT getChannel(int index, ChannelReal * * channel);
};

} // namespace FMOD

#endif
