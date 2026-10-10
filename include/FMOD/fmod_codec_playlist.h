// G2MEAB playlist reader ("FMOD Playlist Reader Codec", descriptor 0x807533D4). The 4.06 PS3 build
// compiles this codec out; the class and method names are guessed from the parsed formats.
// CodecPlaylist adds no members (descriptor mSize 0x1F4, 0x805E1D08).

#ifndef _FMOD_CODEC_PLAYLIST_H
#define _FMOD_CODEC_PLAYLIST_H

#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct FMOD_CODEC_DESCRIPTION_EX;
}

namespace FMOD {

// Guessed name
class CodecPlaylist : public Codec
{
public:
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT readASX(); // Guessed name
    FMOD_RESULT getNextXMLTag(char * tagname, int * tagnamesize, char * tagdata, int * tagdatasize); // Guessed name
    FMOD_RESULT readM3U(); // Guessed name
    FMOD_RESULT readPLS(); // Guessed name
    FMOD_RESULT getPLSToken(char * buffer, int length, int * tokensize); // Guessed name
    FMOD_RESULT readSimple(); // Guessed name
    FMOD_RESULT readLine(char * buffer, int length, int * linelength); // Guessed name
    FMOD_RESULT skipSimpleComments(); // Guessed name
    FMOD_RESULT skipWhiteSpace(int * numspaces); // Guessed name
    bool isNewLine(char c); // Guessed name
    FMOD_RESULT closeInternal();
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
};

} // namespace FMOD

#endif
