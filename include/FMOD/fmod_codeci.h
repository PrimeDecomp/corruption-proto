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

typedef FMOD_RESULT (* FMOD_CODEC_RESETCALLBACK)(FMOD_CODEC_STATE *);
typedef FMOD_RESULT (* FMOD_CODEC_CANPOINTCALLBACK)(FMOD_CODEC_STATE *);
// G2MEAB layout: builders clear 0x50 bytes and store mType at +0x40 and mSize at +0x44, which fits a
// SortedLinkedListNode base as in the later Gormiti build (4.06 used LinkedListNode, mType +0x3C).
// No canpoint callback is stored; the field at +0x4C is 4.06 reset or Gormiti mHandle (unresolved).
struct FMOD_CODEC_DESCRIPTION_EX : public FMOD_CODEC_DESCRIPTION, public SortedLinkedListNode
{
    FMOD_SOUND_TYPE mType; // offset 0x40
    int mSize; // offset 0x44
    FMOD_OS_LIBRARY * mModule; // offset 0x48
    FMOD_CODEC_RESETCALLBACK reset; // offset 0x4C
};

struct SYNCDATA
{
    unsigned int offset; // offset 0x0
    char name[256]; // offset 0x4
};

struct Codec : public Plugin, public FMOD_CODEC_STATE
{
    FMOD_CODEC_WAVEFORMAT * mWaveFormatMemory; // offset 0x38
    FMOD_SOUND_TYPE mType; // offset 0x3C
    FMOD_CODEC_DESCRIPTION_EX mDescription; // offset 0x40
    unsigned int mSrcDataOffset; // offset 0x90
    unsigned int mLoopPoints[2]; // offset 0x94
    int mSubSoundIndex; // offset 0x9C
    bool mAccurateLength; // offset 0xA0
    unsigned int mBlockAlign; // offset 0xA4
    unsigned char * mReadBuffer; // offset 0xA8
    unsigned int mReadBufferLength; // offset 0xAC
    unsigned char * mPCMBuffer; // offset 0xB0
    unsigned char * mPCMBufferMemory; // offset 0xB4
    unsigned int mPCMBufferLength; // offset 0xB8
    unsigned int mPCMBufferLengthBytes; // offset 0xBC
    unsigned int mPCMBufferOffsetBytes; // offset 0xC0
    unsigned int mPCMBufferFilledBytes; // offset 0xC4
    FMOD_MODE mMode; // offset 0xC8
    FMOD_MODE mOriginalMode; // offset 0xCC
    Metadata * mMetadata; // offset 0xD0
    File * mFile; // offset 0xD4
    FMOD_RESULT defaultFileRead(void *, void *, unsigned int, unsigned int *, void *);
    FMOD_RESULT defaultFileSeek(void *, unsigned int, void *);
    FMOD_RESULT defaultMetaData(FMOD_CODEC_STATE *, FMOD_TAGTYPE, char *, void *, unsigned int, FMOD_TAGDATATYPE, int);
    FMOD_RESULT defaultGetWaveFormat(FMOD_CODEC_STATE *, int, FMOD_CODEC_WAVEFORMAT *);
    Codec();
    FMOD_RESULT init(FMOD_SOUND_TYPE);
    virtual FMOD_RESULT reset();
    FMOD_RESULT canPointTo();
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
