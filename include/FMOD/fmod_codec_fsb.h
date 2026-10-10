// G2MEAB FSB bank codec ("FMOD FSB Codec", descriptor 0x8073F7C4, mSize 0x23C at 0x805C5814).
// The sample header records follow the 4.06 DWARF (fmod_codec_fsb.h:96..142). The 4.06 DWARF has no
// CodecFSB type entry; members are placed from the G2MEAB accesses and their names are guessed.

#ifndef _FMOD_CODEC_FSB_H
#define _FMOD_CODEC_FSB_H

#include "fmod.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    class CodecWav;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct SyncPoint;
}

namespace FMOD {

// FSB2/FSB3 bank header flags and versions (the values match the FMOD 3 FSB format). Guessed names.
#define FMOD_FSB_VERSION_3_1 0x00030001
#define FMOD_FSB_SOURCE_BASICHEADERS 0x00000002

// FSB sample mode bits (FMOD 3 FSOUND_ values). Guessed names.
#define FSOUND_8BITS 0x00000008
#define FSOUND_16BITS 0x00000010
#define FSOUND_HW3D 0x00001000
#define FSOUND_2D 0x00002000
#define FSOUND_HW2D 0x00080000
#define FSOUND_3D 0x00100000
#define FSOUND_IMAADPCM 0x00400000
#define FSOUND_VAG 0x00800000
#define FSOUND_XMA 0x01000000
#define FSOUND_GCADPCM 0x02000000
#define FSOUND_SYNCPOINTS 0x80000000

// Guessed name. G2MEAB: 0x18 bytes read whole by openInternal 0x805C58B8; an "FSB2" header has no
// version/mode, so they are cleared and the file rewinds 8 bytes (0x805C5900).
struct FMOD_FSB_HEADER
{
    char id[4]; // offset 0x0, "FSB2" or "FSB3"
    int numsamples; // offset 0x4
    unsigned int shdrsize; // offset 0x8
    unsigned int datasize; // offset 0xC
    unsigned int version; // offset 0x10
    unsigned int mode; // offset 0x14
};

struct FMOD_FSB_SAMPLE_HEADER
{
    unsigned short size; // offset 0x0
    char name[30]; // offset 0x2
    unsigned int lengthsamples; // offset 0x20
    unsigned int lengthcompressedbytes; // offset 0x24
    unsigned int loopstart; // offset 0x28
    unsigned int loopend; // offset 0x2C
    unsigned int mode; // offset 0x30
    int deffreq; // offset 0x34
    unsigned short defvol; // offset 0x38
    short defpan; // offset 0x3A
    unsigned short defpri; // offset 0x3C
    unsigned short numchannels; // offset 0x3E
};

struct FMOD_FSB_SAMPLE_HEADER_3_1
{
    unsigned short size; // offset 0x0
    char name[30]; // offset 0x2
    unsigned int lengthsamples; // offset 0x20
    unsigned int lengthcompressedbytes; // offset 0x24
    unsigned int loopstart; // offset 0x28
    unsigned int loopend; // offset 0x2C
    unsigned int mode; // offset 0x30
    int deffreq; // offset 0x34
    unsigned short defvol; // offset 0x38
    short defpan; // offset 0x3A
    unsigned short defpri; // offset 0x3C
    unsigned short numchannels; // offset 0x3E
    float mindistance; // offset 0x40
    float maxdistance; // offset 0x44
    int varfreq; // offset 0x48
    unsigned short varvol; // offset 0x4C
    short varpan; // offset 0x4E
};

struct FMOD_FSB_SAMPLE_HEADER_BASIC
{
    unsigned int lengthsamples; // offset 0x0
    unsigned int lengthcompressedbytes; // offset 0x4
};

// Name from the 4.06 descriptor callbacks; layout from G2MEAB (sizeof 0x23C). The 4.06
// getWaveFormat/reset/canPoint entry points do not exist in G2MEAB.
struct CodecFSB : public Codec
{
    FMOD_FSB_HEADER mHeader; // offset 0x1F4, Guessed name
    FMOD_FSB_SAMPLE_HEADER * * mShdr; // offset 0x20C, Guessed name: full headers, calloc line 229
    FMOD_FSB_SAMPLE_HEADER_BASIC * * mShdrb; // offset 0x210, Guessed name: basic headers, calloc line 221
    FMOD_FSB_SAMPLE_HEADER * mFirstSample; // offset 0x214, Guessed name: start of the header block (closeInternal frees it)
    unsigned int * mDataOffset; // offset 0x218, Guessed name: per-sample data offsets, calloc line 261
    int mCurrentIndex; // offset 0x21C, Guessed name: subsound selected by setPositionInternal
    int * mNumSyncPoints; // offset 0x220, Guessed name: per-sample "SYNC" counts, calloc line 771
    SyncPoint * * mSyncPoint; // offset 0x224, Guessed name: per-sample SyncPoint arrays, calloc lines 777/796
    unsigned char * mReadBuffer; // offset 0x228, Guessed name: copied to CodecWav::mReadBuffer (0x805C6F04)
    unsigned int mReadBufferLength; // offset 0x22C, Guessed name: copied to CodecWav::mReadBufferLength
    CodecWav * mADPCM; // offset 0x230, Guessed name: IMA ADPCM template codec created at 0x805C62C0
    bool mDecodeADPCM; // offset 0x234, Guessed name: FMOD_CREATECOMPRESSEDSAMPLE was requested (0x805C62B8)
    int mMaxChannels; // offset 0x238, Guessed name: largest sample channel count

    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT soundcreateInternal(int subsound, FMOD_SOUND * sound);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT soundcreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
};

} // namespace FMOD

#endif
