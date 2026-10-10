// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OUTPUT_EMULATED_H
#define _FMOD_OUTPUT_EMULATED_H

#include "fmod.h"
#include "fmod_outputi.h"

namespace FMOD {
    class ChannelEmulated;
    class OutputEmulated;
}

namespace FMOD {

class OutputEmulated : public Output
{
    ChannelEmulated * mChannel; // offset 0xC0
public:
    OutputEmulated();
    FMOD_RESULT init(int maxchannels);
    virtual FMOD_RESULT release();
    FMOD_RESULT update();
};

} // namespace FMOD

#endif
