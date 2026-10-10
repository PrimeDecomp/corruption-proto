// G2MEAB DSPResampler (group D): derives from DSPFilter (ctor 0x805FDA60 stores vtable 0x806EC948 then 0x806EDA40),
// sizeof 0x178. The 4.06 members keep their order shifted by +0x20 (ctor/alloc 0x805FDC68/setPosition 0x805FE458
// store order matches 4.06); the 4.06 mLength/loop/mode members are replaced by a ChannelReal pointer at 0x170
// (ChannelSoftware 0x805B9358 stores it, DSPCodec::readInternal 0x806282E0 reads mode/loop/length through it).
// Vtable 0x806EDA40: dtor 0x805FE4C8, alloc, execute(float) 0x805FDDA0, setPosition 0x805FE458, release(bool),
// addInput 0x805FE3E4.

#ifndef _FMOD_DSP_RESAMPLER_H
#define _FMOD_DSP_RESAMPLER_H

#include "fmod.h"
#include "fmod_dsp_filter.h"
#include "fmod_types.h"

namespace FMOD {
    class ChannelReal;
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

const int FMOD_DSP_RESAMPLER_OVERFLOWLENGTH = 4;

struct DSPResampler : public DSPFilter
{
    FMOD_UINT64P mPosition; // offset 0x128
    FMOD_SINT64P mSpeed; // offset 0x130
    float mFrequency; // offset 0x138, setFrequency 0x805FE400
    int mTargetFrequency; // offset 0x13C, alloc 0x805FDCA8 (output rate)
    FMOD_UINT64P mResamplePosition; // offset 0x140
    void * mResampleBufferMemory; // offset 0x148, alloc calloc line 0x86, release free line 0x49
    void * mResampleBuffer; // offset 0x14C
    int mResampleBufferChannels; // offset 0x150
    unsigned int mResampleBlockLength; // offset 0x154
    unsigned int mResampleBufferLength; // offset 0x158
    unsigned int mResampleBufferPos; // offset 0x15C
    unsigned int mResampleFinishPos; // offset 0x160
    unsigned int mOverflowLength; // offset 0x164
    unsigned int mUnk168; // offset 0x168
    int mFill; // offset 0x16C
    ChannelReal * mChannel; // offset 0x170, Guessed name
    unsigned int mUnk174; // offset 0x174

    DSPResampler();

    // DSPI overrides
    virtual FMOD_RESULT alloc(FMOD_DSP_DESCRIPTION_EX * description);
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT setPosition(unsigned int position);
    virtual FMOD_RESULT release(bool freethis);
    virtual FMOD_RESULT addInput(DSPI * target);

    FMOD_RESULT setFrequency(float frequency);
};

// Guessed name. The category-4 (resampler) unit PluginFactory::createDSP 0x806116F0 builds (sizeof 0x178, no
// members of its own): vtable 0x806F00F8 overrides execute 0x80628648, which pulls its input through
// DSPFilter::execute before resampling, and addInput 0x80628B1C; deleting dtor 0x80628B78. The code follows
// __sinit_fmod_dsp_codec_cpp 0x806285D4, so it most likely comes from a separate source file that the current
// split folds into fmod_dsp_codec.
struct DSPInputResampler : public DSPResampler
{
    // DSPI overrides
    virtual FMOD_RESULT execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode);
    virtual FMOD_RESULT addInput(DSPI * target);
};

} // namespace FMOD

// Interpolation kernels (4.06 fmod_dsp_resampler_*.cpp, unmangled there); G2MEAB emits them at the end of this TU.
// Selected by SystemI::mResampleMethod (DSPInputResampler::execute 0x8062898C): 0 NoInterp, 1 Linear, 2 Cubic, 3 Spline.
extern "C" {
void FMOD_Resampler_Cubic(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels);
void FMOD_Resampler_Linear(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels);
void FMOD_Resampler_NoInterp(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels);
void FMOD_Resampler_Spline(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels);
}

#endif
