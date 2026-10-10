// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelEmulated is the G2MEAB layout.

#ifndef _FMOD_CHANNEL_EMULATED_H
#define _FMOD_CHANNEL_EMULATED_H

#include "fmod.h"
#include "fmod_channel_real.h"

namespace FMOD {
    class ChannelEmulated;
    class DSPI;
    class Output;
    struct SystemI;
}

namespace FMOD {

// G2MEAB: 0x78 bytes, no members beyond ChannelReal (output initializer 0x8060EF40 allocates
// count * 0x78). Vtable 0x806E25F0 overrides only update (slot 7) and isVirtual (slot 30); the 4.06
// DSP head, init, alloc, close, getDSPHead and setSpeakerLevels overrides are absent.
class ChannelEmulated : public ChannelReal
{
public:
    ChannelEmulated();

    // ChannelReal
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT isVirtual(bool * isvirtual);
};

} // namespace FMOD

#endif
