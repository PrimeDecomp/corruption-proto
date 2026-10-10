// G2MEAB DLS instrument collection codec ("FMOD DLS Codec", descriptor 0x8073F764, mSize 0x214 at
// 0x805C3308). The 4.06 PS3 build compiles this codec out. The chunk records use the DLS level 1
// specification names; their layouts come from the reads and byte swaps in parseChunk 0x805C33A0.

#ifndef _FMOD_CODEC_DLS_H
#define _FMOD_CODEC_DLS_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name. "insh" chunk, 12 bytes read and swapped as three DWORDs (0x805C3758).
struct DLS_INSTHEADER
{
    unsigned int cRegions; // offset 0x0
    unsigned int ulBank; // offset 0x4
    unsigned int ulInstrument; // offset 0x8
};

// Guessed name. "rgnh" chunk, 12 bytes read and swapped as six WORDs (0x805C38DC).
struct DLS_RGNHEADER
{
    unsigned short usKeyLow; // offset 0x0
    unsigned short usKeyHigh; // offset 0x2
    unsigned short usVelocityLow; // offset 0x4
    unsigned short usVelocityHigh; // offset 0x6
    unsigned short fusOptions; // offset 0x8
    unsigned short usKeyGroup; // offset 0xA
};

// Guessed name. "wsmp" chunk header; sFineTune is swapped as a signed WORD (0x805C3B44).
struct DLS_WSMPL
{
    unsigned int cbSize; // offset 0x0
    unsigned short usUnityNote; // offset 0x4
    short sFineTune; // offset 0x6
    int lAttenuation; // offset 0x8
    unsigned int fulOptions; // offset 0xC
    unsigned int cSampleLoops; // offset 0x10
};

// Guessed name. The single "wsmp" loop record; swapped only when cSampleLoops is set.
struct DLS_WLOOP
{
    unsigned int cbSize; // offset 0x0
    unsigned int ulType; // offset 0x4
    unsigned int ulStart; // offset 0x8
    unsigned int ulLength; // offset 0xC
};

// Guessed name. "wlnk" chunk, 12 bytes swapped as two WORDs and two DWORDs (0x805C41A0).
struct DLS_WAVELINK
{
    unsigned short fusOptions; // offset 0x0
    unsigned short usPhaseGroup; // offset 0x2
    unsigned int ulChannel; // offset 0x4
    unsigned int ulTableIndex; // offset 0x8
};

// Guessed name. "art1" chunk header, 8 bytes; cbSize past 8 is skipped (0x805C4310).
struct DLS_CONNECTIONLIST
{
    unsigned int cbSize; // offset 0x0
    unsigned int cConnections; // offset 0x4
};

// Guessed name. One "art1" connection block, 12 bytes (calloc cConnections * 0xC, 0x805C43B0).
struct DLS_CONNECTION
{
    unsigned short usSource; // offset 0x0
    unsigned short usControl; // offset 0x2
    unsigned short usDestination; // offset 0x4
    unsigned short usTransform; // offset 0x6
    int lScale; // offset 0x8
};

// Guessed name. One "rgn " list (calloc cRegions * 0x44, 0x805C3878).
struct DLS_REGION
{
    DLS_RGNHEADER mHeader; // offset 0x0, Guessed name
    DLS_WSMPL mWaveSample; // offset 0xC, Guessed name
    DLS_WLOOP mWaveLoop; // offset 0x20, Guessed name
    DLS_WAVELINK mWaveLink; // offset 0x30, Guessed name
    unsigned int mNumConnections; // offset 0x3C, Guessed name
    DLS_CONNECTION * mConnection; // offset 0x40, Guessed name: freed by closeInternal (0x805C5298)
};

// Guessed name. One "ins " list (calloc mNumInstruments * 0x118, 0x805C3504).
struct DLS_INSTRUMENT
{
    char mName[256]; // offset 0x0, Guessed name: "INAM" text
    DLS_INSTHEADER mHeader; // offset 0x100, Guessed name
    DLS_REGION * mRegion; // offset 0x10C, Guessed name
    unsigned int mNumConnections; // offset 0x110, Guessed name: instrument-level "art1"
    DLS_CONNECTION * mConnection; // offset 0x114, Guessed name
};

// Guessed name. One "wave" list (calloc mNumSamples * 0x128, 0x805C35DC).
struct DLS_SAMPLE
{
    char mName[256]; // offset 0x0, Guessed name: "INAM" text
    unsigned int mDataOffset; // offset 0x100, Guessed name: file position of the "data" chunk
    DLS_WSMPL mWaveSample; // offset 0x104, Guessed name
    DLS_WLOOP mWaveLoop; // offset 0x118, Guessed name
};

// Guessed name
class CodecDLS : public Codec
{
public:
    int mNumInstruments; // offset 0x1F4, Guessed name: "colh" count
    int mCurrentInstrument; // offset 0x1F8, Guessed name
    DLS_INSTRUMENT * mInstrument; // offset 0x1FC, Guessed name
    int mNumSamples; // offset 0x200, Guessed name: "ptbl" cCues
    int mCurrentSample; // offset 0x204, Guessed name
    DLS_SAMPLE * mSample; // offset 0x208, Guessed name
    int mCurrentIndex; // offset 0x20C, Guessed name: subsound selected by setPositionInternal
    int mCurrentRegion; // offset 0x210, Guessed name

    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT parseChunk(char * parentchunk, unsigned int chunksize);
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
