// G2MEAB DSPCodec (group D): the category-1 unit, sizeof 0x328 (getDescriptionEx 0x8062827C stores mSize 0x328),
// derives from DSPResampler. It declares no virtual function of its own: PluginFactory::createDSP 0x806116F0
// constructs it inline with vtable 0x806EE9B0 (weak there, slots: dtor 0x80611AF0, DSPResampler::execute
// 0x805FDDA0, DSPResampler::addInput 0x805FE3E4). The codec is a separately allocated clone (DSPCodecPool::init
// 0x80628C88); the 4.06 DSPCodecMPEG/ADPCM/Raw embedding subclasses, mNewPosition, mPoolIndex and mWaveFormat are
// absent. The vtable 0x806F00F8 class emitted in this TU is DSPInputResampler (fmod_dsp_resampler.h).

#ifndef _FMOD_DSP_CODEC_H
#define _FMOD_DSP_CODEC_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_dsp_resampler.h"
#include "fmod_file.h"
#include "fmod_file_memory.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct Codec;
    struct DSPCodecPool;
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

struct DSPCodec : public DSPResampler
{
    DSPCodecPool * mPool; // offset 0x178, DSPCodecPool::init 0x80629158
    // Guessed name: the MemoryFile the pool codec reads through (DSPCodecPool::init 0x80629134); createDSP builds it
    // with fn_80608720 (vtable 0x806EDF88) and dtor 0x80611AF0 destroys it.
    MemoryFile mFileMemory; // offset 0x17C
    unsigned int mPosition; // offset 0x320, readInternal 0x806283B8 / setPositionInternal 0x8062846C
    Codec * mCodec; // offset 0x324

    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();

    // 4.06 inline; expanded in ChannelSoftware::stop 0x805B97F4 (group B).
    FMOD_RESULT freeFromPool()
    {
        mAllocated = false;
        mPool->mUnk8--;
        return FMOD_OK;
    }

    FMOD_RESULT createInternal();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setPositionInternal(unsigned int position);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);

    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT releaseCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT resetCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setPositionCallback(FMOD_DSP_STATE * dsp, unsigned int pos);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
