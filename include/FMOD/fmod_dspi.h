// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. DSPI, FMOD_DSP_DESCRIPTION_EX and
// FMOD_DSP_CATEGORY use the G2MEAB layout (group D); DSPConnectionRequest is still the 4.06 reference.

#ifndef _FMOD_DSPI_H
#define _FMOD_DSPI_H

#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_plugin.h"

namespace FMOD {
    struct DSP;
    class DSPConnection;
    class DSPI;
    struct FMOD_DSP_DESCRIPTION_EX;
    class LinkedListNode;
    struct System;
}

namespace FMOD {

// G2MEAB values (4.06 has seven categories). Evidence: DSPI::alloc 0x80606D14 switch (0: channels <= output
// channels, 1: channels != 0, 2/3: unchecked, 4: channels == 0); DSPI::addInput 0x806073E0 rejects a category 4
// unit with outputs (4.06 does that for RESAMPLER) and a category 2 input (the soundcard).
enum FMOD_DSP_CATEGORY {
    FMOD_DSP_CATEGORY_FILTER = 0,
    FMOD_DSP_CATEGORY_DSPCODEC = 1, // Guessed name
    FMOD_DSP_CATEGORY_SOUNDCARD = 2, // Guessed name
    FMOD_DSP_CATEGORY_WAVETABLE = 3, // Guessed name
    FMOD_DSP_CATEGORY_RESAMPLER = 4 // Guessed name
};

const int FMOD_DSP_TYPE_CODECREADER = 1000;
// G2MEAB layout: builders clear 0x90 bytes. The 0x14 node base occupies +0x5C..+0x70 and MWCC appends
// this struct's own vptr at +0x8C (vtable __vt__Q24FMOD23FMOD_DSP_DESCRIPTION_EX, __sinit stores).
struct FMOD_DSP_DESCRIPTION_EX : public FMOD_DSP_DESCRIPTION, public LinkedListNode
{
    FMOD_SOUND_FORMAT mFormat; // offset 0x70
    FMOD_DSP_TYPE mType; // offset 0x74
    int mSize; // offset 0x78
    FMOD_DSP_CATEGORY mCategory; // offset 0x7C
    FMOD_OS_LIBRARY * mModule; // offset 0x80
    void * mAEffect; // offset 0x84
    int mResamplerBlockLength; // offset 0x88
};

enum FMOD_DSPCONNECTIONREQUEST_CMD {
    DSPCONNECTION_REQUEST_ADDINPUT = 0,
    DSPCONNECTION_REQUEST_DISCONNECTFROM = 1,
    DSPCONNECTION_REQUEST_DISCONNECTALLINPUTS = 2,
    DSPCONNECTION_REQUEST_DISCONNECTALLOUTPUTS = 3,
    DSPCONNECTION_REQUEST_DISCONNECTALL = 4
};

struct DSPConnectionRequest : public LinkedListNode
{
    DSPI * mThis; // offset 0xC
    DSPI * mTarget; // offset 0x10
    DSPConnection * mConnection; // offset 0x14
    FMOD_DSPCONNECTIONREQUEST_CMD mRequest; // offset 0x18
};

// G2MEAB layout, sizeof 0x118 (DSPFilter members start at 0x118, fmod_dsp_filter 0x805F6E68).
// Constructor 0x806070B8: Plugin base 0x0..0x20 (vtable 0x806EE990), FMOD_DSP_STATE at 0x20 (callbacks get
// this+0x20 after `instance = this`, e.g. 0x80607074), DSPI vtable 0x806EDE78 with 32 slots.
// Every public DSP operation is virtual here; the declaration order below is the vtable slot order.
class DSPI : public Plugin, public FMOD_DSP_STATE
{
public:
    LinkedListNode mInputHead; // offset 0x28, connections' mInputNode (getInput 0x80606E2C)
    LinkedListNode mOutputHead; // offset 0x3C, connections' mOutputNode (getOutput 0x80606EC8)
    int mNumInputs; // offset 0x50
    int mNumOutputs; // offset 0x54
    float * mBuffer; // offset 0x58, own mix buffer while the unit has several outputs (addInput 0x8060763C, disconnectFrom 0x8060794C)
    float * mOutputBuffer; // offset 0x5C, SystemI mix buffer of this tree level or mBuffer (updateTreeLevel 0x806073B8)
    bool mVisited; // offset 0x60, cleared by resetVisited 0x80606F98, set after an input ran (0x805F6B08)
    bool mActive; // offset 0x61, cleared by alloc 0x80606E10 and remove 0x80607A94; inactive inputs are skipped
    bool mBypass; // offset 0x62, DSPFilter::execute skips the read callback (0x805F6D00)
    bool mUnk63; // offset 0x63, 4.06 mFinished: an input with this set is skipped (0x805F6ACC), resampler execute 0x805FE3B0 sets it at the end of the sound
    bool mIdle; // offset 0x64, set on entry to DSPFilter::execute and cleared when it produced data (not initialized by the ctor)
    int mUnk68; // offset 0x68, unresolved
    int mTargetFrequency; // offset 0x6C, slots 0x80/0x84
    int mTreeLevel; // offset 0x70, updateTreeLevel 0x806072E4; ctor -1
    FMOD_DSP_DESCRIPTION_EX mDescription; // offset 0x74, alloc memcpy 0x90 bytes
    float mDefaultVolume; // offset 0x104, setDefaults 0x80608144
    float mDefaultFrequency; // offset 0x108
    float mDefaultPan; // offset 0x10C
    int mDefaultPriority; // offset 0x110
    bool mAllocated; // offset 0x114, Guessed name: pool checkout flag (DSPCodecPool::alloc 0x806292A4), cleared by the ctor

