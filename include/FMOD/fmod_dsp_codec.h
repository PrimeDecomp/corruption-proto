// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_CODEC_H
#define _FMOD_DSP_CODEC_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codec_mpeg.h"
#include "fmod_codec_raw.h"
#include "fmod_codec_wav.h"
#include "fmod_dsp_resampler.h"
#include "fmod_file_memory.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct Codec;
    struct DSPCodec;
    struct DSPCodecPool;
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

struct DSPCodec : public DSPResampler
{
    DSPCodecPool * mPool; // offset 0x164
    MemoryFile mMemoryFile; // offset 0x170
    unsigned int mPosition; // offset 0xB30
    unsigned int mNewPosition; // offset 0xB34
    int mPoolIndex; // offset 0xB38
    FMOD_CODEC_WAVEFORMAT mWaveFormat; // offset 0xB3C
    Codec * mCodec; // offset 0xC64
    FMOD_RESULT freeFromPool();
    FMOD_RESULT getPositionCallback(FMOD_DSP_STATE *, unsigned int *);
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    virtual FMOD_RESULT release(bool freethis);
    FMOD_RESULT releaseInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(short * inbuffer, short * outbuffer, unsigned int length, int inchannels, int outchannels);
    virtual FMOD_RESULT setPosition(unsigned int position);
    FMOD_RESULT setPositionInternal(unsigned int position);
    FMOD_RESULT getPositionInternal(unsigned int * position);
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

struct DSPCodecMPEG : public DSPCodec
{
    char mResampleBufferMemory[9360]; // offset 0xC68
    CodecMPEG mCodecMemory; // offset 0x30F8
    CodecMPEG_MemoryBlock mCodecMemoryBlock; // offset 0x31E8
};

struct DSPCodecADPCM : public DSPCodec
{
    char mResampleBufferMemory[656]; // offset 0xC68
    CodecWav mCodecMemory; // offset 0xEF8
};

struct DSPCodecRaw : public DSPCodec
{
    char mResampleBufferMemory[2192]; // offset 0xC68
    CodecRaw mCodecMemory; // offset 0x14F8
};

} // namespace FMOD

#endif
