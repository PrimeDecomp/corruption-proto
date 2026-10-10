// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E733C..0x805E8310 (12 native functions).
// Original basename named by the allocation/free file string ("fmod_codec_tag.cpp").
// CodecTag adds no members (descriptor mSize 0x1F4). Source order follows the 4.06 file (open,
// close, readTags, readID3v1, readID3v2, readID3v2FromFooter); readID3v2 allocates at line 696 and
// frees at lines 709 and 746, and the layout of this file keeps those lines.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec_tag.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_string.h"

#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX tagcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecTag::getDescriptionEx()
{
    memset(&tagcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    tagcodec.name = "FMOD Tag Reader Codec";
    tagcodec.version = 0x00010100;
    tagcodec.timeunits = FMOD_TIMEUNIT_PCM;
    tagcodec.open = &CodecTag::openCallback;
    tagcodec.close = &CodecTag::closeCallback;
    tagcodec.read = &CodecTag::readCallback;
    tagcodec.setposition = &CodecTag::setPositionCallback;

    tagcodec.mType = FMOD_SOUND_TYPE_TAG;
    tagcodec.mSize = sizeof(CodecTag);

    return &tagcodec;
}

FMOD_RESULT CodecTag::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;

    init(FMOD_SOUND_TYPE_TAG);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    if (usermode & FMOD_IGNORETAGS)
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = readTags();
    if (result == FMOD_OK)
    {
        unsigned int startoffset, filepos;

        result = mFile->tell(&filepos);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->getStartOffset(&startoffset);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->setStartOffset(startoffset + filepos);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Always fail so the real codec opens the file after the tags.
    */
    return FMOD_ERR_FORMAT;
}

FMOD_RESULT CodecTag::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecTag::readTags()
{
    FMOD_RESULT result;
    int offset = 0;
    char header[16];
    unsigned int filepos, itemsread;

    /*
        Tags at the end of the file.
    */
    for (;;)
    {
        result = mFile->seek(offset - 128, 2);
        if (result != FMOD_OK)
        {
            break;
        }

        result = mFile->read(header, 1, 3, &itemsread);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (itemsread != 3)
        {
            return FMOD_ERR_FILE_BAD;
        }

        if (!FMOD_strncmp(header, "TAG", 3))
        {
            result = readID3v1();
            if (result != FMOD_OK)
            {
                return result;
            }

            offset -= 128;

            result = mFile->tell(&filepos);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (filepos <= 128)
            {
                break;
            }
            continue;
        }

        result = mFile->seek(offset - 10, 2);
        if (result != FMOD_OK)
        {
            if (result == FMOD_ERR_FILE_COULDNOTSEEK)
            {
                break;
            }
            return result;
        }

        result = mFile->read(header, 1, 3, &itemsread);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (itemsread != 3)
        {
            return FMOD_ERR_FILE_BAD;
        }

        if (FMOD_strncmp(header, "3DI", 3))
        {
            break;
        }

        result = readID3v2FromFooter();
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->tell(&filepos);
        if (result != FMOD_OK)
        {
            return result;
        }

        offset = filepos;
    }

    /*
        Tags at the start of the file.
    */
    offset = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (;;)
    {
        result = mFile->read(header, 1, 16, &itemsread);
        if (result != FMOD_OK)
        {
            return result;
        }
        if (itemsread != 16)
        {
            return FMOD_ERR_FILE_BAD;
        }

        if (!FMOD_strncmp(header, "TAG", 3))
        {
            result = mFile->seek(-13, 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = readID3v1();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (!FMOD_strncmp(header, "ID3", 3))
        {
            result = mFile->seek(-13, 1);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = readID3v2();
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else
        {
            result = mFile->seek(offset, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            return FMOD_OK;
        }

        result = mFile->tell(&filepos);
        if (result != FMOD_OK)
        {
            return result;
        }

        offset = filepos;
    }
}

FMOD_RESULT CodecTag::readID3v1()
{
    FMOD_RESULT result;
    char value[31], tmp[8];
    unsigned int itemsread;

    memset(value, 0, 31);
    result = mFile->read(value, 1, 30, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 30)
    {
        return FMOD_ERR_FILE_BAD;
    }
    if (FMOD_strlen(value))
    {
        metaData(FMOD_TAGTYPE_ID3V1, "TITLE", value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    memset(value, 0, 31);
    result = mFile->read(value, 1, 30, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 30)
    {
        return FMOD_ERR_FILE_BAD;
    }
    if (FMOD_strlen(value))
    {
        metaData(FMOD_TAGTYPE_ID3V1, "ARTIST", value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    memset(value, 0, 31);
    result = mFile->read(value, 1, 30, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 30)
    {
        return FMOD_ERR_FILE_BAD;
    }
    if (FMOD_strlen(value))
    {
        metaData(FMOD_TAGTYPE_ID3V1, "ALBUM", value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    memset(value, 0, 31);
    result = mFile->read(value, 1, 4, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 4)
    {
        return FMOD_ERR_FILE_BAD;
    }
    if (FMOD_strlen(value))
    {
        metaData(FMOD_TAGTYPE_ID3V1, "YEAR", value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    memset(value, 0, 31);
    result = mFile->read(value, 1, 30, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 30)
    {
        return FMOD_ERR_FILE_BAD;
    }
    if (FMOD_strlen(value))
    {
        metaData(FMOD_TAGTYPE_ID3V1, "COMMENT", value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    /*
        ID3v1.1: a zero byte before the last comment byte makes that byte the track number.
    */
    if (!value[28] && value[29])
    {
        sprintf(tmp, "%d", value[29] & 0xFF);
        metaData(FMOD_TAGTYPE_ID3V1, "TRACK", tmp, FMOD_strlen(tmp) + 1, FMOD_TAGDATATYPE_STRING, false);
    }

    memset(value, 0, 31);
    result = mFile->read(value, 1, 1, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 1)
    {
        return FMOD_ERR_FILE_BAD;
    }
    sprintf(tmp, "%d", value[0] & 0xFF);
    metaData(FMOD_TAGTYPE_ID3V1, "GENRE", tmp, FMOD_strlen(tmp) + 1, FMOD_TAGDATATYPE_STRING, false);

    return FMOD_OK;
}
































































































































































































FMOD_RESULT CodecTag::readID3v2()
{
    FMOD_RESULT result;
    unsigned char size[4], flags;
    unsigned short version;
    unsigned int taglen, tagdatalen, tagoff, filepos, itemsread;
    int wavdatapos;

    result = mFile->tell(&filepos);
    if (result != FMOD_OK)
    {
        return result;
    }
    wavdatapos = filepos;

    result = mFile->read(&version, 1, 2, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 2)
    {
        return FMOD_ERR_FILE_BAD;
    }

    result = mFile->read(&flags, 1, 1, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 1)
    {
        return FMOD_ERR_FILE_BAD;
    }

    result = mFile->read(size, 1, 4, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 4)
    {
        return FMOD_ERR_FILE_BAD;
    }

    taglen = (size[0] << 21) + (size[1] << 14) + (size[2] << 7) + size[3];
    if (flags & 0x10)
    {
        taglen += 10;   /* footer present */
    }

    wavdatapos += taglen + 7;

    tagoff = 10;
    do
    {
        unsigned char tagdataid[5];
        unsigned short tagdataflags;
        bool ascii;

        tagdataid[0] = tagdataid[1] = tagdataid[2] = tagdataid[3] = tagdataid[4] = 0;

        if (version <= 2)
        {
            result = mFile->read(tagdataid, 3, 1, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != 1)
            {
                return FMOD_ERR_FILE_BAD;
            }

            result = mFile->read(size, 3, 1, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != 1)
            {
                return FMOD_ERR_FILE_BAD;
            }

            tagdatalen = (size[0] << 16) | (size[1] << 8) | size[2];
        }
        else if (version >= 3)
        {
            result = mFile->read(tagdataid, 4, 1, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != 1)
            {
                return FMOD_ERR_FILE_BAD;
            }

            result = mFile->read(size, 4, 1, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != 1)
            {
                return FMOD_ERR_FILE_BAD;
            }

            result = mFile->read(&tagdataflags, 2, 1, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != 1)
            {
                return FMOD_ERR_FILE_BAD;
            }

            tagdatalen = (size[0] << 24) + (size[1] << 16) + (size[2] << 8) + size[3];
        }

        ascii = ((tagdataid[0] >= 32 && tagdataid[0] < 128) || !tagdataid[0]) &&
                ((tagdataid[1] >= 32 && tagdataid[1] < 128) || !tagdataid[1]) &&
                ((tagdataid[2] >= 32 && tagdataid[2] < 128) || !tagdataid[2]) &&
                ((tagdataid[3] >= 32 && tagdataid[3] < 128) || !tagdataid[3]);

        if (ascii && tagdatalen && tagdatalen < 0x100000)
        {
            FMOD_TAGDATATYPE datatype = FMOD_TAGDATATYPE_BINARY;
            char * tagdata;

            tagdata = (char *)FMOD_Memory_Alloc(tagdatalen);
            if (!tagdata)
            {
                mFile->seek(wavdatapos, 0);
                return FMOD_ERR_MEMORY;
            }
            result = mFile->read(tagdata, 1, tagdatalen, &itemsread);
            if (result != FMOD_OK)
            {
                return result;
            }
            if (itemsread != tagdatalen)
            {
                FMOD_Memory_Free(tagdata);
                return result;
            }

            /*
                Text frames start with an encoding byte.
            */
            if (tagdataid[0] == 'T')
            {
                switch (tagdata[0])
                {
                    case 0:
                        datatype = FMOD_TAGDATATYPE_STRING;
                        break;
                    case 1:
                        datatype = FMOD_TAGDATATYPE_STRING_UTF16;
                        break;
                    case 2:
                        datatype = FMOD_TAGDATATYPE_STRING_UTF16BE;
                        break;
                    case 3:
                        datatype = FMOD_TAGDATATYPE_STRING_UTF8;
                        break;
                }

                memcpy(tagdata, tagdata + 1, tagdatalen - 1);
                tagdata[tagdatalen - 1] = 0;
            }

            metaData(FMOD_TAGTYPE_ID3V2, (char *)tagdataid, tagdata, tagdatalen, datatype, false);







            FMOD_Memory_Free(tagdata);
        }

        tagoff += tagdatalen + 10;
    } while (tagoff < taglen);

    result = mFile->seek(wavdatapos, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecTag::readID3v2FromFooter()
{
    FMOD_RESULT result;
    char size[4];
    unsigned short version;
    unsigned char flags;
    int taglen, pos;
    unsigned int itemsread, filepos;

    result = mFile->read(&version, 1, 2, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 2)
    {
        return FMOD_ERR_FILE_BAD;
    }

    result = mFile->read(&flags, 1, 1, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 1)
    {
        return FMOD_ERR_FILE_BAD;
    }

    result = mFile->read(size, 1, 4, &itemsread);
    if (result != FMOD_OK)
    {
        return result;
    }
    if (itemsread != 4)
    {
        return FMOD_ERR_FILE_BAD;
    }

    taglen = (size[0] << 21) + (size[1] << 14) + (size[2] << 7) + size[3];
    if (flags & 0x10)
    {
        taglen += 10;
    }

    result = mFile->seek(3 - taglen, 1);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->tell(&filepos);
    if (result != FMOD_OK)
    {
        return result;
    }

    pos = filepos - 3;

    result = readID3v2();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(pos, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecTag::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecTag *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecTag::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecTag *)codec)->closeInternal();
}

FMOD_RESULT CodecTag::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return FMOD_OK;
}

FMOD_RESULT CodecTag::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return FMOD_OK;
}

} // namespace FMOD
