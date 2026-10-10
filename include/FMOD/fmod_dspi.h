// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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

enum FMOD_DSP_CATEGORY {
    FMOD_DSP_CATEGORY_FILTER = 0,
    FMOD_DSP_CATEGORY_DSPCODECMPEG = 1,
    FMOD_DSP_CATEGORY_DSPCODECADPCM = 2,
    FMOD_DSP_CATEGORY_DSPCODECXMA = 3,
    FMOD_DSP_CATEGORY_DSPCODECRAW = 4,
    FMOD_DSP_CATEGORY_WAVETABLE = 5,
    FMOD_DSP_CATEGORY_RESAMPLER = 6
};

const int FMOD_DSP_TYPE_CODECREADER = 1000;
struct FMOD_DSP_DESCRIPTION_EX : public FMOD_DSP_DESCRIPTION, public LinkedListNode
{
    FMOD_SOUND_FORMAT mFormat; // offset 0x68
    FMOD_DSP_TYPE mType; // offset 0x6C
    int mSize; // offset 0x70
    FMOD_DSP_CATEGORY mCategory; // offset 0x74
    FMOD_OS_LIBRARY * mModule; // offset 0x78
    void * mAEffect; // offset 0x7C
    int mResamplerBlockLength; // offset 0x80
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

class DSPI : public Plugin, public FMOD_DSP_STATE
{
    LinkedListNode mInputHead; // offset 0x20
    LinkedListNode mOutputHead; // offset 0x2C
    int mNumInputs; // offset 0x38
    int mNumOutputs; // offset 0x3C
protected:
    float * mOutputBuffer; // offset 0x40
    bool mVisited; // offset 0x44
    bool mActive; // offset 0x45
    bool mBypass; // offset 0x46
    bool mWantsToFinish; // offset 0x47
    bool mFinished; // offset 0x48
    bool mIdle; // offset 0x49
    bool mFirstMix; // offset 0x4A
    LinkedListNode * mPrevious; // offset 0x4C
    int mTargetFrequency; // offset 0x50
    int mTreeLevel; // offset 0x54
public:
    static FMOD_OS_CRITICALSECTION * gCrit;
    static bool gActive;
    static FMOD_RESULT convert(void * outbuffer, void * inbuffer, FMOD_SOUND_FORMAT outformat, FMOD_SOUND_FORMAT informat, unsigned int length, int destchannelstep, int srcchannelstep, float volume);
    FMOD_DSP_DESCRIPTION_EX mDescription; // offset 0x58
    float mDefaultVolume; // offset 0xDC
    float mDefaultFrequency; // offset 0xE0
    float mDefaultPan; // offset 0xE4
    int mDefaultPriority; // offset 0xE8
    bool mUsedAddDSP; // offset 0xEC
    float * mBuffer; // offset 0xF0
    int mBufferChannels; // offset 0xF4
    unsigned int mMramAddress; // offset 0xF8
    DSPI * mMemory; // offset 0xFC
    LinkedListNode * mInputHeadAddress; // offset 0x100
    FMOD_RESULT setActive(bool);
    FMOD_RESULT getActive(bool *);
    FMOD_RESULT setBypass(bool);
    FMOD_RESULT getBypass(bool *);
    FMOD_RESULT setFinished(bool, bool);
    FMOD_RESULT getFinished(bool *);
    FMOD_RESULT doesUnitExist(DSPI * target);
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    FMOD_RESULT getInput(int index, DSPConnection * * input, DSPI * * inputdsp);
    FMOD_RESULT getOutput(int index, DSPConnection * * output, DSPI * * outputdsp);
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT execute(void * inbuffer, void * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    static FMOD_RESULT stepBack(LinkedListNode * & current, DSPConnection * & connection, DSPI * & t, LinkedListNode * & next);
    static FMOD_RESULT stepForwards(LinkedListNode * & current, DSPConnection * & connection, DSPConnection * & prevconnection, DSPI * & t, LinkedListNode * & next);
    FMOD_RESULT run(float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    FMOD_RESULT resetVisited();
    static FMOD_RESULT validate(DSP * dsp, DSPI * * dspi);
    virtual FMOD_RESULT setPosition(unsigned int position);
    static FMOD_RESULT calculateSpeakerLevels(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, FMOD_SPEAKERMODE speakermode, int channels, float * outlevels, int * numinputlevels);
    DSPI();
    virtual FMOD_RESULT release(bool freethis);
    virtual FMOD_RESULT getSystemObject(System * * system);
    FMOD_RESULT updateTreeLevel(int level);
    FMOD_RESULT addInputInternal(DSPI * target, bool checkcircular, DSPConnection * connection, DSPConnection * * connection_out);
    virtual FMOD_RESULT addInput(DSPI * target);
    FMOD_RESULT addInputQueued(DSPI * target, bool checkcircular, DSPConnection * * connection_out);
    FMOD_RESULT disconnectFromInternal(DSPI * target);
    FMOD_RESULT disconnectFrom(DSPI * target);
    FMOD_RESULT disconnectFromQueued(DSPI * target);
    FMOD_RESULT disconnectAll(bool inputs, bool outputs);
    FMOD_RESULT disconnectAllInternal(bool inputs, bool outputs);
    FMOD_RESULT disconnectAllQueued(bool inputs, bool outputs);
    virtual FMOD_RESULT remove();
    FMOD_RESULT getNumInputs(int * numinputs);
    FMOD_RESULT getNumOutputs(int * numoutputs);
    FMOD_RESULT getInput(int index, DSPI * * input);
    FMOD_RESULT getOutput(int index, DSPI * * output);
    FMOD_RESULT setInputMix(int index, float volume);
    FMOD_RESULT getInputMix(int index, float * volume);
    FMOD_RESULT setInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT getInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT setOutputMix(int index, float volume);
    FMOD_RESULT getOutputMix(int index, float * volume);
    FMOD_RESULT setOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    FMOD_RESULT getOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
    virtual FMOD_RESULT reset();
    virtual FMOD_RESULT setParameter(int index, float value);
    virtual FMOD_RESULT getParameter(int index, float * value, char * valuestr, int valuestrlen);
    virtual FMOD_RESULT getNumParameters(int * numparams);
    virtual FMOD_RESULT getParameterInfo(int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max);
    virtual FMOD_RESULT showConfigDialog(void * hwnd, bool show);
    virtual FMOD_RESULT getInfo(char * name, unsigned int * version, int * channels, int * configwidth, int * configheight);
    virtual FMOD_RESULT getType(FMOD_DSP_TYPE * type);
    virtual FMOD_RESULT setDefaults(float frequency, float volume, float pan, int priority);
    virtual FMOD_RESULT getDefaults(float * frequency, float * volume, float * pan, int * priority);
    FMOD_RESULT setUserData(void * userdata);
    FMOD_RESULT getUserData(void * * userdata);
    virtual FMOD_RESULT setTargetFrequency(int frequency);
    virtual FMOD_RESULT getTargetFrequency(int * frequency);
};

} // namespace FMOD

#endif