    static FMOD_RESULT convert(void * outbuffer, void * inbuffer, FMOD_SOUND_FORMAT outformat, FMOD_SOUND_FORMAT informat, unsigned int length, int destchannelstep, int srcchannelstep, float volume);
    static FMOD_RESULT validate(DSP * dsp, DSPI * * dspi);

    DSPI();

    FMOD_RESULT doesUnitExist(DSPI * target);
    FMOD_RESULT getInput(int index, DSPConnection * * connection);
    FMOD_RESULT getOutput(int index, DSPConnection * * connection);
    FMOD_RESULT resetVisited();
    FMOD_RESULT updateTreeLevel(int level);
    FMOD_RESULT setActive(bool active)
    {
        mActive = active;
        return FMOD_OK;
    }

    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description); // 0x10
    virtual FMOD_RESULT execute(void * inbuffer, void * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode); // 0x14
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode); // 0x18
    virtual FMOD_RESULT setPosition(unsigned int position); // 0x1C
    virtual FMOD_RESULT release(bool freethis); // 0x20
    virtual FMOD_RESULT getSystemObject(System * * system); // 0x24
    virtual FMOD_RESULT addInput(DSPI * target); // 0x28
    virtual FMOD_RESULT disconnectFrom(DSPI * target); // 0x2C
    virtual FMOD_RESULT remove(); // 0x30
    virtual FMOD_RESULT getNumInputs(int * numinputs); // 0x34
    virtual FMOD_RESULT getNumOutputs(int * numoutputs); // 0x38
    virtual FMOD_RESULT getInput(int index, DSPI * * input); // 0x3C
    virtual FMOD_RESULT getOutput(int index, DSPI * * output); // 0x40
    virtual FMOD_RESULT setInputMix(int index, float volume); // 0x44
    virtual FMOD_RESULT getInputMix(int index, float * volume); // 0x48
    virtual FMOD_RESULT reset(); // 0x4C
    virtual FMOD_RESULT setParameter(int index, float value); // 0x50
    virtual FMOD_RESULT getParameter(int index, float * value, char * valuestr, int valuestrlen); // 0x54
    virtual FMOD_RESULT getNumParameters(int * numparams); // 0x58
    virtual FMOD_RESULT getParameterInfo(int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max); // 0x5C
    virtual FMOD_RESULT showConfigDialog(void * hwnd, bool show); // 0x60
    virtual FMOD_RESULT getInfo(char * name, unsigned int * version, int * channels, int * configwidth, int * configheight); // 0x64
    virtual FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority); // 0x68
    virtual FMOD_RESULT getDefaults(float * frequency, float * volume, float * pan, int * priority); // 0x6C
    virtual FMOD_RESULT setUserData(void * userdata); // 0x70
    virtual FMOD_RESULT getUserData(void * * userdata); // 0x74
    virtual FMOD_RESULT setInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels); // 0x78
    virtual FMOD_RESULT getInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels); // 0x7C
    virtual FMOD_RESULT setTargetFrequency(int frequency); // 0x80
    virtual FMOD_RESULT getTargetFrequency(int * frequency); // 0x84
};

} // namespace FMOD

#endif
