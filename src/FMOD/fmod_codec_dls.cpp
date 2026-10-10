// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805C3308..0x805C5794 (11 native functions plus __sinit).
// Original basename named by the allocator file string (parseChunk 0x805C33A0, closeInternal 0x805C5218).
// The 4.06 PS3 build compiles this codec out, so the method and member names are guessed. parseChunk
// walks the RIFF "DLS " tree recursively: "colh" and "ptbl" size the instrument and wave tables, "ins ",
// "rgn " and "wave" lists advance the current entries, and every "wave" becomes one subsound.
// The "colh" and "ptbl" counts are used without a byte swap, as in G2MEAB.
// Also emits the weak out-of-line SoundI::getSamplesFromBytes (0x805C4EC4).

#include "fmod_codec_dls.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codec_wav.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_types.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX dlscodec;

FMOD_CODEC_DESCRIPTION_EX * CodecDLS::getDescriptionEx()
{
    memset(&dlscodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    dlscodec.name = "FMOD DLS Codec";
    dlscodec.version = 0x00010100;
    dlscodec.timeunits = FMOD_TIMEUNIT_PCM;
    dlscodec.open = &CodecDLS::openCallback;
    dlscodec.close = &CodecDLS::closeCallback;
    dlscodec.read = &CodecDLS::readCallback;
    dlscodec.setposition = &CodecDLS::setPositionCallback;

    dlscodec.mType = FMOD_SOUND_TYPE_DLS;
    dlscodec.mSize = sizeof(CodecDLS);

    return &dlscodec;
}

FMOD_RESULT CodecDLS::parseChunk(char * parentchunk, unsigned int chunksize)
{
    FMOD_RESULT result;
    unsigned int size, offset;
    WAVE_CHUNK chunk;

    result = mFile->tell(&offset);
    if (result != FMOD_OK)
    {
        return result;
    }

    size = 4;
    offset -= sizeof(WAVE_CHUNK);

    do
    {
        result = mFile->seek(offset + sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);

        if (!FMOD_strncmp((const char *)chunk.id, "vers", 4) ||
            !FMOD_strncmp((const char *)chunk.id, "msyn", 4) ||
            !FMOD_strncmp((const char *)chunk.id, "dlid", 4))
        {
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "colh", 4))
        {
            result = mFile->read(&mNumInstruments, 4, 1, 0);
            if (result != FMOD_OK)
            {
                return result;
            }































































            mInstrument = (DLS_INSTRUMENT *)FMOD_Memory_Calloc(mNumInstruments * sizeof(DLS_INSTRUMENT));
            if (!mInstrument)
            {
                return FMOD_ERR_MEMORY;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "ptbl", 4))
        {
            unsigned int cbsize;

            result = mFile->read(&cbsize, 4, 1, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = mFile->read(&mNumSamples, 4, 1, 0);
            if (result != FMOD_OK)
            {
                return result;
            }






            waveformat = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(mNumSamples * sizeof(FMOD_CODEC_WAVEFORMAT));
            if (!waveformat)
            {
                return FMOD_ERR_MEMORY;
            }

            mSample = (DLS_SAMPLE *)FMOD_Memory_Calloc(mNumSamples * sizeof(DLS_SAMPLE));
            if (!mSample)
            {
                return FMOD_ERR_MEMORY;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "LIST", 4))
        {
            char listid[4];

            result = mFile->read(listid, 1, 4, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = parseChunk(listid, chunk.size);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!FMOD_strncmp(listid, "wave", 4))
            {
                mCurrentSample++;
            }
            else if (!FMOD_strncmp(listid, "ins ", 4))
            {
                mCurrentInstrument++;
            }
            else if (!FMOD_strncmp(listid, "rgn ", 4))
            {
                mCurrentRegion++;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "dlid", 4))
        {
            FMOD_GUID guid;

            result = mFile->read(&guid, 1, sizeof(FMOD_GUID), 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "insh", 4))
        {
            result = mFile->read(&mInstrument[mCurrentInstrument].mHeader, 1, sizeof(DLS_INSTHEADER), 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            mInstrument[mCurrentInstrument].mHeader.cRegions = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mHeader.cRegions);
            mInstrument[mCurrentInstrument].mHeader.ulBank = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mHeader.ulBank);
            mInstrument[mCurrentInstrument].mHeader.ulInstrument = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mHeader.ulInstrument);










            mInstrument[mCurrentInstrument].mRegion = (DLS_REGION *)FMOD_Memory_Calloc(mInstrument[mCurrentInstrument].mHeader.cRegions * sizeof(DLS_REGION));
            if (!mInstrument[mCurrentInstrument].mRegion)
            {
                return FMOD_ERR_MEMORY;
            }

            mCurrentRegion = 0;
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "rgnh", 4))
        {
            result = mFile->read(&mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader, 1, sizeof(DLS_RGNHEADER), 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyLow = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyLow);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyHigh = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyHigh);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usVelocityLow = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usVelocityLow);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usVelocityHigh = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usVelocityHigh);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.fusOptions = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.fusOptions);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyGroup = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mHeader.usKeyGroup);
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "wsmp", 4))
        {
            unsigned int toread = sizeof(DLS_WSMPL) + sizeof(DLS_WLOOP);

            if (chunk.size < toread)
            {
                toread = chunk.size;
            }

            if (!FMOD_strncmp(parentchunk, "wave", 4))
            {
                result = mFile->read(&mSample[mCurrentSample].mWaveSample, 1, toread, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mSample[mCurrentSample].mWaveSample.cbSize = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveSample.cbSize);
                mSample[mCurrentSample].mWaveSample.usUnityNote = FMOD_SWAPENDIAN_WORD((unsigned short)mSample[mCurrentSample].mWaveSample.usUnityNote);
                mSample[mCurrentSample].mWaveSample.sFineTune = (short)FMOD_SWAPENDIAN_WORD((unsigned short)mSample[mCurrentSample].mWaveSample.sFineTune);
                mSample[mCurrentSample].mWaveSample.lAttenuation = FMOD_SWAPENDIAN_DWORD((unsigned int)mSample[mCurrentSample].mWaveSample.lAttenuation);
                mSample[mCurrentSample].mWaveSample.fulOptions = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveSample.fulOptions);

                if (mSample[mCurrentSample].mWaveSample.cSampleLoops)
                {
                    mSample[mCurrentSample].mWaveLoop.cbSize = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveLoop.cbSize);
                    mSample[mCurrentSample].mWaveLoop.ulType = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveLoop.ulType);
                    mSample[mCurrentSample].mWaveLoop.ulStart = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveLoop.ulStart);
                    mSample[mCurrentSample].mWaveLoop.ulLength = FMOD_SWAPENDIAN_DWORD(mSample[mCurrentSample].mWaveLoop.ulLength);

                    waveformat[mCurrentSample].mode = FMOD_LOOP_NORMAL;
                    waveformat[mCurrentSample].loopstart = mSample[mCurrentSample].mWaveLoop.ulStart;
                    waveformat[mCurrentSample].loopend = mSample[mCurrentSample].mWaveLoop.ulStart + mSample[mCurrentSample].mWaveLoop.ulLength - 1;
                }
            }
            else if (!FMOD_strncmp(parentchunk, "rgn ", 4))
            {
                result = mFile->read(&mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample, 1, toread, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.cbSize = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.cbSize);
                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.usUnityNote = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.usUnityNote);
                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.sFineTune = (short)FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.sFineTune);
                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.lAttenuation = FMOD_SWAPENDIAN_DWORD((unsigned int)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.lAttenuation);
                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.fulOptions = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.fulOptions);

                if (mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveSample.cSampleLoops)
                {
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.cbSize = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.cbSize);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulType = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulType);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulStart = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulStart);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulLength = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLoop.ulLength);
                }
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "wlnk", 4))
        {
            result = mFile->read(&mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink, 1, sizeof(DLS_WAVELINK), 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.fusOptions = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.fusOptions);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.usPhaseGroup = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.usPhaseGroup);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.ulChannel = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.ulChannel);
            mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.ulTableIndex = FMOD_SWAPENDIAN_DWORD(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mWaveLink.ulTableIndex);
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "art1", 4))
        {
            DLS_CONNECTIONLIST list;

            result = mFile->read(&list, 1, sizeof(DLS_CONNECTIONLIST), 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            list.cbSize = FMOD_SWAPENDIAN_DWORD(list.cbSize);
            list.cConnections = FMOD_SWAPENDIAN_DWORD(list.cConnections);

            if (list.cbSize > sizeof(DLS_CONNECTIONLIST))
            {
                mFile->seek(list.cbSize - sizeof(DLS_CONNECTIONLIST), 1);
            }

            if (mCurrentRegion < mInstrument[mCurrentInstrument].mHeader.cRegions)
            {
                unsigned int count;















                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection = (DLS_CONNECTION *)FMOD_Memory_Calloc(list.cConnections * sizeof(DLS_CONNECTION));
                if (!mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection)
                {
                    return FMOD_ERR_MEMORY;
                }

                mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mNumConnections = list.cConnections;

                result = mFile->read(mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection, 1, list.cConnections * sizeof(DLS_CONNECTION), 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                for (count = 0; count < list.cConnections; count++)
                {
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usSource = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usSource);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usControl = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usControl);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usDestination = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usDestination);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usTransform = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].usTransform);
                    mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].lScale = FMOD_SWAPENDIAN_DWORD((unsigned int)mInstrument[mCurrentInstrument].mRegion[mCurrentRegion].mConnection[count].lScale);
                }
            }
            else
            {
                unsigned int count;





                mInstrument[mCurrentInstrument].mConnection = (DLS_CONNECTION *)FMOD_Memory_Calloc(list.cConnections * sizeof(DLS_CONNECTION));
                if (!mInstrument[mCurrentInstrument].mConnection)
                {
                    return FMOD_ERR_MEMORY;
                }

                mInstrument[mCurrentInstrument].mNumConnections = list.cConnections;

                result = mFile->read(mInstrument[mCurrentInstrument].mConnection, 1, list.cConnections * sizeof(DLS_CONNECTION), 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                for (count = 0; count < list.cConnections; count++)
                {
                    mInstrument[mCurrentInstrument].mConnection[count].usSource = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mConnection[count].usSource);
                    mInstrument[mCurrentInstrument].mConnection[count].usControl = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mConnection[count].usControl);
                    mInstrument[mCurrentInstrument].mConnection[count].usDestination = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mConnection[count].usDestination);
                    mInstrument[mCurrentInstrument].mConnection[count].usTransform = FMOD_SWAPENDIAN_WORD((unsigned short)mInstrument[mCurrentInstrument].mConnection[count].usTransform);
                    mInstrument[mCurrentInstrument].mConnection[count].lScale = FMOD_SWAPENDIAN_DWORD((unsigned int)mInstrument[mCurrentInstrument].mConnection[count].lScale);
                }
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "fmt ", 4))
        {
            WAVE_FORMATEXTENSIBLE srcformat;
            unsigned int toread = chunk.size;

            if (toread > sizeof(WAVE_FORMATEXTENSIBLE))
            {
                toread = sizeof(WAVE_FORMATEXTENSIBLE);
            }

            memset(&srcformat, 0, sizeof(WAVE_FORMATEXTENSIBLE));

            result = mFile->read(&srcformat, 1, toread, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (chunk.size > sizeof(WAVE_FORMATEXTENSIBLE))
            {
                result = mFile->seek(chunk.size - sizeof(WAVE_FORMATEXTENSIBLE), 1);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            srcformat.Format.wFormatTag = FMOD_SWAPENDIAN_WORD(srcformat.Format.wFormatTag);
            srcformat.Format.nChannels = FMOD_SWAPENDIAN_WORD(srcformat.Format.nChannels);
            srcformat.Format.nSamplesPerSec = FMOD_SWAPENDIAN_DWORD(srcformat.Format.nSamplesPerSec);
            srcformat.Format.nAvgBytesPerSec = FMOD_SWAPENDIAN_DWORD(srcformat.Format.nAvgBytesPerSec);
            srcformat.Format.nBlockAlign = FMOD_SWAPENDIAN_WORD(srcformat.Format.nBlockAlign);
            srcformat.Format.wBitsPerSample = FMOD_SWAPENDIAN_WORD(srcformat.Format.wBitsPerSample);
            srcformat.Format.cbSize = FMOD_SWAPENDIAN_WORD(srcformat.Format.cbSize);
            srcformat.Samples.wValidBitsPerSample = FMOD_SWAPENDIAN_WORD(srcformat.Samples.wValidBitsPerSample);
            srcformat.dwChannelMask = FMOD_SWAPENDIAN_DWORD(srcformat.dwChannelMask);

            switch (srcformat.Format.wBitsPerSample)
            {
                case 8:
                {
                    waveformat[mCurrentSample].format = FMOD_SOUND_FORMAT_PCM8;
                    break;
                }
                case 16:
                {
                    waveformat[mCurrentSample].format = FMOD_SOUND_FORMAT_PCM16;
                    break;
                }
                case 24:
                {
                    waveformat[mCurrentSample].format = FMOD_SOUND_FORMAT_PCM24;
                    break;
                }
                case 32:
                {
                    if (srcformat.Format.wFormatTag == 1)
                    {
                        waveformat[mCurrentSample].format = FMOD_SOUND_FORMAT_PCM32;
                    }
                    else if (srcformat.Format.wFormatTag == 3)
                    {
                        waveformat[mCurrentSample].format = FMOD_SOUND_FORMAT_PCMFLOAT;
                    }
                    break;
                }
            }

            waveformat[mCurrentSample].channels = srcformat.Format.nChannels;
            waveformat[mCurrentSample].frequency = srcformat.Format.nSamplesPerSec;
            waveformat[mCurrentSample].blockalign = srcformat.Format.nBlockAlign;
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "data", 4))
        {
            SoundI::getSamplesFromBytes(chunk.size, &waveformat[mCurrentSample].lengthpcm, waveformat[mCurrentSample].channels, waveformat[mCurrentSample].format);

            result = mFile->tell(&mSample[mCurrentSample].mDataOffset);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "INAM", 4))
        {
            if (mCurrentInstrument < mNumInstruments)
            {
                memset(mInstrument[mCurrentInstrument].mName, 0, 256);

                result = mFile->read(mInstrument[mCurrentInstrument].mName, 1, chunk.size, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }
            else if (mCurrentSample < mNumSamples)
            {
                memset(mSample[mCurrentSample].mName, 0, 256);

                result = mFile->read(mSample[mCurrentSample].mName, 1, chunk.size, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                FMOD_strncpy(waveformat[mCurrentSample].name, mSample[mCurrentSample].mName, 256);
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "IARL", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IART", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ICMS", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ICMT", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ICOP", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ICRD", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IENG", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IGNR", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IKEY", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IMED", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "IPRD", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ISBJ", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ISFT", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ISRC", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ISRF", 4) ||
                 !FMOD_strncmp((const char *)chunk.id, "ITCH", 4))
        {
        }
        else
        {
            mFile->seek(chunk.size, 1);

            if (result != FMOD_OK)
            {
                return result;
            }
        }

        size += chunk.size + sizeof(WAVE_CHUNK);
        offset += chunk.size + sizeof(WAVE_CHUNK);

        if (chunk.size & 1)
        {
            size++;
            offset++;
        }

    } while (size < chunksize && size);

    return FMOD_OK;
}

FMOD_RESULT CodecDLS::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    WAVE_CHUNK chunk;
    char dls[4];

    init(FMOD_SOUND_TYPE_DLS);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mSrcDataOffset = 0;
    mNumInstruments = 0;
    mNumSamples = 0;

    result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp((const char *)chunk.id, "RIFF", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mFile->read(dls, 1, 4, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (FMOD_strncmp(dls, "DLS ", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);

    mSrcDataOffset = 0;
    mCurrentSample = 0;

    result = parseChunk(dls, chunk.size);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mNumInstruments <= 0)
    {
        return FMOD_ERR_FORMAT;
    }

    memcpy(&mWaveFormat, waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = mNumSamples;

    result = mFile->getSize(&mWaveFormat.lengthbytes);
    if (result != FMOD_OK)
    {
        return result;
    }

    return result;
}

FMOD_RESULT CodecDLS::closeInternal()
{
    if (waveformat)
    {
























































        FMOD_Memory_Free(waveformat);
        waveformat = 0;
    }

    if (mInstrument)
    {
        int count;

        for (count = 0; count < mNumInstruments; count++)
        {
            if (mInstrument[count].mRegion)
            {
                unsigned int count2;

                for (count2 = 0; count2 < mInstrument[count].mHeader.cRegions; count2++)
                {
                    if (mInstrument[count].mRegion[count2].mConnection)
                    {



                        FMOD_Memory_Free(mInstrument[count].mRegion[count2].mConnection);
                    }
                }

                FMOD_Memory_Free(mInstrument[count].mRegion);
            }
            if (mInstrument[count].mConnection)
            {
                FMOD_Memory_Free(mInstrument[count].mConnection);
            }
        }

        FMOD_Memory_Free(mInstrument);
        mInstrument = 0;
    }

    if (mSample)
    {
        FMOD_Memory_Free(mSample);
        mSample = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecDLS::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result;
    unsigned int count;
    short * wptr;

    result = mFile->read(buffer, 1, sizebytes, bytesread);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    wptr = (short *)buffer;
    for (count = 0; count < *bytesread / 2; count++)
    {
        wptr[count] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[count]);
    }

    return result;
}

FMOD_RESULT CodecDLS::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;

    if (subsound < 0 || (numsubsounds && subsound >= numsubsounds))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFile->mSeekable)
    {
        unsigned int bytes;

        if (subsound != mCurrentIndex)
        {
            mCurrentIndex = subsound;
        }

        result = SoundI::getBytesFromSamples(position, &bytes, waveformat[mCurrentIndex].channels, waveformat[mCurrentIndex].format);
        if (result != FMOD_OK)
        {
            return result;
        }

        bytes += mSample[mCurrentIndex].mDataOffset;

        result = mFile->seek(bytes, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return result;
}

FMOD_RESULT CodecDLS::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecDLS * dls = (CodecDLS *)codec;

    return dls->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecDLS::closeCallback(FMOD_CODEC_STATE * codec)
{
    CodecDLS * dls = (CodecDLS *)codec;

    return dls->closeInternal();
}

FMOD_RESULT CodecDLS::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecDLS * dls = (CodecDLS *)codec;

    return dls->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecDLS::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecDLS * dls = (CodecDLS *)codec;

    return dls->setPositionInternal(subsound, position, postype);
}

} // namespace FMOD
