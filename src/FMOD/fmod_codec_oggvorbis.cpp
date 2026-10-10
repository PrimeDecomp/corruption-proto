// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E113C..0x805E1C88 (14 native functions).
// Original basename named by the allocator file string (openInternal 0x805E15EC).
// Follows the 4.06 PS3 source where G2MEAB agrees. G2MEAB differences: the RIFF wrapper probe has a
// single cleanup free and copies the CodecWav fields directly (no tempwaveformat), there is no
// multi-stream check, setPosition ignores the seek result, readInternal re-reads the comments
// through the metadata callback, and readVorbisComments is emitted after setPositionInternal.
// libvorbis callees (still unnamed in G2MEAB): ov_open_callbacks 0x80632CF4, ov_info 0x80633E34,
// ov_comment 0x80633E9C, ov_read 0x80633F38, ov_pcm_total 0x80632D58, ov_raw_tell 0x80633E0C,
// ov_pcm_seek 0x80633A20, ov_clear 0x80632B88, vorbis_comment_clear 0x8062D384,
// _vorbis_window_init 0x8063433C (4.06 name; G2MEAB places it in the vorbisfile range).

#include "fmod_codec_oggvorbis.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codec_wav.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_types.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX oggvorbiscodec;

static bool gInitialized; // 0x8079B820

size_t FMOD_OggVorbis_ReadCallback(void * ptr, size_t size, size_t nmemb, void * datasource)
{
    FMOD_RESULT result;
    unsigned int rd;
    File * fp = (File *)datasource;

    result = fp->read(ptr, size, nmemb, &rd);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return (size_t)-1;
    }

    return rd;
}

int FMOD_OggVorbis_SeekCallback(void * datasource, ogg_int64_t offset, int whence)
{
    File * fp = (File *)datasource;

    if (!fp->mSeekable)
    {
        return -1;
    }

    return fp->seek((int)offset, whence);
}

ogg_int32_t FMOD_OggVorbis_TellCallback(void * datasource)
{
    File * fp = (File *)datasource;
    unsigned int pos;

    fp->tell(&pos);

    return pos;
}

