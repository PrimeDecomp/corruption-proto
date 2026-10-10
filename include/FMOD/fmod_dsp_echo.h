// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_DSP_ECHO_H
#define _FMOD_DSP_ECHO_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_dsp_echo.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
// The 4.06 DWARF has no type entry. G2MEAB: getDescriptionEx (0x805F4AB8) stores mSize 0x154;
// members from setParameterInternal 0x805F5474 (switch stores), getParameterInternal 0x805F55BC (formats),
// releaseInternal/resetInternal 0x805F4C1C/0x805F4C74 and readInternal 0x805F4CB8.
class DSPEcho : public DSPFilter
{
    float mDelay; // offset 0x124, Guessed name (param 0 "Delay", ms)
    float mDecayRatio; // offset 0x128, Guessed name (param 1 "Decay")
    float mDryMix; // offset 0x12C, Guessed name (param 3 "Drymix")
    float mWetMix; // offset 0x130, Guessed name (param 4 "Wetmix")
    int mMaxChannels; // offset 0x134, Guessed name (param 2 "Max channels")
    float * mEchoBuffer; // offset 0x138, Guessed name
    unsigned int mEchoBufferLengthBytes; // offset 0x13C, Guessed name (memset/calloc length)
    unsigned int mEchoPosition; // offset 0x140, Guessed name
    unsigned int mEchoLength; // offset 0x144, Guessed name (frames)
    unsigned int mUnk148; // offset 0x148, no G2MEAB access
    int mOutputRate; // offset 0x14C, Guessed name (getSoftwareFormat samplerate)
    int mChannels; // offset 0x150, Guessed name (getSoftwareFormat numoutputchannels)

public:
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT releaseCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT resetCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
