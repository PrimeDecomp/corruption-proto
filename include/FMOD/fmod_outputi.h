// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_OUTPUTI_H
#define _FMOD_OUTPUTI_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_output.h"
#include "fmod_plugin.h"

struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_OUTPUT_STATE;
struct FMOD_REVERB_PROPERTIES;
namespace FMOD {
    struct ChannelGroupI;
    class ChannelPool;
    class ChannelReal;
    struct FMOD_OUTPUT_DESCRIPTION_EX;
    class Output;
    struct Sample;
    struct SystemI;
}

namespace FMOD {

const FMOD_OUTPUTTYPE FMOD_OUTPUTTYPE_EMULATED = (FMOD_OUTPUTTYPE)-1;
const FMOD_OUTPUTTYPE FMOD_OUTPUTTYPE_SOFTWARE = (FMOD_OUTPUTTYPE)-2;
typedef int (* FMOD_OUTPUT_GETSAMPLEMAXCHANNELS)(FMOD_OUTPUT_STATE *, FMOD_MODE, FMOD_SOUND_FORMAT);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETDRIVERCAPSEXCALLBACK)(FMOD_OUTPUT_STATE *, int, FMOD_CAPS *, int *, int *, FMOD_SPEAKERMODE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_INITEXCALLBACK)(FMOD_OUTPUT_STATE *, int, FMOD_INITFLAGS, int *, int, FMOD_SOUND_FORMAT *, int, int, int, int, void *);
typedef FMOD_RESULT (* FMOD_OUTPUT_STARTCALLBACK)(FMOD_OUTPUT_STATE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_STOPCALLBACK)(FMOD_OUTPUT_STATE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_UPDATEFINISHEDCALLBACK)(FMOD_OUTPUT_STATE *);
typedef FMOD_RESULT (* FMOD_OUTPUT_CREATESAMPLECALLBACK)(FMOD_OUTPUT_STATE *, FMOD_MODE, FMOD_CODEC_WAVEFORMAT *, Sample * *);
typedef FMOD_RESULT (* FMOD_OUTPUT_GETSOUNDRAMCALLBACK)(FMOD_OUTPUT_STATE *, int *, int *, int *);
typedef FMOD_RESULT (* FMOD_OUTPUT_RECORDSTARTCALLBACK)(FMOD_OUTPUT_STATE *, int, FMOD_SOUND *, int);
typedef FMOD_RESULT (* FMOD_OUTPUT_SETREVERBCALLBACK)(FMOD_OUTPUT_STATE *, const FMOD_REVERB_PROPERTIES *);
// G2MEAB layout: builders clear 0x98 bytes; mType +0x48, mSize +0x4C, getsamplemaxchannels +0x54,
// start +0x60, stop +0x64, createsample +0x6C, getsoundram +0x70, reverb_setproperties +0x90
// (GC output and OutputSoftware stores). The 0x14 node base occupies +0x34..+0x48 and MWCC appends
// this struct's own vptr at +0x94 (vtable lbl_806EE668, destructor fn_8060E9C4).
struct FMOD_OUTPUT_DESCRIPTION_EX : public FMOD_OUTPUT_DESCRIPTION, public LinkedListNode
{
    FMOD_OUTPUTTYPE mType; // offset 0x48
    int mSize; // offset 0x4C
    FMOD_OS_LIBRARY * mModule; // offset 0x50
    FMOD_OUTPUT_GETSAMPLEMAXCHANNELS getsamplemaxchannels; // offset 0x54
    FMOD_OUTPUT_GETDRIVERCAPSEXCALLBACK getdrivercapsex; // offset 0x58
    FMOD_OUTPUT_INITEXCALLBACK initex; // offset 0x5C
    FMOD_OUTPUT_STARTCALLBACK start; // offset 0x60
    FMOD_OUTPUT_STOPCALLBACK stop; // offset 0x64
    FMOD_OUTPUT_UPDATEFINISHEDCALLBACK updatefinished; // offset 0x68
    FMOD_OUTPUT_CREATESAMPLECALLBACK createsample; // offset 0x6C
    FMOD_OUTPUT_GETSOUNDRAMCALLBACK getsoundram; // offset 0x70
    FMOD_OUTPUT_GETNUMDRIVERSCALLBACK record_getnumdrivers; // offset 0x74
    FMOD_OUTPUT_GETDRIVERNAMECALLBACK record_getdrivername; // offset 0x78
    FMOD_OUTPUT_RECORDSTARTCALLBACK record_start; // offset 0x7C
    FMOD_OUTPUT_STOPCALLBACK record_stop; // offset 0x80
    FMOD_OUTPUT_GETPOSITIONCALLBACK record_getposition; // offset 0x84
    FMOD_OUTPUT_LOCKCALLBACK record_lock; // offset 0x88
    FMOD_OUTPUT_UNLOCKCALLBACK record_unlock; // offset 0x8C
    FMOD_OUTPUT_SETREVERBCALLBACK reverb_setproperties; // offset 0x90
};

class Output : public Plugin, public FMOD_OUTPUT_STATE
{
protected:
    bool mEnumerated; // offset 0x20
    bool mPolling; // offset 0x21
    SystemI * mSystem; // offset 0x24
    ChannelPool * mChannelPool; // offset 0x28
    ChannelPool * mChannelPool3D; // offset 0x2C
    ChannelGroupI * mMusicChannelGroup; // offset 0x30
public:
    FMOD_OUTPUT_DESCRIPTION_EX mDescription; // offset 0x34
    Output();
    virtual FMOD_RESULT release();
    FMOD_RESULT mix(void * buffer, unsigned int numsamples);
    FMOD_RESULT getFreeChannel(FMOD_MODE mode, ChannelReal * * realchannel, int numchannels, int * found);
    static FMOD_RESULT mixCallback(FMOD_OUTPUT_STATE * output, void * buffer, unsigned int length);
};

} // namespace FMOD

#endif
