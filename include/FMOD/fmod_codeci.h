// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODECI_H
#define _FMOD_CODECI_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_plugin.h"

struct FMOD_CODEC_STATE;
struct FMOD_CODEC_WAVEFORMAT;
namespace FMOD {
    struct Codec;
    struct FMOD_CODEC_DESCRIPTION_EX;
    class File;
    struct Metadata;
}

namespace FMOD {

typedef FMOD_RESULT (* FMOD_CODEC_CANPOINTCALLBACK)(FMOD_CODEC_STATE *);
// G2MEAB layout: builders clear 0x50 bytes and store mType at +0x40 and mSize at +0x44. The node base
// sits at +0x2C, so MWCC appends this struct's own vptr after mModule (+0x4C, vtable 0x806E2D24;
// destructor 0x805C24C4 with base thunk 0x805C2538). No reset/canpoint callbacks are present.
struct FMOD_CODEC_DESCRIPTION_EX : public FMOD_CODEC_DESCRIPTION, public LinkedListNode
{
    FMOD_SOUND_TYPE mType; // offset 0x40
    int mSize; // offset 0x44
    FMOD_OS_LIBRARY * mModule; // offset 0x48
};

struct SYNCDATA
{
    unsigned int offset; // offset 0x0
    char name[256]; // offset 0x4
};

// G2MEAB layout (sizeof 0x1F4): PluginFactory::createCodec 0x806115A0 allocates at least 0x1F4 and
// inlines the constructor; the Raw/AIFF/Tag/User/Playlist descriptors store mSize 0x1F4. Vtable
// 0x806E2D14 holds only the destructor 0x805C2414 and release. The waveformat is embedded (+0x40:
// CodecRaw::openInternal 0x805E3124 clears 0x128 bytes there; setPosition 0x805C1A90 copies subsound
// formats into it). Absent versus 4.06: mWaveFormatMemory, mReadBuffer, mReadBufferLength,
// mPCMBufferMemory (FSB frees mPCMBuffer directly) and mOriginalMode. See layouts.md for evidence.
struct Codec : public Plugin, public FMOD_CODEC_STATE
{
    FMOD_CODEC_WAVEFORMAT mWaveFormat; // offset 0x40, Guessed name
    FMOD_SOUND_TYPE mType; // offset 0x168
    FMOD_CODEC_DESCRIPTION_EX mDescription; // offset 0x16C
    unsigned int mSrcDataOffset; // offset 0x1BC
    unsigned int mLoopPoints[2]; // offset 0x1C0
    int mSubSoundIndex; // offset 0x1C8
    bool mAccurateLength; // offset 0x1CC
    unsigned int mBlockAlign; // offset 0x1D0
    unsigned char * mPCMBuffer; // offset 0x1D4
    unsigned int mPCMBufferLength; // offset 0x1D8
    unsigned int mPCMBufferLengthBytes; // offset 0x1DC
    unsigned int mPCMBufferOffsetBytes; // offset 0x1E0
    unsigned int mPCMBufferFilledBytes; // offset 0x1E4
    FMOD_MODE mMode; // offset 0x1E8
    Metadata * mMetadata; // offset 0x1EC
    File * mFile; // offset 0x1F0

    static FMOD_RESULT defaultFileRead(void * handle, void * buffer, unsigned int sizebytes, unsigned int * bytesread, void * userdata);
    static FMOD_RESULT defaultFileSeek(void * handle, unsigned int pos, void * userdata);
    static FMOD_RESULT defaultMetaData(FMOD_CODEC_STATE * codec, FMOD_TAGTYPE type, char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, int unique);

    Codec()
    {
        mType = FMOD_SOUND_TYPE_UNKNOWN;
        mMetadata = 0;
        fileread = defaultFileRead;
        fileseek = defaultFileSeek;
        metadata = defaultMetaData;
    }

    FMOD_RESULT init(FMOD_SOUND_TYPE type)
    {
        Plugin::init();
        mType = type;
        mMetadata = 0;
        return FMOD_OK;
    }

    virtual FMOD_RESULT release();
    FMOD_RESULT read(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT getMetadataFromFile();
    FMOD_RESULT getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype);
    FMOD_RESULT setPosition(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT getPosition(unsigned int * position, FMOD_TIMEUNIT postype);
    FMOD_RESULT metaData(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique);
};

} // namespace FMOD

#endif
