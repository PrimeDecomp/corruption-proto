// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805C5794..0x805C84F4 (14 native functions).
// Original basename named by the allocator file string (openInternal 0x805C5A94).
// Follows the 4.06 PS3 structure where G2MEAB agrees. G2MEAB differences: no getWaveFormat, reset or
// canPoint entry points (the wave formats are built in openInternal), no MPEG/XMA support, the IMA ADPCM
// template codec and the GameCube ADPCM channel headers are prepared in openInternal, readInternal
// decodes IMA ADPCM itself when the sample is not kept compressed, and soundcreateInternal only applies
// the 3.1 variations and 3D distances and hands over the sync points.
// Also emits the weak Plugin (0x805C706C) and CodecWav (0x805C83C0) deleting destructors.

#include "fmod_codec_fsb.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codec_wav.h"
#include "fmod_codec_wav_imaadpcm.h"
#include "fmod_codeci.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_codecpool.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_syncpoint.h"
#include "fmod_systemi.h"
#include "fmod_types.h"

#include <string.h>

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX fsbcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecFSB::getDescriptionEx()
{
    memset(&fsbcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    fsbcodec.name = "FMOD FSB Codec";
    fsbcodec.version = 0x00010100;
    fsbcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    fsbcodec.open = &CodecFSB::openCallback;
    fsbcodec.close = &CodecFSB::closeCallback;
    fsbcodec.read = &CodecFSB::readCallback;
    fsbcodec.setposition = &CodecFSB::setPositionCallback;
    fsbcodec.soundcreate = &CodecFSB::soundcreateCallback;

    fsbcodec.mType = FMOD_SOUND_TYPE_FSB;
    fsbcodec.mSize = sizeof(CodecFSB);

    return &fsbcodec;
}

FMOD_RESULT CodecFSB::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    unsigned int rd;
    int count;
    char * shdrblock;
    FMOD_RESULT result;
    unsigned int offset, soffset;

    init(FMOD_SOUND_TYPE_FSB);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));
    numsubsounds = 0;
    waveformat = 0;

    result = mFile->seek(0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(&mHeader, 1, sizeof(FMOD_FSB_HEADER), &rd);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (rd != sizeof(FMOD_FSB_HEADER))
    {
        return FMOD_ERR_FILE_BAD;
    }

    if (!FMOD_strncmp(mHeader.id, "FSB2", 4))
    {
        mHeader.version = 0;
        mHeader.mode = 0;

        result = mFile->seek(-8, 1);
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (FMOD_strncmp(mHeader.id, "FSB3", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    mHeader.numsamples = FMOD_SWAPENDIAN_DWORD((unsigned int)mHeader.numsamples);
    mHeader.datasize = FMOD_SWAPENDIAN_DWORD(mHeader.datasize);
    mHeader.shdrsize = FMOD_SWAPENDIAN_DWORD(mHeader.shdrsize);
    mHeader.version = FMOD_SWAPENDIAN_DWORD(mHeader.version);
    mHeader.mode = FMOD_SWAPENDIAN_DWORD(mHeader.mode);

    mSrcDataOffset = 0;

    if (mHeader.numsamples < 1)
    {
        return FMOD_ERR_FILE_BAD;
    }

    if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
    {











































































































        mShdrb = (FMOD_FSB_SAMPLE_HEADER_BASIC * *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(void *));
        if (!mShdrb)
        {
            return FMOD_ERR_MEMORY;
        }
    }
    else
    {
        mShdr = (FMOD_FSB_SAMPLE_HEADER * *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(void *));
        if (!mShdr)
        {
            return FMOD_ERR_MEMORY;
        }
    }




    shdrblock = (char *)FMOD_Memory_Calloc(mHeader.shdrsize);
    if (!shdrblock)
    {
        return FMOD_ERR_MEMORY;
    }

    result = mFile->read(shdrblock, 1, mHeader.shdrsize, &rd);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (rd != mHeader.shdrsize)
    {
        return FMOD_ERR_FILE_BAD;
    }

    mSrcDataOffset = mHeader.shdrsize + (mHeader.version ? sizeof(FMOD_FSB_HEADER) : 16);




    mDataOffset = (unsigned int *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(unsigned int));
    if (!mDataOffset)
    {
        return FMOD_ERR_MEMORY;
    }

    offset = 0;
    soffset = mSrcDataOffset;
    mFirstSample = 0;






    waveformat = (FMOD_CODEC_WAVEFORMAT *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(FMOD_CODEC_WAVEFORMAT));
    if (!waveformat)
    {
        return FMOD_ERR_MEMORY;
    }

    for (count = 0; count < mHeader.numsamples; count++)
    {
        if ((mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS) && count)
        {
            FMOD_FSB_SAMPLE_HEADER_BASIC * s = (FMOD_FSB_SAMPLE_HEADER_BASIC *)(shdrblock + offset);

            mShdrb[count] = s;
            mDataOffset[count] = soffset;

            s->lengthsamples = FMOD_SWAPENDIAN_DWORD(s->lengthsamples);
            s->lengthcompressedbytes = FMOD_SWAPENDIAN_DWORD(s->lengthcompressedbytes);

            soffset += s->lengthcompressedbytes;
            offset += sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC);

            if (mFirstSample->mode & FSOUND_GCADPCM)
            {
                int sizeofheader = (mHeader.version == FMOD_FSB_VERSION_3_1) ? sizeof(FMOD_FSB_SAMPLE_HEADER_3_1) : sizeof(FMOD_FSB_SAMPLE_HEADER);

                offset += mFirstSample->size - sizeofheader;
            }

            if (mFirstSample->mode & FSOUND_XMA)
            {
                return FMOD_ERR_FORMAT;
            }
        }
        else
        {
            FMOD_FSB_SAMPLE_HEADER * s = (FMOD_FSB_SAMPLE_HEADER *)(shdrblock + offset);

            if (!count)
            {
                mFirstSample = s;
            }

            if (!(mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS))
            {
                mShdr[count] = s;
            }

            if (s->name[0])
            {
                mDataOffset[count] = soffset;
            }
            else
            {
                if (!count)
                {
                    return FMOD_ERR_FILE_BAD;
                }

                mDataOffset[count] = mDataOffset[count - 1];
            }

            s->size = FMOD_SWAPENDIAN_WORD(s->size);
            s->lengthsamples = FMOD_SWAPENDIAN_DWORD(s->lengthsamples);
            s->lengthcompressedbytes = FMOD_SWAPENDIAN_DWORD(s->lengthcompressedbytes);
            s->loopstart = FMOD_SWAPENDIAN_DWORD(s->loopstart);
            s->loopend = FMOD_SWAPENDIAN_DWORD(s->loopend);
            s->mode = FMOD_SWAPENDIAN_DWORD(s->mode);
            s->deffreq = FMOD_SWAPENDIAN_DWORD((unsigned int)s->deffreq);
            s->defvol = FMOD_SWAPENDIAN_WORD(s->defvol);
            s->defpan = (short)FMOD_SWAPENDIAN_WORD((unsigned short)s->defpan);
            s->defpri = FMOD_SWAPENDIAN_WORD(s->defpri);
            s->numchannels = FMOD_SWAPENDIAN_WORD(s->numchannels);

            if (mHeader.version == FMOD_FSB_VERSION_3_1)
            {
                FMOD_FSB_SAMPLE_HEADER_3_1 tmp;

                memcpy(&tmp, s, sizeof(FMOD_FSB_SAMPLE_HEADER_3_1));

                tmp.varfreq = FMOD_SWAPENDIAN_DWORD((unsigned int)tmp.varfreq);
                tmp.varvol = FMOD_SWAPENDIAN_DWORD((unsigned int)tmp.varvol);
                tmp.varpan = FMOD_SWAPENDIAN_DWORD((unsigned int)tmp.varpan);
                {
                    unsigned int * __tmp = (unsigned int *)&tmp.mindistance;
                    *__tmp = FMOD_SWAPENDIAN_DWORD(*__tmp);
                }
                {
                    unsigned int * __tmp = (unsigned int *)&tmp.maxdistance;
                    *__tmp = FMOD_SWAPENDIAN_DWORD(*__tmp);
                }

                memcpy(s, &tmp, sizeof(FMOD_FSB_SAMPLE_HEADER_3_1));
            }

            s->defvol <<= 2;
            s->defpan <<= 2;
            s->defpri <<= 2;

            if (mHeader.version == FMOD_FSB_VERSION_3_1)
            {
                FMOD_FSB_SAMPLE_HEADER_3_1 tmp;

                memcpy(&tmp, s, sizeof(FMOD_FSB_SAMPLE_HEADER_3_1));

                tmp.varvol <<= 2;
                tmp.varpan <<= 2;

                memcpy(s, &tmp, sizeof(FMOD_FSB_SAMPLE_HEADER_3_1));
            }

            soffset += s->name[0] ? s->lengthcompressedbytes : 0;
            offset += s->size;
        }
    }

    if (mFirstSample->mode & FSOUND_GCADPCM)
    {
        void * * channelinfo;

















































        channelinfo = (void * *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(void *));
        if (!channelinfo)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < mHeader.numsamples; count++)
        {
            if ((mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS) && count)
            {
                channelinfo[count] = FMOD_Memory_Calloc(0x2E);
                if (!channelinfo[count])
                {
                    return FMOD_ERR_MEMORY;
                }

                memcpy(channelinfo[count], (char *)mShdrb[count] + sizeof(FMOD_FSB_SAMPLE_HEADER_BASIC), 0x2E);
            }
            else
            {
                FMOD_FSB_SAMPLE_HEADER * s = !count ? mFirstSample : mShdr[count];

                channelinfo[count] = FMOD_Memory_Calloc(s->numchannels * 0x2E);
                if (!channelinfo[count])
                {
                    return FMOD_ERR_MEMORY;
                }

                if (mHeader.version == FMOD_FSB_VERSION_3_1)
                {
                    memcpy(channelinfo[count], (char *)s + sizeof(FMOD_FSB_SAMPLE_HEADER_3_1), s->numchannels * 0x2E);
                }
                else
                {
                    memcpy(channelinfo[count], (char *)s + sizeof(FMOD_FSB_SAMPLE_HEADER), s->numchannels * 0x2E);
                }
            }
        }

        plugindata = channelinfo;
    }
    else if (mFirstSample->mode & FSOUND_XMA)
    {
        return FMOD_ERR_FORMAT;
    }
    else if (mFirstSample->mode & FSOUND_IMAADPCM)
    {
        if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
        {
            mDecodeADPCM = true;
        }

        if (mDecodeADPCM)
        {
























































































































































































            mADPCM = FMOD_Object_Calloc(CodecWav);
            if (!mADPCM)
            {
                return FMOD_ERR_MEMORY;
            }

            memcpy(&mADPCM->mDescription, CodecWav::getDescriptionEx(), sizeof(FMOD_CODEC_DESCRIPTION_EX));
            mADPCM->mFile = mFile;
            mADPCM->mPCMBufferLengthBytes = mPCMBufferLengthBytes;

            mWaveFormat.format = FMOD_SOUND_FORMAT_IMAADPCM;
        }
    }

    if (!(mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS))
    {
        int fsbheadersize, headersize;

        offset = 0;

        fsbheadersize = (mHeader.version == FMOD_FSB_VERSION_3_1) ? sizeof(FMOD_FSB_SAMPLE_HEADER_3_1) : sizeof(FMOD_FSB_SAMPLE_HEADER);
        headersize = fsbheadersize;

        if (mFirstSample->mode & FSOUND_GCADPCM)
        {
            headersize = fsbheadersize + mShdr[0]->numchannels * 0x2E;
        }

        for (count = 0; count < mHeader.numsamples; count++)
        {
            FMOD_FSB_SAMPLE_HEADER * s = (FMOD_FSB_SAMPLE_HEADER *)(shdrblock + offset);

            if (s->mode & FSOUND_SYNCPOINTS)
            {
                char * syncpointdata = (char *)s + headersize;

                if (!FMOD_strncmp(syncpointdata, "SYNC", 4))
                {
                    if (!mNumSyncPoints)
                    {


















































                        mNumSyncPoints = (int *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(int));
                        if (!mNumSyncPoints)
                        {
                            return FMOD_ERR_MEMORY;
                        }

                        mSyncPoint = (SyncPoint * *)FMOD_Memory_Calloc(mHeader.numsamples * sizeof(void *));
                        if (!mSyncPoint)
                        {
                            return FMOD_ERR_MEMORY;
                        }
                    }

                    mNumSyncPoints[count] = *(int *)(syncpointdata + 4);
                    mNumSyncPoints[count] = FMOD_SWAPENDIAN_DWORD((unsigned int)mNumSyncPoints[count]);

                    if (mNumSyncPoints)
                    {
                        int count2;

                        syncpointdata += 8;




                        mSyncPoint[count] = (SyncPoint *)FMOD_Memory_Calloc(mNumSyncPoints[count] * sizeof(SyncPoint));
                        if (!mSyncPoint[count])
                        {
                            return FMOD_ERR_MEMORY;
                        }

                        for (count2 = 0; count2 < mNumSyncPoints[count]; count2++)
                        {
                            SYNCDATA * syncdata = (SYNCDATA *)syncpointdata;

                            FMOD_strncpy(mSyncPoint[count][count2].mName, syncdata->name, 256);
                            mSyncPoint[count][count2].mOffset = syncdata->offset;
                            mSyncPoint[count][count2].mOffset = FMOD_SWAPENDIAN_DWORD(mSyncPoint[count][count2].mOffset);

                            syncpointdata += sizeof(SYNCDATA);
                        }
                    }
                }
            }

            offset += s->size;
        }
    }

    for (count = 0; count < mHeader.numsamples; count++)
    {
        if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
        {
            if (!count)
            {
                unsigned int mode = waveformat[0].mode & ~(FMOD_2D | FMOD_3D | FMOD_HARDWARE | FMOD_SOFTWARE);

                if (mFirstSample->mode & FSOUND_2D)
                {
                    mode |= FMOD_SOFTWARE | FMOD_2D;
                }
                else if (mFirstSample->mode & FSOUND_HW2D)
                {
                    mode |= FMOD_HARDWARE | FMOD_2D;
                }
                else if (mFirstSample->mode & FSOUND_HW3D)
                {
                    mode |= FMOD_HARDWARE | FMOD_3D;
                }
                else if (mFirstSample->mode & FSOUND_3D)
                {
                    mode |= FMOD_SOFTWARE | FMOD_3D;
                }
                waveformat[0].mode = mode;

                FMOD_strncpy(waveformat[0].name, mFirstSample->name, 256);
                waveformat[0].channels = mFirstSample->numchannels;
                waveformat[0].frequency = mFirstSample->deffreq;
                waveformat[0].lengthpcm = mFirstSample->lengthsamples;
                waveformat[0].lengthbytes = mFirstSample->lengthcompressedbytes;

                if (waveformat[count].channels > mMaxChannels)
                {
                    mMaxChannels = waveformat[count].channels;
                }

                if (mFirstSample->mode & FSOUND_8BITS)
                {
                    waveformat[0].format = FMOD_SOUND_FORMAT_PCM8;
                }
                else if (mFirstSample->mode & FSOUND_16BITS)
                {
                    waveformat[0].format = FMOD_SOUND_FORMAT_PCM16;
                }
                else if (mFirstSample->mode & FSOUND_IMAADPCM)
                {
                    mReadBufferLength = mFirstSample->numchannels * 36;

                    if (mADPCM)
                    {
                        waveformat[0].format = FMOD_SOUND_FORMAT_IMAADPCM;
                        mPCMBufferLength = 64;
                        mPCMBufferLengthBytes = mFirstSample->numchannels * (mPCMBufferLength * sizeof(float));
                    }
                    else
                    {
                        waveformat[0].format = FMOD_SOUND_FORMAT_PCM16;
                        mPCMBufferLength = 64;
                        mPCMBufferLengthBytes = mFirstSample->numchannels * (mPCMBufferLength * sizeof(short));
                    }
                }
                else if (mFirstSample->mode & FSOUND_VAG)
                {
                    waveformat[0].format = FMOD_SOUND_FORMAT_VAG;
                }
                else if (mFirstSample->mode & FSOUND_GCADPCM)
                {
                    waveformat[0].format = FMOD_SOUND_FORMAT_GCADPCM;
                }

                if (!waveformat[0].blockalign)
                {
                    if (waveformat[0].format == FMOD_SOUND_FORMAT_VAG)
                    {
                        waveformat[0].blockalign = 4096;
                    }
                    else if (waveformat[0].format == FMOD_SOUND_FORMAT_GCADPCM)
                    {
                        waveformat[0].blockalign = 32;
                    }
                    else if (waveformat[0].format == FMOD_SOUND_FORMAT_XMA)
                    {
                        waveformat[0].blockalign = 2048;
                    }
                    else
                    {
                        SoundI::getBytesFromSamples(1, (unsigned int *)&waveformat[0].blockalign, waveformat[0].channels, waveformat[0].format);
                    }
                }
            }
            else
            {
                FMOD_strncpy(waveformat[count].name, waveformat[0].name, 256);
                waveformat[count].blockalign = waveformat[0].blockalign;
                waveformat[count].channelmask = waveformat[0].channelmask;
                waveformat[count].channels = waveformat[0].channels;
                waveformat[count].format = waveformat[0].format;
                waveformat[count].frequency = waveformat[0].frequency;
                waveformat[count].lengthbytes = mShdrb[count]->lengthcompressedbytes;
                waveformat[count].lengthpcm = mShdrb[count]->lengthsamples;
                waveformat[count].mode = waveformat[0].mode;
            }
        }
        else
        {
            unsigned int mode = waveformat[count].mode & ~(FMOD_2D | FMOD_3D | FMOD_HARDWARE | FMOD_SOFTWARE);

            if (mShdr[count]->mode & FSOUND_2D)
            {
                mode |= FMOD_SOFTWARE | FMOD_2D;
            }
            else if (mShdr[count]->mode & FSOUND_HW2D)
            {
                mode |= FMOD_HARDWARE | FMOD_2D;
            }
            else if (mShdr[count]->mode & FSOUND_HW3D)
            {
                mode |= FMOD_HARDWARE | FMOD_3D;
            }
            else if (mShdr[count]->mode & FSOUND_3D)
            {
                mode |= FMOD_SOFTWARE | FMOD_3D;
            }
            waveformat[count].mode = mode;

            FMOD_strncpy(waveformat[count].name, mShdr[count]->name, 256);
            waveformat[count].channels = mShdr[count]->numchannels;
            waveformat[count].frequency = mShdr[count]->deffreq;
            waveformat[count].lengthpcm = mShdr[count]->lengthsamples;
            waveformat[count].loopstart = mShdr[count]->loopstart;
            waveformat[count].loopend = mShdr[count]->loopend;
            waveformat[count].lengthbytes = mShdr[count]->lengthcompressedbytes;

            if (waveformat[count].channels > mMaxChannels)
            {
                mMaxChannels = waveformat[count].channels;
            }

            if (mShdr[count]->mode & FSOUND_8BITS)
            {
                waveformat[count].format = FMOD_SOUND_FORMAT_PCM8;
            }
            else if (mShdr[count]->mode & FSOUND_16BITS)
            {
                waveformat[count].format = FMOD_SOUND_FORMAT_PCM16;
            }
            else if (mShdr[count]->mode & FSOUND_IMAADPCM)
            {
                mReadBufferLength = mMaxChannels * 36;

                if (mDecodeADPCM)
                {
                    waveformat[count].format = FMOD_SOUND_FORMAT_IMAADPCM;
                    mPCMBufferLength = 64;
                    mPCMBufferLengthBytes = mMaxChannels * (mPCMBufferLength * sizeof(float));
                }
                else
                {
                    waveformat[count].format = FMOD_SOUND_FORMAT_PCM16;
                    mPCMBufferLength = 64;
                    mPCMBufferLengthBytes = mMaxChannels * (mPCMBufferLength * sizeof(short));
                }
            }
            else if (mShdr[count]->mode & FSOUND_VAG)
            {
                waveformat[count].format = FMOD_SOUND_FORMAT_VAG;
            }
            else if (mShdr[count]->mode & FSOUND_GCADPCM)
            {
                waveformat[count].format = FMOD_SOUND_FORMAT_GCADPCM;
            }

            if (!waveformat[count].blockalign)
            {
                if (waveformat[count].format == FMOD_SOUND_FORMAT_VAG)
                {
                    waveformat[count].blockalign = 4096;
                }
                else if (waveformat[count].format == FMOD_SOUND_FORMAT_GCADPCM)
                {
                    waveformat[count].blockalign = 32;
                }
                else if (waveformat[count].format == FMOD_SOUND_FORMAT_XMA)
                {
                    waveformat[count].blockalign = 2048;
                }
                else
                {
                    SoundI::getBytesFromSamples(1, (unsigned int *)&waveformat[count].blockalign, waveformat[count].channels, waveformat[count].format);
                }
            }
        }
    }

    if (mReadBufferLength)
    {


























































































        mReadBuffer = (unsigned char *)FMOD_Memory_Calloc(mReadBufferLength * 10);
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

    numsubsounds = mHeader.numsamples;

    memcpy(&mWaveFormat, waveformat, sizeof(FMOD_CODEC_WAVEFORMAT));
    mWaveFormat.channels = mMaxChannels;

    result = mFile->getSize(&mWaveFormat.lengthbytes);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        if (mMaxChannels > 2)
        {
            return FMOD_ERR_TOOMANYCHANNELS;
        }

        if (mADPCM && mWaveFormat.format == FMOD_SOUND_FORMAT_IMAADPCM)
        {
            memcpy(&mADPCM->mWaveFormat, &mWaveFormat, sizeof(FMOD_CODEC_WAVEFORMAT));
            mADPCM->mReadBufferLength = mReadBufferLength;
            mADPCM->mReadBuffer = mReadBuffer;
            mADPCM->mPCMBufferLength = mPCMBufferLength;
            mADPCM->mPCMBufferLengthBytes = mPCMBufferLengthBytes;

















































            mADPCM->mSrcFormat = (WAVE_FORMATEXTENSIBLE *)FMOD_Memory_Calloc(sizeof(WAVE_FORMATEX) + sizeof(unsigned short));
            if (!mADPCM->mSrcFormat)
            {
                return FMOD_ERR_MEMORY;
            }

            mADPCM->mSrcFormat->Format.wFormatTag = 0x11;
            mADPCM->mSrcFormat->Format.nBlockAlign = mMaxChannels * 36;
            mADPCM->mSrcFormat->Format.nChannels = 1;
            mADPCM->mSrcFormat->Samples.wSamplesPerBlock = 64;
            mADPCM->mSamplesPerADPCMBlock = mADPCM->mSrcFormat->Samples.wSamplesPerBlock;

            if (mSystem->mDSPCodecPool_ADPCM.mNumDSPCodecs)
            {
                if (mPCMBufferLengthBytes > ((DSPCodec *)mSystem->mDSPCodecPool_ADPCM.mPool[0])->mCodec->mPCMBufferLengthBytes)
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
                result = mSystem->mDSPCodecPool_ADPCM.init(mADPCM, mSystem->mAdvancedSettings.maxADPCMcodecs);
                if (result != FMOD_OK)
                {
                    return result;
                }
            }

            for (count = 0; count < mSystem->mDSPCodecPool_ADPCM.mNumDSPCodecs; count++)
            {
                ((DSPCodec *)mSystem->mDSPCodecPool_ADPCM.mPool[count])->mCodec->mSrcDataOffset = 0;
            }
        }
    }

    return result;
}

FMOD_RESULT CodecFSB::closeInternal()
{
    if (waveformat)
    {


























        FMOD_Memory_Free(waveformat);
        waveformat = 0;
    }

    if (mFirstSample)
    {
        FMOD_Memory_Free(mFirstSample);
        mFirstSample = 0;
    }

    if (mShdr)
    {
        FMOD_Memory_Free(mShdr);
        mShdr = 0;
    }

    if (mNumSyncPoints)
    {
        FMOD_Memory_Free(mNumSyncPoints);
        mNumSyncPoints = 0;
    }

    if (mSyncPoint)
    {
        FMOD_Memory_Free(mSyncPoint);
        mSyncPoint = 0;
    }

    if (mShdrb)
    {
        FMOD_Memory_Free(mShdrb);
        mShdrb = 0;
    }

    if (mDataOffset)
    {
        FMOD_Memory_Free(mDataOffset);
        mDataOffset = 0;
    }

    if (mPCMBuffer)
    {
        FMOD_Memory_Free(mPCMBuffer);
        mPCMBuffer = 0;
    }
    mPCMBufferLengthBytes = 0;

    if (mReadBuffer)
    {
        FMOD_Memory_Free(mReadBuffer);
        mReadBuffer = 0;
    }
    mReadBufferLength = 0;

    if (plugindata)
    {
        void * * channelinfo = (void * *)plugindata;
        int i;

        for (i = 0; i < mHeader.numsamples; i++)
        {
            if (channelinfo[i])
            {

                FMOD_Memory_Free(channelinfo[i]);
                channelinfo[i] = 0;
            }
        }
    }

    if (plugindata)
    {

        FMOD_Memory_Free(plugindata);
        plugindata = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecFSB::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result;
    unsigned int bytesreadinternal;
    unsigned int srcmode;

    if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
    {
        srcmode = mFirstSample->mode;
    }
    else
    {
        srcmode = mShdr[mCurrentIndex]->mode;
    }

    if ((srcmode & FSOUND_IMAADPCM) && waveformat[mCurrentIndex].format == FMOD_SOUND_FORMAT_PCM16)
    {
        int blockalign = mWaveFormat.channels * 36;
        int readbufflength = blockalign;

        result = mFile->read(mReadBuffer, 1, blockalign, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mWaveFormat.channels == 1)
        {
            result = IMAAdpcm_DecodeM16(mReadBuffer, (short *)mPCMBuffer, 1, readbufflength, 64, 1);
        }
        else if (mWaveFormat.channels == 2)
        {
            result = IMAAdpcm_DecodeS16(mReadBuffer, (short *)mPCMBuffer, 1, readbufflength, 64);
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
                    tempin[count2] = ((short *)mReadBuffer)[count + count2 * mWaveFormat.channels];
                }

                result = IMAAdpcm_DecodeM16((unsigned char *)tempin, (short *)mPCMBuffer + count, 1, blockalign, 64, mWaveFormat.channels);
            }
        }

        bytesreadinternal = 64 * sizeof(short) * mWaveFormat.channels;
    }
    else
    {
        result = mFile->read(buffer, 1, mWaveFormat.channels * (sizebytes / mMaxChannels), &bytesreadinternal);
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCM8)
        {
            unsigned char * ptr = (unsigned char *)buffer;
            unsigned int count;

            count = bytesreadinternal >> 3;
            while (count)
            {
                ptr[0] ^= 128;
                ptr[1] ^= 128;
                ptr[2] ^= 128;
                ptr[3] ^= 128;
                ptr[4] ^= 128;
                ptr[5] ^= 128;
                ptr[6] ^= 128;
                ptr[7] ^= 128;
                ptr += 8;
                count--;
            }

            count = bytesreadinternal & 7;
            while (count)
            {
                ptr[0] ^= 128;
                ptr++;
                count--;
            }
        }

        if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCM16)
        {
            unsigned int count;
            short * wptr = (short *)buffer;

            for (count = 0; count < bytesreadinternal / 2; count++)
            {
                wptr[count] = (short)FMOD_SWAPENDIAN_WORD((unsigned short)wptr[count]);
            }
        }
    }

    if (mWaveFormat.channels < mMaxChannels)
    {
        int count, block;
        int blocksize, numblocks;
        char * srcptr, * destptr;

        blocksize = mWaveFormat.blockalign;
        if ((srcmode & FSOUND_IMAADPCM) && waveformat[mCurrentIndex].format == FMOD_SOUND_FORMAT_IMAADPCM && mMaxChannels == 2)
        {
            blocksize = 4;
        }

        srcptr = (char *)buffer + bytesreadinternal - blocksize * mWaveFormat.channels;
        destptr = (char *)buffer + mMaxChannels * (bytesreadinternal / mWaveFormat.channels) - blocksize * mMaxChannels;
        numblocks = bytesreadinternal / blocksize;

        if (blocksize == 1)
        {
            for (count = 0; count < numblocks; count++)
            {
                int srcchannel = mWaveFormat.channels - 1;

                for (block = mMaxChannels - 1; block >= 0; block--)
                {
                    destptr[block] = srcptr[srcchannel];
                    srcchannel--;
                    if (srcchannel < 0)
                    {
                        srcchannel = mWaveFormat.channels - 1;
                    }
                }

                srcptr -= mWaveFormat.channels;
                destptr -= mMaxChannels;
            }
        }
        else if (blocksize == 2)
        {
            short * srcptrw = (short *)srcptr;
            short * destptrw = (short *)destptr;

            if (mWaveFormat.channels == 1)
            {
                for (count = 0; count < numblocks; count++)
                {
                    for (block = mMaxChannels - 1; block >= 0; block--)
                    {
                        destptrw[block] = *srcptrw;
                    }

                    srcptrw--;
                    destptrw -= mMaxChannels;
                }
            }
            else
            {
                for (count = 0; count < numblocks; count++)
                {
                    int srcchannel = mWaveFormat.channels - 1;

                    for (block = mMaxChannels - 1; block >= 0; block--)
                    {
                        destptrw[block] = srcptrw[srcchannel];
                        srcchannel--;
                        if (srcchannel < 0)
                        {
                            srcchannel = mWaveFormat.channels - 1;
                        }
                    }

                    srcptrw -= mWaveFormat.channels;
                    destptrw -= mMaxChannels;
                }
            }
        }
        else if (blocksize == 4)
        {
            int * srcptrd = (int *)srcptr;
            int * destptrd = (int *)destptr;

            if (mWaveFormat.channels == 1)
            {
                for (count = 0; count < numblocks; count++)
                {
                    for (block = mMaxChannels - 1; block >= 0; block--)
                    {
                        destptrd[block] = *srcptrd;
                    }

                    srcptrd--;
                    destptrd -= mMaxChannels;
                }
            }
            else
            {
                for (count = 0; count < numblocks; count++)
                {
                    int srcchannel = mWaveFormat.channels - 1;

                    for (block = mMaxChannels - 1; block >= 0; block--)
                    {
                        destptrd[block] = srcptrd[srcchannel];
                        srcchannel--;
                        if (srcchannel < 0)
                        {
                            srcchannel = mWaveFormat.channels - 1;
                        }
                    }

                    srcptrd -= mWaveFormat.channels;
                    destptrd -= mMaxChannels;
                }
            }
        }
        else
        {
            for (count = 0; count < numblocks; count++)
            {
                int srcchannel = mWaveFormat.channels - 1;

                for (block = mMaxChannels - 1; block >= 0; block--)
                {
                    memcpy(destptr + block * blocksize, srcptr + srcchannel * blocksize, blocksize);
                    srcchannel--;
                    if (srcchannel < 0)
                    {
                        srcchannel = mWaveFormat.channels - 1;
                    }
                }

                srcptr -= blocksize * mWaveFormat.channels;
                destptr -= blocksize * mMaxChannels;
            }
        }

        *bytesread = bytesreadinternal * mMaxChannels / mWaveFormat.channels;
    }
    else
    {
        *bytesread = bytesreadinternal;
    }

    return result;
}

