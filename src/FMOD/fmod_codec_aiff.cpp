// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805C2540..0x805C3308 (12 native functions).
// Inferred basename; original source filename is unproven. The 4.06 PS3 build compiles this codec
// out, so the code follows the G2MEAB assembly alone and the class/function names are guessed.
// Also emits the weak File::getSize (0x805C2DE4) used through the File vtable.

#include "fmod_codec_aiff.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_types.h"

#include <math.h>
#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX aiffcodec; // Guessed name

static bool gAIFC; // Guessed name, 0x8079B7F0: the FORM type is "AIFC"
static bool gLittleEndian; // Guessed name, 0x8079B7F1: AIFC compression type "sowt"

// Guessed name: the Apple 80-bit IEEE extended to float conversion; infinities return 0.
static float ConvertFromIeeeExtended(unsigned char * bytes)
{
    float f;
    int expon;
    unsigned int hiMant, loMant;

    expon = ((bytes[0] & 0x7F) << 8) | bytes[1];
    hiMant = ((unsigned int)bytes[2] << 24) | ((unsigned int)bytes[3] << 16) | ((unsigned int)bytes[4] << 8) | (unsigned int)bytes[5];
    loMant = ((unsigned int)bytes[6] << 24) | ((unsigned int)bytes[7] << 16) | ((unsigned int)bytes[8] << 8) | (unsigned int)bytes[9];

    if (expon == 0 && hiMant == 0 && loMant == 0)
    {
        f = 0;
    }
    else if (expon == 0x7FFF)
    {
        f = 0;
    }
    else
    {
        float hi;

        expon -= 16383;
        expon -= 31;
        hi = (float)ldexp((float)(int)(hiMant - 2147483647 - 1) + 2147483648.0f, expon);
        expon -= 32;
        f = hi + (float)ldexp((float)(int)(loMant - 2147483647 - 1) + 2147483648.0f, expon);
    }

    if (bytes[0] & 0x80)
    {
        return -f;
    }

    return f;
}

