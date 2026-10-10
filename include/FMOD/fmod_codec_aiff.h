// G2MEAB AIFF/AIFC codec ("FMOD AIFF Codec", descriptor 0x8073F704). The 4.06 PS3 build compiles this
// codec out, so the class, record and member names are guessed from the chunk names. CodecAIFF adds no
// members (descriptor mSize 0x1F4, 0x805C259C). The records are big-endian and byte-packed.

#ifndef _FMOD_CODEC_AIFF_H
#define _FMOD_CODEC_AIFF_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

#pragma pack(1)

// Guessed name: 8-byte chunk header (openInternal 0x805C27AC). The size is tested signed (0x805C2C00).
struct AIFF_CHUNK
{
    char id[4]; // offset 0x0
    int size; // offset 0x4
};

// Guessed name: 0x12-byte "COMM" body of an AIFF file (openInternal 0x805C29A0).
struct AIFF_COMMONCHUNK
{
    short numChannels; // offset 0x0
    unsigned int numSampleFrames; // offset 0x2
    short sampleSize; // offset 0x6
    unsigned char eSampleRate[10]; // offset 0x8, 80-bit IEEE extended
};

// Guessed name: 0x117-byte "COMM" body of an AIFC file (openInternal 0x805C2920).
struct AIFC_COMMONCHUNK
{
    short numChannels; // offset 0x0
    unsigned int numSampleFrames; // offset 0x2
    short sampleSize; // offset 0x6
    unsigned char eSampleRate[10]; // offset 0x8
    char compressionType[4]; // offset 0x12, "NONE" or "sowt"
    char compressionName[257]; // offset 0x16
};

// Guessed name: "SSND" header (openInternal 0x805C2B24).
struct AIFF_SOUNDDATACHUNK
{
    unsigned int offset; // offset 0x0
    unsigned int blockSize; // offset 0x4
};

// Guessed name: 0x14-byte "INST" body, read and ignored (openInternal 0x805C2BB4).
struct AIFF_INSTRUMENTCHUNK
{
    unsigned char data[0x14]; // offset 0x0
};

#pragma pack()

// Guessed name
class CodecAIFF : public Codec
{
public:
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
};

} // namespace FMOD

#endif
