// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E1C88..0x805E308C (18 native functions).
// Inferred basename; original source filename is unproven (no allocations, so no file string).
// The 4.06 PS3 build compiles this codec out, so the code follows the G2MEAB assembly alone and
// the class/method names are guessed. Every playlist entry is reported as a FMOD_TAGTYPE_PLAYLIST tag.

#include "fmod_codec_playlist.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_string.h"

#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX playlistcodec; // Guessed name

FMOD_CODEC_DESCRIPTION_EX * CodecPlaylist::getDescriptionEx()
{
    memset(&playlistcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    playlistcodec.name = "FMOD Playlist Reader Codec";
    playlistcodec.version = 0x00010100;
    playlistcodec.timeunits = FMOD_TIMEUNIT_PCM;
    playlistcodec.open = &CodecPlaylist::openCallback;
    playlistcodec.close = &CodecPlaylist::closeCallback;
    playlistcodec.read = &CodecPlaylist::readCallback;
    playlistcodec.setposition = &CodecPlaylist::setPositionCallback;

    playlistcodec.mType = FMOD_SOUND_TYPE_PLAYLIST;
    playlistcodec.mSize = sizeof(CodecPlaylist);

    return &playlistcodec;
}

FMOD_RESULT CodecPlaylist::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    char header[16];
    char * name;
    int length;

    init(FMOD_SOUND_TYPE_PLAYLIST);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = skipWhiteSpace(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    memset(header, 0, sizeof(header));

    result = mFile->read(header, 12, 1, 0);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    if (!FMOD_strnicmp("#EXTM3U", header, 7))
    {
        result = readM3U();
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (!FMOD_strnicmp("[PLAYLIST]", header, 10))
    {
        result = readPLS();
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (!FMOD_strnicmp("<ASX VERSION", header, 12))
    {
        result = readASX();
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else
    {
        result = mFile->getName(&name);
        if (result != FMOD_OK)
        {
            return result;
        }

        length = FMOD_strlen(name);

        if (!FMOD_strncmp(name + length - 4, ".pls", 4) ||
            !FMOD_strncmp(name + length - 4, ".m3u", 4) ||
            !FMOD_strncmp(name + length - 4, ".asx", 4) ||
            !FMOD_strncmp(name + length - 4, ".wax", 4))
        {
            result = readSimple();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            return FMOD_ERR_FORMAT;
        }
    }

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return result;
}

FMOD_RESULT CodecPlaylist::readASX()
{
    FMOD_RESULT result;
    char tagname[512];
    char tagdata[512];
    char value[512];
    int tagnamesize = sizeof(tagname);
    int tagdatasize = sizeof(tagdata);

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = getNextXMLTag(tagname, &tagnamesize, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strnicmp("ASX VERSION", tagname, 11))
    {
        return FMOD_ERR_FORMAT;
    }

    for (;;)
    {
        tagnamesize = sizeof(tagname);
        tagdatasize = sizeof(tagdata);

        result = getNextXMLTag(tagname, &tagnamesize, tagdata, &tagdatasize);
        if (result != FMOD_OK)
        {
            break;
        }

        tagname[tagnamesize] = 0;
        tagdata[tagdatasize] = 0;

        if (!FMOD_strnicmp("ENTRY", tagname, 5))
        {
            metaData(FMOD_TAGTYPE_PLAYLIST, "ENTRY", 0, 0, FMOD_TAGDATATYPE_STRING, false);
        }
        else if (!tagdatasize)
        {
            int i = 0;
            int length = 0;
            char c;

            while (tagname[i++] != '"')
            {
            }

            do
            {
                c = tagname[i++];
                if (c != '"')
                {
                    value[length++] = c;
                }
            } while (c != '"');

            value[length] = 0;

            if (!FMOD_strnicmp("REF HREF", tagname, 8))
            {
                metaData(FMOD_TAGTYPE_PLAYLIST, "FILE", value, length + 1, FMOD_TAGDATATYPE_STRING, false);
            }
            else if (!FMOD_strnicmp("MOREINFO HREF", tagname, 13))
            {
                metaData(FMOD_TAGTYPE_PLAYLIST, "MOREINFO", value, length + 1, FMOD_TAGDATATYPE_STRING, false);
            }
            else if (!FMOD_strnicmp("DURATION VALUE", tagname, 14))
            {
                metaData(FMOD_TAGTYPE_PLAYLIST, "DURATION", value, length + 1, FMOD_TAGDATATYPE_STRING, false);
            }
            else if (!FMOD_strnicmp("LOGO HREF", tagname, 9))
            {
                metaData(FMOD_TAGTYPE_PLAYLIST, "LOGO", value, length + 1, FMOD_TAGDATATYPE_STRING, false);
            }
            else if (!FMOD_strnicmp("BANNER HREF", tagname, 11))
            {
                metaData(FMOD_TAGTYPE_PLAYLIST, "BANNER", value, length + 1, FMOD_TAGDATATYPE_STRING, false);
            }
        }
        else
        {
            metaData(FMOD_TAGTYPE_PLAYLIST, FMOD_strupr(tagname), tagdata, tagdatasize + 1, FMOD_TAGDATATYPE_STRING, false);
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::getNextXMLTag(char * tagname, int * tagnamesize, char * tagdata, int * tagdatasize)
{
    FMOD_RESULT result;
    int count = 0;
    unsigned char c = 0;
    int maxdata = 0;

    result = skipWhiteSpace(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }
    } while (c != '<');

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        tagname[count++] = c;
    } while (c != '>' && count < *tagnamesize);

    *tagnamesize = count - 1;
    count = 0;

    result = skipWhiteSpace(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (tagdatasize)
    {
        maxdata = *tagdatasize;
    }

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (count < maxdata)
        {
            tagdata[count++] = c;
        }
    } while (c != '<');

    if (tagdatasize)
    {
        *tagdatasize = count - 1;
    }

    result = mFile->getByte(&c);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (c == '/')
    {
        do
        {
            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                return result;
            }
        } while (c != '>');
    }
    else
    {
        result = mFile->seek(-2, 1);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::readM3U()
{
    FMOD_RESULT result;
    char buffer[512];
    int count = 0;
    int length = 0;
    unsigned char c;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        buffer[count++] = c;
    } while (!isNewLine(c) && result == FMOD_OK);

    if (FMOD_strnicmp(buffer, "#EXTM3U", 7))
    {
        return FMOD_ERR_FORMAT;
    }

    for (;;)
    {
        count = 0;

        result = skipWhiteSpace(0);
        if (result != FMOD_OK)
        {
            break;
        }

        do
        {
            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                break;
            }

            buffer[count++] = c;
        } while (c != ':' && result == FMOD_OK);

        if (FMOD_strnicmp("#EXTINF", buffer, 7))
        {
            return FMOD_ERR_FORMAT;
        }

        result = skipWhiteSpace(0);
        if (result != FMOD_OK)
        {
            break;
        }

        count = 0;
        do
        {
            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                break;
            }

            buffer[count++] = c;
        } while (c != ',' && result == FMOD_OK);

        buffer[count - 1] = 0;
        length = atoi(buffer);
        metaData(FMOD_TAGTYPE_PLAYLIST, "LENGTH", &length, sizeof(int), FMOD_TAGDATATYPE_INT, false);

        result = skipWhiteSpace(0);
        if (result != FMOD_OK)
        {
            break;
        }

        count = 0;
        do
        {
            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                break;
            }

            if (c != '\n' && c != '\r')
            {
                buffer[count++] = c;
            }
        } while (!isNewLine(c) && result == FMOD_OK);

        buffer[count] = 0;
        metaData(FMOD_TAGTYPE_PLAYLIST, "TITLE", buffer, count + 1, FMOD_TAGDATATYPE_STRING, false);

        result = skipWhiteSpace(0);
        if (result != FMOD_OK)
        {
            break;
        }

        count = 0;
        do
        {
            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                break;
            }

            if (c != '\n' && c != '\r')
            {
                buffer[count++] = c;
            }
        } while (!isNewLine(c) && result == FMOD_OK);

        buffer[count] = 0;
        metaData(FMOD_TAGTYPE_PLAYLIST, "FILE", buffer, count, FMOD_TAGDATATYPE_STRING, false);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::readPLS()
{
    FMOD_RESULT result;
    char buffer[512];
    int length;
    int value;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (getPLSToken(buffer, sizeof(buffer), 0) != FMOD_OK)
    {
        return FMOD_ERR_FORMAT;
    }

    if (FMOD_strnicmp(buffer, "[playlist]", 10))
    {
        return FMOD_ERR_FORMAT;
    }

    for (;;)
    {
        if (getPLSToken(buffer, sizeof(buffer), 0) != FMOD_OK)
        {
            break;
        }

        if (!FMOD_strnicmp("File", buffer, 4))
        {
            if (getPLSToken(buffer, sizeof(buffer), &length) != FMOD_OK)
            {
                break;
            }

            metaData(FMOD_TAGTYPE_PLAYLIST, "FILE", buffer, length + 1, FMOD_TAGDATATYPE_STRING, false);
        }
        else if (!FMOD_strnicmp("Title", buffer, 5))
        {
            if (getPLSToken(buffer, sizeof(buffer), &length) != FMOD_OK)
            {
                break;
            }

            metaData(FMOD_TAGTYPE_PLAYLIST, "TITLE", buffer, length + 1, FMOD_TAGDATATYPE_STRING, false);
        }
        else if (!FMOD_strnicmp("Length", buffer, 6))
        {
            value = 0;

            if (getPLSToken(buffer, sizeof(buffer), &length) != FMOD_OK)
            {
                break;
            }

            buffer[length] = 0;
            value = atoi(buffer);
            metaData(FMOD_TAGTYPE_PLAYLIST, "LENGTH", &value, sizeof(int), FMOD_TAGDATATYPE_INT, false);
        }
        else if (!FMOD_strnicmp("NumberOfEntries", buffer, 15))
        {
            if (getPLSToken(buffer, sizeof(buffer), 0) != FMOD_OK)
            {
                break;
            }
        }
        else if (!FMOD_strnicmp("Version", buffer, 7))
        {
            if (getPLSToken(buffer, sizeof(buffer), 0) != FMOD_OK)
            {
                break;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::getPLSToken(char * buffer, int length, int * tokensize)
{
    FMOD_RESULT result;
    int count = 0;
    int numspaces = 0;
    unsigned char c;

    result = skipWhiteSpace(&numspaces);
    if (result != FMOD_OK)
    {
        return result;
    }

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (c != '\n' && c != '\r' && count < length)
        {
            buffer[count++] = c;
        }

        if (c == '=')
        {
            result = mFile->seek(-1 - (count + numspaces), 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->seek(count + numspaces, 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (isNewLine(c))
            {
                count++;
                break;
            }
        }

        if (c == ']')
        {
            result = mFile->seek(-count, 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->getByte(&c);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->seek(count + 1, 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (c == '[')
            {
                count++;
                break;
            }
        }
    } while (!isNewLine(c));

    if (tokensize)
    {
        *tokensize = count;
    }

    buffer[count] = 0;

    return result;
}

FMOD_RESULT CodecPlaylist::readSimple()
{
    FMOD_RESULT result;
    char buffer[512];
    int length = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (;;)
    {
        if (skipSimpleComments() != FMOD_OK)
        {
            break;
        }

        if (readLine(buffer, sizeof(buffer), &length) != FMOD_OK)
        {
            break;
        }

        metaData(FMOD_TAGTYPE_PLAYLIST, "FILE", buffer, length + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::readLine(char * buffer, int length, int * linelength)
{
    FMOD_RESULT result;
    int count = 0;
    unsigned char c;

    result = skipWhiteSpace(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (c != '\n' && c != '\r' && count < length)
        {
            buffer[count++] = c;
        }
    } while (!isNewLine(c));

    if (linelength)
    {
        *linelength = count;
    }

    buffer[count] = 0;

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::skipSimpleComments()
{
    FMOD_RESULT result;
    int numspaces = 0;
    unsigned char c;

    for (;;)
    {
        result = skipWhiteSpace(&numspaces);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (c == '#' || c == '[')
        {
            do
            {
                result = mFile->getByte(&c);
                if (result != FMOD_OK)
                {
                    return result;
                }
            } while (!isNewLine(c));
        }
        else
        {
            break;
        }
    }

    result = mFile->seek(-1, 1);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::skipWhiteSpace(int * numspaces)
{
    FMOD_RESULT result;
    int count = 0;
    unsigned char c;

    do
    {
        result = mFile->getByte(&c);
        if (result != FMOD_OK)
        {
            return result;
        }

        count++;
    } while (c == ' ' || c == '\t' || c == '\n' || c == '\r');

    result = mFile->seek(-1, 1);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numspaces)
    {
        *numspaces = count - 1;
    }

    return FMOD_OK;
}

bool CodecPlaylist::isNewLine(char c)
{
    switch (c)
    {
        case '\n':
        {
            return true;
        }
        case '\r':
        {
            unsigned char next;

            mFile->getByte(&next);
            mFile->seek(-1, 1);

            return next != '\n';
        }
    }

    return false;
}

FMOD_RESULT CodecPlaylist::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecPlaylist * playlist = (CodecPlaylist *)codec;

    return playlist->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecPlaylist::closeCallback(FMOD_CODEC_STATE * codec)
{
    CodecPlaylist * playlist = (CodecPlaylist *)codec;

    return playlist->closeInternal();
}

FMOD_RESULT CodecPlaylist::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return FMOD_OK;
}

FMOD_RESULT CodecPlaylist::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return FMOD_OK;
}

} // namespace FMOD
