// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805E113C..0x805E1C88 (14 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: Three leading file callbacks113C/1198/11D4 implement Ogg read/seek/tell. Read-only
// incoming refs show callback table806B0500 contains exactly these and only Ogg open1298 references
// it; they are part of this family before descriptor registration11FC. Descriptor80753374
// formatE/state4C8 binds1B64/1B90/1BBC/1BE8 to1298/18C4/18F4/1A3C. Open1298 directly names
// fmod_codec_oggvorbis.cpp; tag helper1A6C accompanies read. External Vorbis library
// calls80632CF4/80633E34/80633A20 are not absorbed. Final1C14 registers same descriptor and
// ends1C88, Playlist registration. Preserve every retained callback, emitted helper and
// initializer; complete inventory and inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_oggvorbis.h"
#include <stddef.h>
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "vorbis/os_types.h"

namespace FMOD {

bool CodecOggVorbis::gInitialized;
} // namespace FMOD

void (* FMOD_Memory_Free)(void *);
namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX oggvorbiscodec;
} // namespace FMOD

void * (* FMOD_Memory_ReAlloc)(void *, int);
void * (* _ogg_calloc)(int, int);
void * (* FMOD_Memory_Alloc)(int);

namespace FMOD {

size_t FMOD_OggVorbis_ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource)
{
}

int FMOD_OggVorbis_SeekCallback(void * datasource, ogg_int64_t offset, int whence)
{
}

ogg_int32_t FMOD_OggVorbis_TellCallback(void * datasource)
{
}

void * FMOD_OggVorbis_Malloc(int size)
{
}

void * FMOD_OggVorbis_Calloc(int count, int size)
{
}

void * FMOD_OggVorbis_ReAlloc(void * ptr, int size)
{
}

void FMOD_OggVorbis_Free(void * ptr)
{
}

FMOD_CODEC_DESCRIPTION_EX * CodecOggVorbis::getDescriptionEx()
{
}

FMOD_RESULT CodecOggVorbis::closeInternal()
{
}

FMOD_RESULT CodecOggVorbis::readVorbisComments()
{
}

FMOD_RESULT CodecOggVorbis::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecOggVorbis::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecOggVorbis::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecOggVorbis::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecOggVorbis::closeCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecOggVorbis::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecOggVorbis::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

} // namespace FMOD
