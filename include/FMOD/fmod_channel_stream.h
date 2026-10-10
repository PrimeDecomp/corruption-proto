// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelStream is the G2MEAB layout and vtable order.

#ifndef _FMOD_CHANNEL_STREAM_H
#define _FMOD_CHANNEL_STREAM_H

#include "fmod.h"
#include "fmod_channel_real.h"
#include "fmod_linkedlist.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct ChannelGroupI;
    class ChannelReal;
    struct ChannelStream;
    class DSPI;
}

namespace FMOD {

// G2MEAB layout (0xB4): ChannelReal (vptr +0x74) then the LinkedListNode base at +0x78 (vptr +0x88,
// second vtable at 0x806E2A4C+0x94). Evidence: ctor fn_805BAB64 initializes both nodes and zeroes
// +0x98/+0x9C; alloc fn_805BAC60 clears +0x8C/+0x8D/+0x90/+0x94/+0x98/+0x9C in the 4.06 member order
// and links mStreamNode (+0xA0) into SystemI::gStreamHead; isPlaying fn_805BC718 reads mFinished +0x8C.
// The 35-slot primary vtable keeps the ChannelReal order; set3DConeSettings fn_805BC4B4 and
// set3DConeOrientation fn_805BC500 are overrides here (4.06 only declares them). No moveChannelGroup
// and no setRealChannel are retained.
struct ChannelStream : public ChannelReal, public LinkedListNode
{
    volatile bool mFinished; // offset 0x8C
    volatile bool mBusy; // offset 0x8D
    unsigned int mLastPCM; // offset 0x90
    unsigned int mDecodeOffset; // offset 0x94
    unsigned int mSamplesProcessed; // offset 0x98
    unsigned int mSamplesProcessedLast; // offset 0x9C
    LinkedListNode mStreamNode; // offset 0xA0

    ChannelStream();

    // ChannelReal overrides, vtable 0x806E2A4C slot order.
    virtual FMOD_RESULT set2DFreqVolumePanFor3D();
    virtual bool isStream() { return true; }
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT updateStream();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT setVolume(float volume);
    virtual FMOD_RESULT setFrequency(float frequency);
    virtual FMOD_RESULT setPan(float pan, float fbpan);
    virtual FMOD_RESULT setDelay(unsigned int startdelay, unsigned int enddelay);
    virtual FMOD_RESULT setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
    virtual FMOD_RESULT setSpeakerLevels(int speaker, float * levels, int numlevels);
    virtual FMOD_RESULT setPosition(unsigned int position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    virtual FMOD_RESULT setLoopPoints(unsigned int loopstart, unsigned int looplength);
    virtual FMOD_RESULT setLoopCount(int loopcount);
    virtual FMOD_RESULT set3DAttributes();
    virtual FMOD_RESULT set3DMinMaxDistance();
    virtual FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume);
    virtual FMOD_RESULT set3DConeOrientation(FMOD_VECTOR * orientation);
    virtual FMOD_RESULT set3DOcclusion(float directocclusion, float reverbocclusion);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT isPlaying(bool * isplaying);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
};

// Guessed name: the 0x168-byte stream channel pool at SystemI+0xFE0, no 4.06 counterpart. Used streams
// hang off mUsedHead's node base (+0x78), free ones off mFreeHead's (+0x12C). Evidence: add fn_80621038
// callocs a 0xB4 ChannelStream into the free list, allocate fn_806210C8 moves one to the used list,
// and the inline free fn_805BC854 (weak copy in this TU, called by ChannelStream::stop) returns one.
class ChannelStreamPool
{
public:
    ChannelStream mUsedHead; // offset 0x0, Guessed name
    ChannelStream mFreeHead; // offset 0xB4, Guessed name

    FMOD_RESULT add(); // Guessed name, fn_80621038
    FMOD_RESULT allocate(ChannelStream * * channel); // Guessed name, fn_806210C8
    FMOD_RESULT free(ChannelStream * channel); // Guessed name, fn_805BC854; inline, defined in fmod_channelgroup.cpp
    FMOD_RESULT remove(); // Guessed name, fn_80619BF8; inline, defined in fmod_soundi.cpp (frees one free-list entry; file string "fmod_freelist.h" line 94)
};

} // namespace FMOD

#endif