FMOD_CODEC_DESCRIPTION_EX * CodecAIFF::getDescriptionEx()
{
    memset(&aiffcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    aiffcodec.name = "FMOD AIFF Codec";
    aiffcodec.version = 0x00010100;
    aiffcodec.timeunits = FMOD_TIMEUNIT_PCM;
    aiffcodec.open = &CodecAIFF::openCallback;
    aiffcodec.close = &CodecAIFF::closeCallback;
    aiffcodec.read = &CodecAIFF::readCallback;
    aiffcodec.setposition = &CodecAIFF::setPositionCallback;

    aiffcodec.mType = FMOD_SOUND_TYPE_AIFF;
    aiffcodec.mSize = sizeof(CodecAIFF);

    return &aiffcodec;
}

FMOD_RESULT CodecAIFF::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    AIFF_CHUNK chunk;
    char formtype[4];
    unsigned int offset, formsize;
    bool done = false;
    int bits = 0;

    init(FMOD_SOUND_TYPE_AIFF);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&chunk, 1, sizeof(AIFF_CHUNK), 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(chunk.id, "FORM", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->read(formtype, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    formsize = chunk.size;

    if (!FMOD_strncmp(formtype, "AIFC", 4))
    {
        gAIFC = true;
    }
    else if (FMOD_strncmp(formtype, "AIFF", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->getSize(&mWaveFormat.lengthbytes);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSrcDataOffset = (unsigned int)-1;
    offset = 12;

    do
    {
        result = mFile->seek(offset, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(&chunk, 1, sizeof(AIFF_CHUNK), 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (!FMOD_strncmp(chunk.id, "COMM", 4))
        {
            AIFF_COMMONCHUNK comm;
            AIFC_COMMONCHUNK commc;

            if (gAIFC)
            {
                result = mFile->read(&commc, 1, sizeof(AIFC_COMMONCHUNK), 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                if (!FMOD_strncmp(commc.compressionType, "NONE", 4))
                {
                    gLittleEndian = false;
                }
                else if (!FMOD_strncmp(commc.compressionType, "sowt", 4))
                {
                    gLittleEndian = true;
                }
                else
                {
                    return FMOD_ERR_FORMAT;
                }
            }
            else
            {
                result = mFile->read(&comm, 1, sizeof(AIFF_COMMONCHUNK), 0);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            if (gAIFC)
            {
                mWaveFormat.frequency = (int)ConvertFromIeeeExtended(commc.eSampleRate);
                bits = commc.sampleSize;

                result = SoundI::getFormatFromBits(bits, &mWaveFormat.format);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mWaveFormat.channels = commc.numChannels;
            }
            else
            {
                mWaveFormat.frequency = (int)ConvertFromIeeeExtended(comm.eSampleRate);
                bits = comm.sampleSize;

                result = SoundI::getFormatFromBits(bits, &mWaveFormat.format);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mWaveFormat.channels = comm.numChannels;
            }
        }
        else if (!FMOD_strncmp(chunk.id, "SSND", 4))
        {
            AIFF_SOUNDDATACHUNK ssnd;

            result = mFile->read(&ssnd, 1, sizeof(AIFF_SOUNDDATACHUNK), 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (mSrcDataOffset == (unsigned int)-1)
            {
                mWaveFormat.lengthbytes = chunk.size - sizeof(AIFF_SOUNDDATACHUNK);

                result = mFile->tell(&mSrcDataOffset);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            if (!mFile->mSeekable)
            {
                done = true;
            }
        }
        else if (!FMOD_strncmp(chunk.id, "INST", 4))
        {
            AIFF_INSTRUMENTCHUNK inst;

            result = mFile->read(&inst, 1, sizeof(AIFF_INSTRUMENTCHUNK), 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (!FMOD_strncmp(chunk.id, "MARK", 4))
        {
        }

        offset += sizeof(AIFF_CHUNK) + chunk.size;
        if (chunk.size & 1)
        {
            offset++;
        }

    } while (chunk.size >= 0 && offset < formsize && offset && !done);

    if (mSrcDataOffset == (unsigned int)-1)
    {
        mSrcDataOffset = 0;
        return FMOD_ERR_FILE_BAD;
    }

    result = SoundI::getSamplesFromBytes(mWaveFormat.lengthbytes, &mWaveFormat.lengthpcm, mWaveFormat.channels, mWaveFormat.format);
    if (result != FMOD_OK)
    {
        return result;
    }

    mWaveFormat.blockalign = mWaveFormat.channels * bits / 8;

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return result;
}

FMOD_RESULT CodecAIFF::closeInternal()
{
    return FMOD_OK;
}

FMOD_RESULT CodecAIFF::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result;

    result = mFile->read(buffer, 1, sizebytes, bytesread);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    if (gLittleEndian && mWaveFormat.format == FMOD_SOUND_FORMAT_PCM16)
    {
        short * wptr = (short *)buffer;
        unsigned int count;

        count = (*bytesread / 2) >> 2;
        while (count)
        {
            wptr[0] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[0]);
            wptr[1] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[1]);
            wptr[2] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[2]);
            wptr[3] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[3]);
            wptr += 4;
            count--;
        }

        count = (*bytesread / 2) & 3;
        while (count)
        {
            *wptr = (short)FMOD_SWAPENDIAN_WORD((unsigned short)*wptr);
            wptr++;
            count--;
        }
    }

    return result;
}

FMOD_RESULT CodecAIFF::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int bytes;

    if (mWaveFormat.format >= FMOD_SOUND_FORMAT_MAX || mWaveFormat.format < FMOD_SOUND_FORMAT_NONE)
    {
        result = FMOD_ERR_FORMAT;
    }
    else
    {
        result = FMOD_OK;
    }
    if (result != FMOD_OK)
    {
        return result;
    }

    SoundI::getBytesFromSamples(position, &bytes, mWaveFormat.channels, mWaveFormat.format);

    return mFile->seek(mSrcDataOffset + bytes, 0);
}

FMOD_RESULT CodecAIFF::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecAIFF *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecAIFF::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecAIFF *)codec)->closeInternal();
}

FMOD_RESULT CodecAIFF::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return ((CodecAIFF *)codec)->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecAIFF::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return ((CodecAIFF *)codec)->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
