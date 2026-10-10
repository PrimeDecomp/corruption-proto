// G2MEAB GameCube output (no 4.06 counterpart). Layouts from fmod_output_gc.cpp 0x806236F0..0x8062591C.

#ifndef _FMOD_OUTPUT_GC_H
#define _FMOD_OUTPUT_GC_H

#include "fmod.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_outputi.h"
#include "fmod_sound_sample.h"

#include <dolphin/ar.h>
#include <dolphin/axfx.h>

struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_OUTPUT_STATE;
struct FMOD_REVERB_PROPERTIES;
namespace FMOD {
    class ChannelGC;
    struct FMOD_OUTPUT_DESCRIPTION_EX;
    class OutputGC;
    struct SampleGC;
}

namespace FMOD {

// Guessed name. One AX auxiliary reverb slot (0x5A8, the .bss objects 0x8075611C for aux A and
// 0x807566C4 for aux B). reverbStop 0x8062460C switches on mType (1 Std at +0x450, 2 Hi at +0x270,
// 5 HiDpl2 at +0x4) and clears mActive +0x5A4; setReverbProperties 0x80624734 fills each effect.
enum
{
    REVERBGC_STD = 1, // Guessed name
    REVERBGC_HI = 2, // Guessed name
    REVERBGC_HIDPL2 = 5 // Guessed name
};

struct ReverbGC
{
    int mType; // offset 0x0, Guessed name
    AXFX_REVERBHI_DPL2 mHiDpl2; // offset 0x4, Guessed name
    AXFX_REVERBHI mHi; // offset 0x270, Guessed name
    AXFX_REVERBSTD mStd; // offset 0x450, Guessed name
    bool mActive; // offset 0x5A4, Guessed name
};

extern ReverbGC gReverbA; // Guessed name, 0x8075611C
extern ReverbGC gReverbB; // Guessed name, 0x807566C4

// Guessed name. The FMOD_REVERB_PROPERTIES fields setReverbProperties 0x80624734 keeps per instance
// (instance 0 at OutputGC+0x164, instance 1 at +0x17C), in its store order.
struct ReverbPropsGC
{
    float mDecayTime; // offset 0x0
    float mReverbDelay; // offset 0x4
    float mModulationDepth; // offset 0x8
    int mReflections; // offset 0xC
    float mEnvDiffusion; // offset 0x10
    int mRoom; // offset 0x14
};

// G2MEAB: sizeof 0x198 (getDescriptionEx 0x80623754 mSize). Evidence: init 0x80623AE8 (ARAM base/size
// +0x124/+0x128 behind +0x120, MemPool::initCustom on +0xE0, silence block +0x130 and its ARAM address
// +0x12C, ring block +0x134, ARQ request +0x138, ChannelGC array +0x194, upload buffer +0xDC, crit
// +0x15C); start 0x80624DA4 allocates the 0x1800-byte mix and DMA buffers +0xD4/+0xD8 and sets
// +0x160 when the mixer thread is started; SampleGC::lock 0x806256A4 tests/sets +0x158.
class OutputGC : public Output
{
public:
    void * mMixBuffer; // offset 0xD4, Guessed name
    void * mDMABuffer; // offset 0xD8, Guessed name
    void * mUploadBuffer; // offset 0xDC, Guessed name
    MemPool mARAMPool; // offset 0xE0, Guessed name
    bool mARAMInitialized; // offset 0x120, Guessed name
    unsigned int mARAMBase; // offset 0x124, Guessed name
    unsigned int mARAMSize; // offset 0x128, Guessed name
    unsigned int mSilenceAddress; // offset 0x12C, Guessed name
    void * mSilenceBlock; // offset 0x130, Guessed name
    void * mRingBlock; // offset 0x134, Guessed name
    ARQRequest mARQRequest; // offset 0x138, Guessed name
    bool mUploadBusy; // offset 0x158, Guessed name
    FMOD_OS_CRITICALSECTION * mUploadCrit; // offset 0x15C, Guessed name
    bool mThreadActive; // offset 0x160, Guessed name
    ReverbPropsGC mReverbProps0; // offset 0x164, Guessed name
    ReverbPropsGC mReverbProps1; // offset 0x17C, Guessed name
    ChannelGC * mChannel; // offset 0x194, Guessed name

    static FMOD_OUTPUT_DESCRIPTION_EX * getDescriptionEx();
    static void mixThreadCallback(void * data); // Guessed name, 0x8062384C
    FMOD_RESULT mixThread(); // Guessed name, 0x8062386C
    static void frameCallback(); // Guessed name, 0x80623920
    static void dmaCallback(unsigned long request); // Guessed name, 0x80623A64
    static void voiceLeftDropCallback(void * voice); // Guessed name, 0x80623A8C
    static void voiceRightDropCallback(void * voice); // Guessed name, 0x80623A98

    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int driver, char * name, int namelen);
    FMOD_RESULT init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT getHandle(void * * handle);
    FMOD_RESULT update();
    FMOD_RESULT createSample(FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample);
    FMOD_RESULT reverbStop(int instance); // Guessed name, 0x8062460C
    FMOD_RESULT setReverbProperties(const FMOD_REVERB_PROPERTIES * prop);
    FMOD_RESULT getSoundRAM(int * currentalloced, int * maxalloced, int * total);
    FMOD_RESULT start();
    FMOD_RESULT stop();
    FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2); // Guessed name, 0x806251AC
    FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2); // Guessed name, 0x80625254

    static FMOD_RESULT getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers);
    static FMOD_RESULT getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen);
    static FMOD_RESULT initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    static FMOD_RESULT closeCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT startCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT stopCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT updateCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT createSampleCallback(FMOD_OUTPUT_STATE * output, FMOD_MODE mode, FMOD_CODEC_WAVEFORMAT * waveformat, Sample * * sample);
    static FMOD_RESULT getHandleCallback(FMOD_OUTPUT_STATE * output, void * * handle);
    static FMOD_RESULT setReverbPropertiesCallback(FMOD_OUTPUT_STATE * output, const FMOD_REVERB_PROPERTIES * prop);
    static FMOD_RESULT getSoundRAMCallback(FMOD_OUTPUT_STATE * output, int * currentalloced, int * maxalloced, int * total);
};

// Guessed name. G2MEAB: 0x394 bytes, vtable 0x806EFE60 (Sample's slots with release, lock and unlock
// replaced). Evidence: ctor 0x806255CC clears +0x37C/+0x390/+0x38C/+0x388; createSample 0x8062433C
// stores the ARAM block +0x37C, its ARAM address +0x380 and the output +0x384; lock 0x806256A4 stores
// the offset +0x38C and length +0x388; unlock 0x80625734 reads the playing channel +0x390.
struct SampleGC : public Sample
{
    void * mARAMBlock; // offset 0x37C, Guessed name
    unsigned int mARAMAddress; // offset 0x380, Guessed name
    OutputGC * mOutput; // offset 0x384, Guessed name
    unsigned int mLockLength; // offset 0x388, Guessed name
    unsigned int mLockOffset; // offset 0x38C, Guessed name
    ChannelGC * mChannel; // offset 0x390, Guessed name

    SampleGC();
    virtual FMOD_RESULT release();
    virtual FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    virtual FMOD_RESULT unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
};

} // namespace FMOD

#endif