FMOD_RESULT CodecFSB::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;

    if (subsound < 0 || (numsubsounds && subsound >= numsubsounds))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFile->mSeekable)
    {
        unsigned int pos, srcmode;

        if (subsound != mCurrentIndex)
        {
            mCurrentIndex = subsound;
        }

        if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
        {
            srcmode = mFirstSample->mode;
        }
        else
        {
            srcmode = mShdr[mCurrentIndex]->mode;
        }

        if (postype == FMOD_TIMEUNIT_RAWBYTES)
        {
            pos = position + mDataOffset[mCurrentIndex];

            result = mFile->seek(pos, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
        else if ((srcmode & FSOUND_IMAADPCM) && waveformat[mCurrentIndex].format == FMOD_SOUND_FORMAT_PCM16)
        {
            unsigned int pcm, pcmaligned, excessbytes;

            pcm = position;
            pcmaligned = pcm & ~63;

            SoundI::getBytesFromSamples(pcmaligned, &pos, waveformat[mCurrentIndex].channels, FMOD_SOUND_FORMAT_IMAADPCM);
            pos += mDataOffset[mCurrentIndex];

            result = mFile->seek(pos, 0);
            if (result != FMOD_OK)
            {
                return result;
            }

            SoundI::getBytesFromSamples(pcm - pcmaligned, &excessbytes, waveformat[mCurrentIndex].channels, waveformat[mCurrentIndex].format);

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
        }
        else
        {
            result = SoundI::getBytesFromSamples(position, &pos, waveformat[mCurrentIndex].channels, waveformat[mCurrentIndex].format);
            if (result != FMOD_OK)
            {
                return result;
            }

            pos += mDataOffset[mCurrentIndex];

            result = mFile->seek(pos, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    return result;
}

FMOD_RESULT CodecFSB::soundcreateInternal(int subsound, FMOD_SOUND * sound)
{
    FMOD_RESULT result;
    SoundI * s = (SoundI *)sound;

    if (!(mMode & FMOD_CREATESTREAM))
    {
        mMaxChannels = waveformat[subsound].channels;
    }

    if (mHeader.version == FMOD_FSB_VERSION_3_1)
    {
        FMOD_FSB_SAMPLE_HEADER_3_1 * shdr31;
        FMOD_FSB_SAMPLE_HEADER_3_1 tmp;

        if (mHeader.mode & FMOD_FSB_SOURCE_BASICHEADERS)
        {
            shdr31 = (FMOD_FSB_SAMPLE_HEADER_3_1 *)mFirstSample;
        }
        else
        {
            shdr31 = (FMOD_FSB_SAMPLE_HEADER_3_1 *)mShdr[subsound];
        }

        memcpy(&tmp, shdr31, sizeof(FMOD_FSB_SAMPLE_HEADER_3_1));

        result = s->setVariations((float)tmp.varfreq, (float)tmp.varvol / 255.0f, (float)tmp.varpan / 255.0f);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = s->set3DMinMaxDistance(tmp.mindistance, tmp.maxdistance);
        if (result != FMOD_OK && result != FMOD_ERR_NEEDS3D)
        {
            return result;
        }
    }

    if (mNumSyncPoints && mSyncPoint && mNumSyncPoints[subsound])
    {
        int count;

        for (count = 0; count < mNumSyncPoints[subsound]; count++)
        {
            SyncPoint * point = &mSyncPoint[subsound][count];

            s->addSyncPoint(point->mOffset, FMOD_TIMEUNIT_PCM, point->mName, 0);
        }






























































































































        FMOD_Memory_Free(mSyncPoint[subsound]);
        mSyncPoint[subsound] = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecFSB::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecFSB * fsb = (CodecFSB *)codec;

    return fsb->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecFSB::closeCallback(FMOD_CODEC_STATE * codec)
{
    CodecFSB * fsb = (CodecFSB *)codec;

    return fsb->closeInternal();
}

FMOD_RESULT CodecFSB::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecFSB * fsb = (CodecFSB *)codec;

    return fsb->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecFSB::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecFSB * fsb = (CodecFSB *)codec;

    return fsb->setPositionInternal(subsound, position, postype);
}

FMOD_RESULT CodecFSB::soundcreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound)
{
    CodecFSB * fsb = (CodecFSB *)codec;

    return fsb->soundcreateInternal(subsound, sound);
}

} // namespace FMOD
