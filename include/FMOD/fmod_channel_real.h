// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. ChannelReal is the G2MEAB layout and vtable order.

#ifndef _FMOD_CHANNEL_REAL_H
#define _FMOD_CHANNEL_REAL_H

#include "fmod.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    struct ChannelGroupI;
    struct ChannelI;
    class ChannelPool;
    class ChannelReal;
    class DSPI;
    class Output;
    struct SoundI;
    struct SystemI;
}

namespace FMOD {

enum CHANNELREAL_TYPE {
    CHANNELREAL_TYPE_HW3D = 0,
    CHANNELREAL_TYPE_HW2D = 1,
    CHANNELREAL_TYPE_SW = 2,
    CHANNELREAL_TYPE_MAX = 3
};

typedef unsigned int CHANNELREAL_FLAG;

// Guessed names: the flag values are preprocessor constants absent from the debug information.
// Bits from ChannelReal::stop fn_805B6FBC (clears 0x10/0x20/0x40/0x200/0x400, sets 0x80),
// setPaused fn_805B7068 (0x20), isPlaying fn_805B7740 (0x10/0x40/0x400) and the emulated update.
#define CHANNELREAL_FLAG_ALLOCATED 0x10 // Guessed name
#define CHANNELREAL_FLAG_PAUSED 0x20 // Guessed name
#define CHANNELREAL_FLAG_PLAYING 0x40 // Guessed name
#define CHANNELREAL_FLAG_STOPPED 0x80 // Guessed name
#define CHANNELREAL_FLAG_RESERVED 0x100 // Guessed name; set with ALLOCATED by ChannelPool::allocateChannel
#define CHANNELREAL_FLAG_UNK200 0x200 // Guessed name
#define CHANNELREAL_FLAG_ENDDELAY 0x400 // Guessed name; stop() with an end delay pending

// G2MEAB layout (0x78): the 4.06 members in 4.06 order shifted down by 4, with the vptr after them at
// +0x74 (MWCC places it where the first virtual is declared). Evidence: ctor fn_805B6E70 stores the
// vtable 0x806E270C at +0x74, zeroes mSystem/mRealChannel[8]/mSound/mOutput/mPool, mLoopCount = -1,
// mMinFrequency/mMaxFrequency, mNumRealChannels = 1; init fn_805B6ED4 stores index +0x48, system +0x0,
// output +0x34; ChannelStream fn_805BAB64 places its LinkedListNode base at +0x78.
// The 35-slot vtable has no destructor, no moveChannelGroup and no updateSpeakerLevels, and keeps two
// cone slots (24, 25) that 4.06 ChannelStream only retains as non-virtual declarations.
class ChannelReal
{
    friend class ChannelPool;
    friend struct ChannelI; // fmod_channeli 0x805BCCA8..0x805C11FC reads and writes the members directly
    friend struct ChannelStream;
    friend struct DSPCodec; // readInternal 0x806282E0 reads mode/loop/length (group D)
    friend struct DSPResampler; // execute 0x805FE270 reads mode/loop/sound (group D)
    friend struct DSPWaveTable; // execute 0x806064CC / setFrequency 0x80606A6C read mode/loop (group D)

protected:
    SystemI * mSystem; // offset 0x0
    int mNumRealChannels; // offset 0x4
    ChannelReal * mRealChannel[8]; // offset 0x8
    int mSubChannelIndex; // offset 0x28
    ChannelPool * mPool; // offset 0x2C
    ChannelI * mParent; // offset 0x30
    Output * mOutput; // offset 0x34
    SoundI * mSound; // offset 0x38
    DSPI * mDSP; // offset 0x3C
    FMOD_MODE mMode; // offset 0x40
    CHANNELREAL_FLAG mFlags; // offset 0x44
    int mIndex; // offset 0x48
    unsigned int mPosition; // offset 0x4C
    int mDirection; // offset 0x50
    int mLoopCount; // offset 0x54
    unsigned int mLoopStart; // offset 0x58
    unsigned int mLoopLength; // offset 0x5C
    unsigned int mLength; // offset 0x60
    unsigned int mStartDelay; // offset 0x64
    unsigned int mEndDelay; // offset 0x68
    float mMaxFrequency; // offset 0x6C
    float mMinFrequency; // offset 0x70
public:
    FMOD_RESULT init();
    FMOD_RESULT calcVolumeAndPitchFor3D();
    ChannelReal();

    // Vtable 0x806E270C slot order.
    virtual FMOD_RESULT set2DFreqVolumePanFor3D(); // vptr offset 0x74
    virtual bool isStream() { return false; }
    virtual FMOD_RESULT init(int index, SystemI * system, Output * output, DSPI * dspmixtarget);
    virtual FMOD_RESULT close();
    virtual FMOD_RESULT alloc();
    virtual FMOD_RESULT alloc(DSPI * dsp);
    virtual FMOD_RESULT start();
    virtual FMOD_RESULT update(int delta);
    virtual FMOD_RESULT updateStream();
    virtual FMOD_RESULT stop(bool force, bool updateflags);
    virtual FMOD_RESULT setPaused(bool paused);
    virtual FMOD_RESULT getPaused(bool * paused);
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
    virtual FMOD_RESULT set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume); // Guessed name (slot 24)
    virtual FMOD_RESULT set3DConeOrientation(FMOD_VECTOR * orientation); // Guessed name (slot 25)
    virtual FMOD_RESULT set3DOcclusion(float directOcclusion, float reverbOcclusion);
    virtual FMOD_RESULT setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop);
    virtual FMOD_RESULT isPlaying(bool * isplaying); // G2MEAB callers pass no includethreadlatency
    virtual FMOD_RESULT isVirtual(bool * isvirtual);
    virtual FMOD_RESULT getSpectrum(float * spectrumarray, int numentries, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
    virtual FMOD_RESULT getWaveData(float * wavearray, int numvalues, int channeloffset);
    virtual FMOD_RESULT getDSPHead(DSPI * * dsp);
    virtual FMOD_RESULT setMode(FMOD_MODE mode);
};

} // namespace FMOD

#endif