FMOD_CODEC_DESCRIPTION_EX * CodecOggVorbis::getDescriptionEx()
{
    memset(&oggvorbiscodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    oggvorbiscodec.name = "FMOD Ogg Vorbis Codec";
    oggvorbiscodec.version = 0x00010100;
    oggvorbiscodec.timeunits = FMOD_TIMEUNIT_PCM;
    oggvorbiscodec.open = &CodecOggVorbis::openCallback;
    oggvorbiscodec.close = &CodecOggVorbis::closeCallback;
    oggvorbiscodec.read = &CodecOggVorbis::readCallback;
    oggvorbiscodec.setposition = &CodecOggVorbis::setPositionCallback;

    oggvorbiscodec.mType = FMOD_SOUND_TYPE_OGGVORBIS;
    oggvorbiscodec.mSize = sizeof(CodecOggVorbis);

    return &oggvorbiscodec;
}

FMOD_RESULT CodecOggVorbis::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    vorbis_info * vi;
    char str[4];
    ov_callbacks callbacks =
    {
        FMOD_OggVorbis_ReadCallback,
        FMOD_OggVorbis_SeekCallback,
        0,
        FMOD_OggVorbis_TellCallback
    };
    bool manualsizecalc = false;

    init(FMOD_SOUND_TYPE_OGGVORBIS);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mWaveFormat.lengthbytes = 0;
    mSrcDataOffset = 0;

    {
        CodecWav tempwav;
        WAVE_CHUNK chunk;

        memset(&tempwav, 0, sizeof(CodecWav));
        tempwav.mFile = mFile;
        tempwav.mSrcDataOffset = (unsigned int)-1;

        result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (!FMOD_strncmp((const char *)chunk.id, "RIFF", 4))
        {
            char wave[4];

            result = mFile->read(wave, 1, 4, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!FMOD_strncmp(wave, "WAVE", 4))
            {
                chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);

                result = tempwav.parseChunk(chunk.size);
                if (result == FMOD_OK && tempwav.mSrcFormat && tempwav.mSrcDataOffset == (unsigned int)-1)
                {
                    int format = tempwav.mSrcFormat->Format.wFormatTag;

                    if (format == 0x6750)
                    {
                        mSrcDataOffset = tempwav.mSrcDataOffset;
                        mWaveFormat.lengthbytes = tempwav.mWaveFormat.lengthbytes;
                        mLoopPoints[0] = tempwav.mLoopPoints[0];
                        mLoopPoints[1] = tempwav.mLoopPoints[1];
                        mSyncPoint = tempwav.mSyncPoint;
                        mNumSyncPoints = tempwav.mNumSyncPoints;
                    }
                    else
                    {
                        result = FMOD_ERR_FORMAT;
                    }

                    if (tempwav.mSrcFormat)
                    {












































































































































































































































                        FMOD_Memory_Free(tempwav.mSrcFormat);
                        tempwav.mSrcFormat = 0;
                    }

                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }
            }
        }

        mFile->seek(mSrcDataOffset, 0);
    }

    result = mFile->read(str, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(str, "OggS", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    if (!gInitialized)
    {
        _vorbis_window_init();
        gInitialized = true;
    }

    if (!mWaveFormat.lengthbytes)
    {
        result = mFile->getSize(&mWaveFormat.lengthbytes);
        if (result != FMOD_OK)
        {
            return result;
        }

        manualsizecalc = true;
    }

    result = mFile->seek(mSrcDataOffset, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (ov_open_callbacks(mFile, &mVorbisFile, 0, 0, callbacks) < 0)
    {
        return FMOD_ERR_FORMAT;
    }

    vi = ov_info(&mVorbisFile, -1);

    result = readVorbisComments();
    if (result != FMOD_OK)
    {
        return result;
    }

    mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
    mWaveFormat.channels = vi->channels;
    mWaveFormat.frequency = vi->rate;
    mWaveFormat.blockalign = mWaveFormat.channels * 2;

    if (manualsizecalc && mWaveFormat.lengthbytes != (unsigned int)-1)
    {
        mWaveFormat.lengthbytes -= mSrcDataOffset;
    }

    if (mFile->mSeekable)
    {
        mWaveFormat.lengthpcm = (unsigned int)ov_pcm_total(&mVorbisFile, 0);
        if (!mWaveFormat.lengthpcm)
        {
            mWaveFormat.lengthpcm = 0;
            return FMOD_ERR_FORMAT;
        }
    }
    else
    {
        mWaveFormat.lengthpcm = 0x7FFFFFFF;
    }

    if (!mSrcDataOffset)
    {
        mSrcDataOffset = (unsigned int)ov_raw_tell(&mVorbisFile);
    }

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return result;
}

FMOD_RESULT CodecOggVorbis::closeInternal()
{
    mVorbisFile.datasource = 0;
    ov_clear(&mVorbisFile);

    return FMOD_OK;
}

FMOD_RESULT CodecOggVorbis::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    int bigendian = 1;
    vorbis_comment * vc;
    char * name;
    char * value;
    int i;

    *bytesread = ov_read(&mVorbisFile, (char *)buffer, sizebytes, bigendian, 2, 1, 0);
    if (!*bytesread)
    {
        return FMOD_ERR_FILE_EOF;
    }

    vc = ov_comment(&mVorbisFile, -1);
    if (vc && vc->comments)
    {
        for (i = 0; i < vc->comments; i++)
        {
            name = vc->user_comments[i];
            value = name;

            while (*value && *value != '=')
            {
                value++;
            }

            if (*value == '=')
            {
                *value = 0;
                value++;
            }
            else
            {
                value = name;
                name = "NONAME";
            }

            metadata(this, FMOD_TAGTYPE_VORBISCOMMENT, name, value, FMOD_strlen(value) + 1, FMOD_TAGDATATYPE_STRING, true);
        }

        vorbis_comment_clear(vc);
    }

    return FMOD_OK;
}

FMOD_RESULT CodecOggVorbis::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    ov_pcm_seek(&mVorbisFile, position);

    return FMOD_OK;
}

FMOD_RESULT CodecOggVorbis::readVorbisComments()
{
    FMOD_RESULT result;
    char * p;
    int count;
    vorbis_comment * vc;

    vc = ov_comment(&mVorbisFile, -1);
    if (!vc)
    {
        return FMOD_OK;
    }

    for (count = 0; count < vc->comments; count++)
    {
        if (vc->comment_lengths[count])
        {
            p = vc->user_comments[count];

            while (*p && *p != '=')
            {
                p++;
            }

            if (*p == '=')
            {
                *p++ = 0;

                result = metaData(FMOD_TAGTYPE_VORBISCOMMENT, vc->user_comments[count], p, FMOD_strlen(p) + 1, FMOD_TAGDATATYPE_STRING, false);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecOggVorbis::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecOggVorbis * ogg = (CodecOggVorbis *)codec;

    return ogg->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecOggVorbis::closeCallback(FMOD_CODEC_STATE * codec)
{
    CodecOggVorbis * ogg = (CodecOggVorbis *)codec;

    return ogg->closeInternal();
}

FMOD_RESULT CodecOggVorbis::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecOggVorbis * ogg = (CodecOggVorbis *)codec;

    return ogg->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecOggVorbis::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecOggVorbis * ogg = (CodecOggVorbis *)codec;

    return ogg->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
