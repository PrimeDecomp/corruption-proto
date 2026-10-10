// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OUTPUT_SOFTWARE_H
#define _FMOD_OUTPUT_SOFTWARE_H

#include "fmod.h"
#include "fmod_outputi.h"

struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_OUTPUT_STATE;
namespace FMOD {
    class ChannelSoftware;
    class OutputSoftware;
    struct Sample;
}

namespace FMOD {

class OutputSoftware : public Output
{
    ChannelSoftware * mChannel; // offset 0xD4 (G2MEAB: Output is 0xD4 bytes)
public:
    OutputSoftware();
    virtual FMOD_RESULT init(int maxchannels);
    virtual FMOD_RESULT release();
    FMOD_RESULT createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample);
    int getSampleMaxChannels(FMOD_MODE mode, FMOD_SOUND_FORMAT format);
    static int getSampleMaxChannelsCallback(FMOD_OUTPUT_STATE * output, FMOD_MODE mode, FMOD_SOUND_FORMAT format);
};

} // namespace FMOD

#endif
