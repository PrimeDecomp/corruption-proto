// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
class DSPConnection : public LinkedListNode
{
public:
    LinkedListNode mInputNode; // offset 0xC
    LinkedListNode mOutputNode; // offset 0x18
private:
    short mMaxOutputLevels; // offset 0x24
    short mMaxInputLevels; // offset 0x26
    float mLevel[8][8]; // offset 0x28
    float mLevelCurrent[8][8]; // offset 0x128
    float mLevelDelta[8][8]; // offset 0x228
    float mNewPan; // offset 0x328
    float mPan; // offset 0x32C
public:
    DSPI * mInputUnit; // offset 0x330
    DSPI * mOutputUnit; // offset 0x334
    int mRampCount; // offset 0x338
    float mVolume; // offset 0x33C
    bool mSetLevelsUsed; // offset 0x340
    unsigned int mMramAddress; // offset 0x344
    unsigned int mInputUnitSize; // offset 0x348
    unsigned int mOutputUnitSize; // offset 0x34C
    FMOD_RESULT init(float * & levelmemory, int maxoutputlevels, int maxinputlevels);
    FMOD_RESULT mix(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length);
    FMOD_RESULT mixAndRamp(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length);
    FMOD_RESULT reset();
    FMOD_RESULT setUnity();
    FMOD_RESULT rampTo();
    FMOD_RESULT checkUnity(int outchannels, int inchannels);
    FMOD_RESULT setPan(float pan);
    FMOD_RESULT getPan(float *);
    FMOD_RESULT updatePan(int outchannels, int inchannels, FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT setMix(float volume);
    FMOD_RESULT getMix(float * volume);
    FMOD_RESULT setLevels(float * levels, int numinputlevels);
    FMOD_RESULT getLevels(float * levels, int numinputlevels);
};

} // namespace FMOD

#endif
