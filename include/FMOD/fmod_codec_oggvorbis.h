// G2MEAB Ogg Vorbis codec ("FMOD Ogg Vorbis Codec", descriptor 0x80753374, mSize 0x4C8 at 0x805E1260).
// The 4.06 DWARF has no CodecOggVorbis type entry; members are placed from the G2MEAB accesses.

#ifndef _FMOD_CODEC_OGGVORBIS_H
#define _FMOD_CODEC_OGGVORBIS_H

#include <stddef.h>
#include "fmod.h"
#include "fmod_codeci.h"
#include "vorbis/os_types.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct SyncPoint;
}

// libvorbis declarations FMOD uses (4.06 DWARF: vorbis/codec.h, vorbis/vorbisfile.h). The G2MEAB library
// symbols are still unnamed; the call sites are listed in the source.
extern "C" {

struct vorbis_info
{
    int version; // offset 0x0
    int channels; // offset 0x4
    ogg_int32_t rate; // offset 0x8
    ogg_int32_t bitrate_upper; // offset 0xC
    ogg_int32_t bitrate_nominal; // offset 0x10
    ogg_int32_t bitrate_lower; // offset 0x14
    ogg_int32_t bitrate_window; // offset 0x18
    void * codec_setup; // offset 0x1C
};

struct vorbis_comment
{
    char ** user_comments; // offset 0x0
    int * comment_lengths; // offset 0x4
    int comments; // offset 0x8
    char * vendor; // offset 0xC
};

struct ov_callbacks
{
    size_t (* read_func)(void * ptr, size_t size, size_t nmemb, void * datasource); // offset 0x0
    int (* seek_func)(void * datasource, ogg_int64_t offset, int whence); // offset 0x4
    int (* close_func)(void * datasource); // offset 0x8
    ogg_int32_t (* tell_func)(void * datasource); // offset 0xC
};

// G2MEAB: 0x2C8 bytes (CodecOggVorbis +0x1F8..+0x4C0). Only the leading libvorbis fields are
// spelled out; closeInternal 0x805E18C4 clears datasource before ov_clear.
struct OggVorbis_File
{
    void * datasource; // offset 0x0
    int seekable; // offset 0x4
    ogg_int64_t offset; // offset 0x8
    ogg_int64_t end; // offset 0x10
    unsigned char mUnk18[0x2B0]; // offset 0x18, remaining libvorbis state
};

int ov_open_callbacks(void * datasource, OggVorbis_File * vf, char * initial, long ibytes, ov_callbacks callbacks);
int ov_clear(OggVorbis_File * vf);
vorbis_info * ov_info(OggVorbis_File * vf, int link);
vorbis_comment * ov_comment(OggVorbis_File * vf, int link);
long ov_read(OggVorbis_File * vf, char * buffer, int length, int bigendianp, int word, int sgned, int * bitstream);
ogg_int64_t ov_pcm_total(OggVorbis_File * vf, int i);
ogg_int64_t ov_raw_tell(OggVorbis_File * vf);
int ov_pcm_seek(OggVorbis_File * vf, ogg_int64_t pos);
void vorbis_comment_clear(vorbis_comment * vc);
void _vorbis_window_init();

}

namespace FMOD {

// Name from the 4.06 descriptor callbacks; layout from G2MEAB (sizeof 0x4C8).
class CodecOggVorbis : public Codec
{
public:
    OggVorbis_File mVorbisFile; // offset 0x1F8, Guessed name (8-byte aligned after the 0x1F4 Codec base)
    SyncPoint * mSyncPoint; // offset 0x4C0, Guessed name: copied from a RIFF wrapper's CodecWav (0x805E15B4)
    int mNumSyncPoints; // offset 0x4C4, Guessed name

    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    FMOD_RESULT readVorbisComments();
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
};

size_t FMOD_OggVorbis_ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource);
int FMOD_OggVorbis_SeekCallback(void * datasource, ogg_int64_t offset, int whence);
ogg_int32_t FMOD_OggVorbis_TellCallback(void * datasource);

} // namespace FMOD

#endif
