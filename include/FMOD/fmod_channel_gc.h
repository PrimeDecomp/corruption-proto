// G2MEAB GameCube hardware channel (no 4.06 counterpart). Layout from fmod_channel_gc.cpp 0x80621870..0x80622C90.

#ifndef _FMOD_CHANNEL_GC_H
#define _FMOD_CHANNEL_GC_H

#include "fmod.h"
#include "fmod_channel_realmanual3d.h"

#include <dolphin/ax.h>

struct FMOD_REVERB_CHANNELPROPERTIES;
namespace FMOD {
    class DSPI;
    class Output;
    class OutputGC;
    struct SystemI;
}

namespace FMOD {

// Guessed name. The 256-entry linear-to-dB (0.1 dB units, -960..0) table at .data 0x806EF7C0, indexed
// by (int)(volume * 255) in setVolume 0x80622040 and by the reverb room level in setReverbProperties.
extern int gGCVolumeTable[256];

// Guessed name. G2MEAB: sizeof 0xA0 (OutputGC::init places the channel array with stride 0xA0, group C).
// Ctor 0x80621870 runs the ChannelReal ctor, stores the ChannelRealManual3D vtable 0x806E27A0 and then
// 0x806EFC70 at +0x74, and clears +0x7C/+0x80/+0x88/+0x89/+0x8C/+0x90. init 0x806218D4 stores the output
// +0x78 and the AX voice +0x7C; start 0x806219BC stores the voice start address +0x9C, sets +0x88 and
// clears the pending position +0x98; setVolume 0x80622040 stores the MIX input level +0x84;
// setReverbProperties 0x80622A70 stores the room levels +0x8C/+0x90. +0x94 has no access in this TU.
// Members are public: the OutputGC AX frame callback and SampleGC::unlock (group C) use them.
class ChannelGC : public ChannelRealManual3D
{
public:
    OutputGC * mOutputGC; // offset 0x78, Guessed name (ChannelReal::mOutput at +0x34 is the same object)
    AXVPB * mVoice; // offset 0x7C, Guessed name
    bool mUnk80; // offset 0x80, unresolved (cleared by the ctor and init only)
    int mInput; // offset 0x84, Guessed name: MIX input level in 0.1 dB
    bool mPendingStart; // offset 0x88, Guessed name: applied by updateStart from the AX frame callback
    bool mPendingStop; // offset 0x89, Guessed name: applied by updateStop from the AX frame callback
    int mReverbRoomA; // offset 0x8C, Guessed name: last FMOD_REVERB_CHANNELPROPERTIES::Room for aux A
    int mReverbRoomB; // offset 0x90, Guessed name: last FMOD_REVERB_CHANNELPROPERTIES::Room for aux B
    int mUnk94; // offset 0x94, unresolved
    unsigned int mPendingPosition; // offset 0x98, Guessed name: AX address applied at start, -1 for none
    unsigned int mStartAddress; // offset 0x9C, Guessed name: AX loop or current address at start

    ChannelGC();

    // ChannelReal overrides
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT isPlaying(bool * isplaying);

    FMOD_RESULT updateStart(); // Guessed name, 0x80621F38
    FMOD_RESULT updateStop(); // Guessed name, 0x80621FF4
    static void voiceCallback(void * voice); // Guessed name, 0x80622C8C (empty AXAcquireVoice callback)
};

} // namespace FMOD

#endif
