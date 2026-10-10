// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_FSB_H
#define _FMOD_CODEC_FSB_H

#include "fmod.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CODEC_WAVEFORMAT;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

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

// Synthesized from the definitions in fmod_codec_fsb.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct CodecFSB : public Codec
{
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT getWaveFormatInternal(int index, FMOD_CODEC_WAVEFORMAT * waveformat_out);
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT soundcreateInternal(int subsound, FMOD_SOUND * sound);
    FMOD_RESULT resetInternal();
    FMOD_RESULT canPointInternal();
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT soundcreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
    static FMOD_RESULT getWaveFormatCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_CODEC_WAVEFORMAT * waveformat);
    static FMOD_RESULT resetCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT canPointCallback(FMOD_CODEC_STATE * codec);
};

} // namespace FMOD

#endif
