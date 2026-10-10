// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CHANNEL_EMULATED_H
#define _FMOD_CHANNEL_EMULATED_H

#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_dsp_filter.h"

namespace FMOD {
    class ChannelEmulated;
    class DSPI;
    class Output;
    struct SystemI;
}

namespace FMOD {

class ChannelEmulated : public ChannelReal
{
    DSPFilter mDSPHeadMemory; // offset 0x78
    char mPad[16]; // offset 0x18C
    DSPI * mDSPHead; // offset 0x19C
public:
    ChannelEmulated();
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT close();
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT isVirtual(bool * isvirtual);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
};

} // namespace FMOD

#endif
