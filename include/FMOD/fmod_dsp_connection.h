// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information; G2MEAB layout (group D).

#ifndef _FMOD_DSP_CONNECTION_H
#define _FMOD_DSP_CONNECTION_H

#include "fmod.h"
#include "fmod_linkedlist.h"

namespace FMOD {
    class DSPI;
}

namespace FMOD {

const int DSP_PAN_OFF = -1;
const int DSP_RAMPCOUNT = 64;
// G2MEAB layout, sizeof 0x78 (DSPConnectionPool stride 0x805F4110). The node base gets vtable 0x806EC5D8;
// the weak deleting dtor 0x805F44A0 is emitted in fmod_dsp_connectionpool. Level rows hold 8 floats each
// and point into the pool's level memory (init 0x805F1364).
class DSPConnection : public LinkedListNode
{
public:
    int mMaxInputLevels; // offset 0x14, init clamps to >= 2
    float * mLevel[2]; // offset 0x18
    float * mLevelCurrent[2]; // offset 0x20
    float * mLevelDelta[2]; // offset 0x28
    float mNewPan; // offset 0x30, setPan 0x805F2EB8
    float mPan; // offset 0x34
    int mUnk38; // offset 0x38, unresolved
    LinkedListNode mInputNode; // offset 0x3C
    LinkedListNode mOutputNode; // offset 0x50
    DSPI * mInputUnit; // offset 0x64
    DSPI * mOutputUnit; // offset 0x68
    int mRampCount; // offset 0x6C, setUnity 0x805F2C74 stores DSP_RAMPCOUNT
    float mVolume; // offset 0x70
    bool mSetLevelsUsed; // offset 0x74

    FMOD_RESULT init(float * & levelmemory, int maxinputlevels);
    FMOD_RESULT reset();
    FMOD_RESULT mix(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length);
    FMOD_RESULT mixAndRamp(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length);
    FMOD_RESULT rampTo();
    FMOD_RESULT checkUnity(int outchannels, int inchannels);
    FMOD_RESULT setPan(float pan);
    FMOD_RESULT updatePan(int outchannels, int inchannels, FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT setMix(float volume);
    FMOD_RESULT getMix(float * volume);
    FMOD_RESULT setLevels(float * levels, int numinputlevels);
    FMOD_RESULT getLevels(float * levels); // G2MEAB 0x805F3E9C takes no count
};

} // namespace FMOD

#endif
