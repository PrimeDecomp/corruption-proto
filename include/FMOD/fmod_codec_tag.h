// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_TAG_H
#define _FMOD_CODEC_TAG_H

#include "fmod.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

const FMOD_SOUND_TYPE FMOD_SOUND_TYPE_TAG = (FMOD_SOUND_TYPE)1000;
// Synthesized from the definitions in fmod_codec_tag.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct CodecTag : public Codec
{
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT closeInternal();
    FMOD_RESULT readID3v1();
    FMOD_RESULT readID3v2();
    FMOD_RESULT readID3v2FromFooter();
    FMOD_RESULT readTags();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT soundcreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
};

} // namespace FMOD

#endif
