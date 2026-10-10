// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_WAV_H
#define _FMOD_CODEC_WAV_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_types.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct SyncPoint;
    struct WAVE_FORMATEX;
    struct WAVE_FORMATEXTENSIBLE;
}

namespace FMOD {

struct WAVE_CHUNK
{
    signed char id[4]; // offset 0x0
    unsigned int size; // offset 0x4
};

struct WAVE_FORMATEX
{
    unsigned short wFormatTag; // offset 0x0
    unsigned short nChannels; // offset 0x2
    unsigned int nSamplesPerSec; // offset 0x4
    unsigned int nAvgBytesPerSec; // offset 0x8
    unsigned short nBlockAlign; // offset 0xC
    unsigned short wBitsPerSample; // offset 0xE
    unsigned short cbSize; // offset 0x10
};

struct WAVE_FORMATEXTENSIBLE
{
    WAVE_FORMATEX Format; // offset 0x0
    union {
        unsigned short wValidBitsPerSample; // offset 0x0
        unsigned short wSamplesPerBlock; // offset 0x0
        unsigned short wReserved; // offset 0x0
    } Samples; // offset 0x12
    unsigned int dwChannelMask; // offset 0x14
    FMOD_GUID SubFormat; // offset 0x18
};

struct WAVE_SMPLHEADER
{
    unsigned int Manufacturer; // offset 0x0
    unsigned int Product; // offset 0x4
    unsigned int SamplePeriod; // offset 0x8
    unsigned int Note; // offset 0xC
    unsigned int FineTune; // offset 0x10
    unsigned int SMPTEFormat; // offset 0x14
    unsigned int SMPTEOffset; // offset 0x18
    unsigned int Loops; // offset 0x1C
    unsigned int SamplerData; // offset 0x20
    struct {
        unsigned int Identifier; // offset 0x0
        unsigned int Type; // offset 0x4
        unsigned int Start; // offset 0x8
        unsigned int End; // offset 0xC
        unsigned int Fraction; // offset 0x10
        unsigned int Count; // offset 0x14
    } Loop; // offset 0x24
};

struct WAVE_CUEPOINT
{
    int dwIdentifier; // offset 0x0
    int dwPosition; // offset 0x4
    char fccChunk[4]; // offset 0x8
    int dwChunkStart; // offset 0xC
    int dwBlockStart; // offset 0x10
    int dwSampleOffset; // offset 0x14
};

class CodecWav : public Codec
{
    WAVE_FORMATEXTENSIBLE mDestFormat; // offset 0xD8
    int mNumSyncPoints; // offset 0x100
    SyncPoint * mSyncPoint; // offset 0x104
    int mSamplesPerADPCMBlock; // offset 0x108
public:
    FMOD_RESULT parseChunk(unsigned int chunksize);
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT soundCreateInternal(int subsound, FMOD_SOUND * sound);
    FMOD_RESULT canPointInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_CODEC_WAVEFORMAT mWaveFormat; // offset 0x10C
    WAVE_FORMATEXTENSIBLE mSrcFormatMemory; // offset 0x234
    WAVE_FORMATEXTENSIBLE * mSrcFormat; // offset 0x25C
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT canPointCallback(FMOD_CODEC_STATE * codec);
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
};

} // namespace FMOD

#endif
