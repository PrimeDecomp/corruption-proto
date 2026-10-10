// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_CODEC_OGGVORBIS_H
#define _FMOD_CODEC_OGGVORBIS_H

#include <stddef.h>
#include "fmod.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_codec_oggvorbis.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct CodecOggVorbis : public Codec
{
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT closeInternal();
    FMOD_RESULT readVorbisComments();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static bool gInitialized;
};

} // namespace FMOD

#include "vorbis/os_types.h"

namespace FMOD {

size_t FMOD_OggVorbis_ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource);
int FMOD_OggVorbis_SeekCallback(void * datasource, ogg_int64_t offset, int whence);
ogg_int32_t FMOD_OggVorbis_TellCallback(void * datasource);
void * FMOD_OggVorbis_Malloc(int size);
void * FMOD_OggVorbis_Calloc(int count, int size);
void * FMOD_OggVorbis_ReAlloc(void * ptr, int size);
void FMOD_OggVorbis_Free(void * ptr);

} // namespace FMOD

#endif
