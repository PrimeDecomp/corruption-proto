// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E85D4..0x805E9924 (12 native functions).
// Basename named by the allocation/free file literal ("fmod_codec_wav.cpp").
// PCM, IEEE float, WAVE_FORMAT_EXTENSIBLE and IMA/Xbox ADPCM sources; the RIFF chunks are parsed by
// CodecWav::parseChunk (fmod_codec_wav_riff) and ADPCM blocks decoded by fmod_codec_wav_imaadpcm.
// canPointInternal/canPointCallback do not exist in this FMOD version.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec_wav.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codec_wav_imaadpcm.h"
#include "fmod_codeci.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"
#include "fmod_types.h"

#include <string.h>

#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_IMA_ADPCM 0x0011
#define WAVE_FORMAT_MPEG 0x0050
#define WAVE_FORMAT_MPEGLAYER3 0x0055
#define WAVE_FORMAT_XBOX_ADPCM 0x0069
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE

namespace FMOD {

static const FMOD_GUID KSDATAFORMAT_SUBTYPE_PCM = { 0x00000001, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };
static const FMOD_GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT = { 0x00000003, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

FMOD_CODEC_DESCRIPTION_EX wavcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecWav::getDescriptionEx()
{
    memset(&wavcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    wavcodec.name = "FMOD Wav Codec";
    wavcodec.version = 0x00010100;
    wavcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    wavcodec.open = &CodecWav::openCallback;
    wavcodec.close = &CodecWav::closeCallback;
    wavcodec.read = &CodecWav::readCallback;
    wavcodec.setposition = &CodecWav::setPositionCallback;
    wavcodec.soundcreate = &CodecWav::soundCreateCallback;

    wavcodec.mType = FMOD_SOUND_TYPE_WAV;
    wavcodec.mSize = sizeof(CodecWav);

    return &wavcodec;
}

FMOD_RESULT CodecWav::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    int tag;
    char wave[4];
    WAVE_CHUNK chunk;

    init(FMOD_SOUND_TYPE_WAV);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp((const char *)chunk.id, "RIFF", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->read(wave, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(wave, "WAVE", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);

    mSrcDataOffset = (unsigned int)-1;
    mSyncPoint = 0;
    mNumSyncPoints = 0;

    result = parseChunk(chunk.size);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mSrcFormat)
    {
        return FMOD_ERR_FORMAT;
    }

    if (mSrcDataOffset == (unsigned int)-1)
    {




















































        FMOD_Memory_Free(mSrcFormat);
        mSrcFormat = 0;
        mSrcDataOffset = 0;
        return FMOD_ERR_FORMAT;
    }

    memset(&mDestFormat, 0, sizeof(WAVE_FORMATEXTENSIBLE));
    mDestFormat.Format.wFormatTag = WAVE_FORMAT_PCM;

    tag = mSrcFormat->Format.wFormatTag;

    if (tag == WAVE_FORMAT_MPEG || tag == WAVE_FORMAT_MPEGLAYER3)
    {
        return FMOD_ERR_FORMAT;
    }

    if (tag == WAVE_FORMAT_EXTENSIBLE)
    {
        if (!memcmp(&mSrcFormat->SubFormat, &KSDATAFORMAT_SUBTYPE_PCM, sizeof(FMOD_GUID)) || !memcmp(&mSrcFormat->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(FMOD_GUID)))
        {
            memcpy(&mDestFormat, mSrcFormat, sizeof(WAVE_FORMATEXTENSIBLE));

            mWaveFormat.lengthpcm = (unsigned int)((FMOD_UINT64)mWaveFormat.lengthbytes * 8 / (FMOD_UINT64)mDestFormat.Format.wBitsPerSample / (FMOD_UINT64)mDestFormat.Format.nChannels);
            mWaveFormat.channelmask = mDestFormat.dwChannelMask;

            if (tag == WAVE_FORMAT_IEEE_FLOAT || !memcmp(&mSrcFormat->SubFormat, &KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, sizeof(FMOD_GUID)))
            {
                mWaveFormat.format = FMOD_SOUND_FORMAT_PCMFLOAT;

                if (mDestFormat.Format.wBitsPerSample != 32)
                {
                    return FMOD_ERR_FORMAT;
                }
            }
            else
            {
                result = SoundI::getFormatFromBits(mDestFormat.Format.wBitsPerSample, &mWaveFormat.format);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        else
        {
            return FMOD_ERR_FORMAT;
        }
    }
    else if (tag == WAVE_FORMAT_PCM || tag == WAVE_FORMAT_IEEE_FLOAT)
    {
        memcpy(&mDestFormat, mSrcFormat, sizeof(WAVE_FORMATEX));

        mWaveFormat.lengthpcm = (unsigned int)((FMOD_UINT64)mWaveFormat.lengthbytes * 8 / (FMOD_UINT64)mDestFormat.Format.wBitsPerSample / (FMOD_UINT64)mDestFormat.Format.nChannels);

        if (tag == WAVE_FORMAT_IEEE_FLOAT)
        {
            mWaveFormat.format = FMOD_SOUND_FORMAT_PCMFLOAT;

            if (mDestFormat.Format.wBitsPerSample != 32)
            {
                return FMOD_ERR_FORMAT;
            }
        }
        else
        {
            result = SoundI::getFormatFromBits(mDestFormat.Format.wBitsPerSample, &mWaveFormat.format);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }
    else if (tag == WAVE_FORMAT_IMA_ADPCM || tag == WAVE_FORMAT_XBOX_ADPCM)
    {
        WAVE_FORMAT_IMAADPCM * formatadpcm = (WAVE_FORMAT_IMAADPCM *)mSrcFormat;
        int numblocks;

        numblocks = mWaveFormat.lengthbytes / formatadpcm->wfx.nBlockAlign;
        mWaveFormat.lengthpcm = numblocks * formatadpcm->wSamplesPerBlock;

        memcpy(&mDestFormat, mSrcFormat, sizeof(WAVE_FORMATEX));

        if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            mWaveFormat.format = FMOD_SOUND_FORMAT_IMAADPCM;
            mDestFormat.Format.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
            mDestFormat.Format.wBitsPerSample = 32;

            if (!(usermode & (FMOD_HARDWARE | FMOD_SOFTWARE)))
            {
                mWaveFormat.mode |= FMOD_SOFTWARE;
            }
        }
        else
        {
            mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
            mDestFormat.Format.wFormatTag = WAVE_FORMAT_PCM;
            mDestFormat.Format.wBitsPerSample = 16;
        }

        mDestFormat.Format.nBlockAlign = mDestFormat.Format.nChannels * mDestFormat.Format.wBitsPerSample / 8;
        mDestFormat.Format.nAvgBytesPerSec = mDestFormat.Format.nBlockAlign * mDestFormat.Format.nSamplesPerSec;

        mSamplesPerADPCMBlock = formatadpcm->wSamplesPerBlock;
        mReadBufferLength = mSrcFormat->Format.nBlockAlign;

        mPCMBufferLength = mSamplesPerADPCMBlock;
        if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            mPCMBufferLengthBytes = mPCMBufferLength * mDestFormat.Format.wBitsPerSample / 8 * 2;
        }
        else
        {
            mPCMBufferLengthBytes = mPCMBufferLength * mDestFormat.Format.wBitsPerSample / 8 * mDestFormat.Format.nChannels;
        }
    }
    else
    {
        return FMOD_ERR_FORMAT;
    }

    if (mReadBufferLength)
    {























































































        mReadBuffer = (unsigned char *)FMOD_Memory_Calloc(mReadBufferLength);
        if (!mReadBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    if (mPCMBufferLengthBytes)
    {
        mPCMBuffer = (unsigned char *)FMOD_Memory_Calloc(mPCMBufferLengthBytes);
        if (!mPCMBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    mWaveFormat.channels = mSrcFormat->Format.nChannels;
    mWaveFormat.frequency = mSrcFormat->Format.nSamplesPerSec;
    mWaveFormat.blockalign = mSrcFormat->Format.nBlockAlign;
    mWaveFormat.loopstart = mLoopPoints[0];
    mWaveFormat.loopend = mLoopPoints[1];

    if (mLoopPoints[1] > mLoopPoints[0])
    {
        mWaveFormat.mode = FMOD_LOOP_NORMAL;
    }

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_IMAADPCM)
    {
        if (mSystem->mDSPCodecPool_ADPCM.mNumDSPCodecs)
        {
            DSPCodec * dspcodec = (DSPCodec *)mSystem->mDSPCodecPool_ADPCM.mPool[0];

            if (mPCMBufferLengthBytes > dspcodec->mCodec->mPCMBufferLengthBytes)
            {
                mSystem->mDSPCodecPool_ADPCM.close();

                result = mSystem->mDSPCodecPool_ADPCM.init(this, mSystem->mAdvancedSettings.maxADPCMcodecs);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
        }
        else
        {
            result = mSystem->mDSPCodecPool_ADPCM.init(this, mSystem->mAdvancedSettings.maxADPCMcodecs);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    return result;
}

FMOD_RESULT CodecWav::closeInternal()
{
    if (mSrcFormat)
    {























        FMOD_Memory_Free(mSrcFormat);
        mSrcFormat = 0;
    }
    if (mReadBuffer)
    {
        FMOD_Memory_Free(mReadBuffer);
        mReadBuffer = 0;
    }
    mReadBufferLength = 0;

    if (mPCMBuffer)
    {


        FMOD_Memory_Free(mPCMBuffer);
        mPCMBuffer = 0;
    }
    mPCMBufferLengthBytes = 0;

    return FMOD_OK;
}

FMOD_RESULT CodecWav::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result;
    bool finished = false;

    if (mSrcFormat->Format.wFormatTag == WAVE_FORMAT_PCM || mSrcFormat->Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT || mSrcFormat->Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE)
    {
        unsigned int pos;

        mFile->tell(&pos);

        if (pos >= mSrcDataOffset + mWaveFormat.lengthbytes)
        {
            return FMOD_ERR_FILE_EOF;
        }

        if (pos + sizebytes > mSrcDataOffset + mWaveFormat.lengthbytes)
        {
            sizebytes = (mSrcDataOffset + mWaveFormat.lengthbytes) - pos;
            finished = true;
        }

        if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCM8)
        {
            unsigned char * ptr = (unsigned char *)buffer;
            unsigned int len;

            result = mFile->read(buffer, 1, sizebytes, bytesread);

            len = *bytesread >> 2;
            while (len)
            {
                ptr[0] ^= 128;
                ptr[1] ^= 128;
                ptr[2] ^= 128;
                ptr[3] ^= 128;
                ptr += 4;
                len--;
            }

            len = *bytesread & 3;
            while (len)
            {
                *ptr ^= 128;
                ptr++;
                len--;
            }
        }
        else if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            result = mFile->read(buffer, 2, sizebytes / 2, bytesread);
            *bytesread *= 2;
        }
        else
        {
            result = mFile->read(buffer, 1, sizebytes, bytesread);
        }

        if (finished)
        {
            result = FMOD_ERR_FILE_EOF;
        }

        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (mSrcFormat->Format.wFormatTag == WAVE_FORMAT_IMA_ADPCM || mSrcFormat->Format.wFormatTag == WAVE_FORMAT_XBOX_ADPCM)
    {
        int blockalign = mWaveFormat.blockalign;

        result = mFile->read(mReadBuffer, 1, mReadBufferLength, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mWaveFormat.format == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            if (mWaveFormat.channels == 1)
            {
                IMAAdpcm_DecodeM16(mReadBuffer, (float *)mPCMBuffer, 1, blockalign, mSamplesPerADPCMBlock, 1);
            }
            else if (mWaveFormat.channels == 2)
            {
                IMAAdpcm_DecodeS16(mReadBuffer, (float *)mPCMBuffer, 1, blockalign, mSamplesPerADPCMBlock);
            }
            else
            {
                int count;

                blockalign /= mWaveFormat.channels;

                for (count = 0; count < mWaveFormat.channels; count++)
                {
                    short tempin[4096];
                    int count2;

                    for (count2 = 0; count2 < (int)mReadBufferLength / mWaveFormat.channels; count2++)
                    {
                        tempin[count2] = ((short *)mReadBuffer)[(count2 * mWaveFormat.channels) + count];
                    }

                    IMAAdpcm_DecodeM16((unsigned char *)tempin, (float *)mPCMBuffer + count, 1, blockalign, mSamplesPerADPCMBlock, mWaveFormat.channels);
                }
            }

            *bytesread = mSamplesPerADPCMBlock * sizeof(float) * mWaveFormat.channels;
        }
        else
        {
            if (mWaveFormat.channels == 1)
            {
                IMAAdpcm_DecodeM16(mReadBuffer, (short *)mPCMBuffer, 1, blockalign, mSamplesPerADPCMBlock, 1);
            }
            else if (mWaveFormat.channels == 2)
            {
                IMAAdpcm_DecodeS16(mReadBuffer, (short *)mPCMBuffer, 1, blockalign, mSamplesPerADPCMBlock);
            }
            else
            {
                int count;

                blockalign /= mWaveFormat.channels;

                for (count = 0; count < mWaveFormat.channels; count++)
                {
                    short tempin[4096];
                    int count2;

                    for (count2 = 0; count2 < (int)mReadBufferLength / mWaveFormat.channels; count2++)
                    {
                        tempin[count2] = ((short *)mReadBuffer)[(count2 * mWaveFormat.channels) + count];
                    }

                    IMAAdpcm_DecodeM16((unsigned char *)tempin, (short *)mPCMBuffer + count, 1, blockalign, mSamplesPerADPCMBlock, mWaveFormat.channels);
                }
            }

            *bytesread = mSamplesPerADPCMBlock * sizeof(short) * mWaveFormat.channels;
        }
    }
    else
    {
        return FMOD_ERR_PLUGIN_MISSING;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecWav::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int raw, pcmbytes, pcmbytesaligned, pcmaligned, excessbytes;

    if (postype == FMOD_TIMEUNIT_RAWBYTES)
    {
        return mFile->seek(mSrcDataOffset + position, 0);
    }

    raw = (unsigned int)((FMOD_UINT64)position * mWaveFormat.lengthbytes / mWaveFormat.lengthpcm);
    raw /= mWaveFormat.blockalign;
    raw *= mWaveFormat.blockalign;

    pcmaligned = (unsigned int)((FMOD_UINT64)raw * mWaveFormat.lengthpcm / mWaveFormat.lengthbytes);

    result = SoundI::getBytesFromSamples(position, &pcmbytes, mWaveFormat.channels, mWaveFormat.format);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = SoundI::getBytesFromSamples(pcmaligned, &pcmbytesaligned, mWaveFormat.channels, mWaveFormat.format);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->seek(mSrcDataOffset + raw, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    excessbytes = pcmbytes - pcmbytesaligned;
    while (excessbytes)
    {
        static char buff[1000];
        unsigned int read = 0, toread = 1000;

        if (excessbytes < 1000)
        {
            toread = excessbytes;
        }

        result = Codec::read(buff, toread, &read);
        if (result != FMOD_OK)
        {
            break;
        }

        excessbytes -= read;
    }

    return result;
}

FMOD_RESULT CodecWav::soundCreateInternal(int subsound, FMOD_SOUND * sound)
{
    SoundI * s = (SoundI *)sound;

    if (mNumSyncPoints && mSyncPoint)
    {
        int count;

        for (count = 0; count < mNumSyncPoints; count++)
        {
            s->addSyncPoint(mSyncPoint[count].mOffset, FMOD_TIMEUNIT_PCM, mSyncPoint[count].mName, 0);
        }











































































































        FMOD_Memory_Free(mSyncPoint);
        mSyncPoint = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecWav::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    return ((CodecWav *)codec)->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecWav::closeCallback(FMOD_CODEC_STATE * codec)
{
    return ((CodecWav *)codec)->closeInternal();
}

FMOD_RESULT CodecWav::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    return ((CodecWav *)codec)->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecWav::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    return ((CodecWav *)codec)->setPositionInternal(subsound, position, postype);
}

FMOD_RESULT CodecWav::soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound)
{
    return ((CodecWav *)codec)->soundCreateInternal(subsound, sound);
}

} // namespace FMOD
