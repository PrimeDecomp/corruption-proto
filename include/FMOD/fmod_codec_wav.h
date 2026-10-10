// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. CodecWav uses the G2MEAB layout
// (sizeof 0x234, descriptor mSize 0x805E8650); the WAVE_* records match the native byte-swap accesses.

#ifndef _FMOD_CODEC_WAV_H
#define _FMOD_CODEC_WAV_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_types.h"
#include "fmod_syncpoint.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct SyncPoint;
    struct WAVE_FORMATEX;
    struct WAVE_FORMATEXTENSIBLE;
}

namespace FMOD {

// The RIFF records are byte-packed: Samples sits at 0x12 and sizeof(WAVE_FORMATEXTENSIBLE) is 0x28
// (parseChunk calloc minimum 0x805EAE00, CodecWav::mNumSyncPoints at 0x228).
#pragma pack(1)

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

#pragma pack()

class CodecWav : public Codec
{
public:
    unsigned char * mReadBuffer; // offset 0x1F4, calloc'd in openInternal 0x805E8BE4 // Guessed name (4.06 Codec member)
    unsigned int mReadBufferLength; // offset 0x1F8, source nBlockAlign 0x805E8B90 // Guessed name (4.06 Codec member)
    WAVE_FORMATEXTENSIBLE * mSrcFormat; // offset 0x1FC, "fmt " chunk copy (parseChunk 0x805EAE20)
    WAVE_FORMATEXTENSIBLE mDestFormat; // offset 0x200
    int mNumSyncPoints; // offset 0x228, "cue " count (parseChunk 0x805EAFFC)
    SyncPoint * mSyncPoint; // offset 0x22C, array of mNumSyncPoints (parseChunk 0x805EB05C)
    int mSamplesPerADPCMBlock; // offset 0x230 (openInternal 0x805E8B88)

    FMOD_RESULT parseChunk(unsigned int chunksize);
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT soundCreateInternal(int subsound, FMOD_SOUND * sound);
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
};

} // namespace FMOD

#endif
