// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805EAD1C..0x805EB6EC (1 native function).
// Basename named by the parseChunk allocation/free file literal ("fmod_codec_wav_riff.cpp").
// The recursive RIFF chunk parser is shared: CodecWav::openInternal, and temporary CodecWav objects
// in CodecMPEG and CodecOggVorbis, call it. Little-endian fields are swapped by hand except
// the 4-byte unit reads, which File::read swaps itself.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod.h"
#include "fmod_codec_wav.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_syncpoint.h"
#include "fmod_types.h"



















namespace FMOD {

FMOD_RESULT CodecWav::parseChunk(unsigned int chunksize)
{
    unsigned int offset, fileoffset;
    FMOD_RESULT result;
    bool done = false;

    result = mFile->tell(&fileoffset);
    if (result != FMOD_OK)
    {
        return result;
    }

    offset = 4;
    fileoffset -= sizeof(WAVE_CHUNK);

    do
    {
        WAVE_CHUNK chunk;

        result = mFile->seek(fileoffset + sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            break;
        }

        result = mFile->read(&chunk, 1, sizeof(WAVE_CHUNK), 0);
        if (result != FMOD_OK)
        {
            break;
        }

        chunk.size = FMOD_SWAPENDIAN_DWORD(chunk.size);

        if (!FMOD_strncmp((const char *)chunk.id, "fmt ", 4))
        {
            mSrcFormat = (WAVE_FORMATEXTENSIBLE *)FMOD_Memory_Calloc(chunk.size < sizeof(WAVE_FORMATEXTENSIBLE) ? sizeof(WAVE_FORMATEXTENSIBLE) : chunk.size);
            if (!mSrcFormat)
            {
                return FMOD_ERR_MEMORY;
            }

            result = mFile->read(mSrcFormat, 1, chunk.size, 0);
            if (result != FMOD_OK)
            {
                break;
            }

            mSrcFormat->Format.wFormatTag = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Format.wFormatTag);
            mSrcFormat->Format.nChannels = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Format.nChannels);
            mSrcFormat->Format.nSamplesPerSec = FMOD_SWAPENDIAN_DWORD(mSrcFormat->Format.nSamplesPerSec);
            mSrcFormat->Format.nAvgBytesPerSec = FMOD_SWAPENDIAN_DWORD(mSrcFormat->Format.nAvgBytesPerSec);
            mSrcFormat->Format.nBlockAlign = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Format.nBlockAlign);
            mSrcFormat->Format.wBitsPerSample = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Format.wBitsPerSample);
            mSrcFormat->Format.cbSize = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Format.cbSize);
            mSrcFormat->Samples.wValidBitsPerSample = FMOD_SWAPENDIAN_WORD((unsigned short)mSrcFormat->Samples.wValidBitsPerSample);
            mSrcFormat->dwChannelMask = FMOD_SWAPENDIAN_DWORD(mSrcFormat->dwChannelMask);
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "cue ", 4))
        {
            result = mFile->read(&mNumSyncPoints, 4, 1, 0);

            if (mNumSyncPoints)
            {
                int count;








                if (mSyncPoint)
                {
                    FMOD_Memory_Free(mSyncPoint);
                }

                mSyncPoint = (SyncPoint *)FMOD_Memory_Alloc(mNumSyncPoints * sizeof(SyncPoint));
                if (!mSyncPoint)
                {
                    return FMOD_ERR_MEMORY;
                }

                for (count = 0; count < mNumSyncPoints; count++)
                {
                    WAVE_CUEPOINT cue;
                    SyncPoint * point = &mSyncPoint[count];

                    result = mFile->read(&cue, 1, sizeof(WAVE_CUEPOINT), 0);
                    if (result != FMOD_OK)
                    {
                        break;
                    }

                    cue.dwSampleOffset = FMOD_SWAPENDIAN_DWORD((unsigned int)cue.dwSampleOffset);

                    point->mOffset = cue.dwSampleOffset;
                    point->mRiffID = cue.dwIdentifier;
                }
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "fact", 4))
        {
            unsigned int fact;

            result = mFile->read(&fact, 1, 4, 0);
            if (result != FMOD_OK)
            {
                break;
            }

            mWaveFormat.lengthpcm = fact;
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "LIST", 4))
        {
            char listid[4];

            result = mFile->read(listid, 1, 4, 0);
            if (result != FMOD_OK)
            {
                break;
            }

            result = parseChunk(chunk.size);
            if (result != FMOD_OK)
            {
                break;
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "labl", 4))
        {
            if (mSyncPoint)
            {
                int id, count;

                result = mFile->read(&id, 4, 1, 0);
                if (result != FMOD_OK)
                {
                    break;
                }

                for (count = 0; count < mNumSyncPoints; count++)
                {
                    SyncPoint * p = &mSyncPoint[count];

                    if (p->mRiffID == id)
                    {
                        result = mFile->read(p->mName, 1, chunk.size - 4, 0);
                        break;
                    }
                }
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "smpl", 4))
        {
            WAVE_SMPLHEADER smpl;
            unsigned int rd;

            result = mFile->read(&smpl, 1, sizeof(WAVE_SMPLHEADER), &rd);
            if (result != FMOD_OK)
            {
                break;
            }

            if (rd == sizeof(WAVE_SMPLHEADER))
            {
                smpl.Manufacturer = FMOD_SWAPENDIAN_DWORD(smpl.Manufacturer);
                smpl.Product = FMOD_SWAPENDIAN_DWORD(smpl.Product);
                smpl.SamplePeriod = FMOD_SWAPENDIAN_DWORD(smpl.SamplePeriod);
                smpl.Note = FMOD_SWAPENDIAN_DWORD(smpl.Note);
                smpl.FineTune = FMOD_SWAPENDIAN_DWORD(smpl.FineTune);
                smpl.SMPTEFormat = FMOD_SWAPENDIAN_DWORD(smpl.SMPTEFormat);
                smpl.SMPTEOffset = FMOD_SWAPENDIAN_DWORD(smpl.SMPTEOffset);
                smpl.Loops = FMOD_SWAPENDIAN_DWORD(smpl.Loops);
                smpl.SamplerData = FMOD_SWAPENDIAN_DWORD(smpl.SamplerData);
                smpl.Loop.Identifier = FMOD_SWAPENDIAN_DWORD(smpl.Loop.Identifier);
                smpl.Loop.Type = FMOD_SWAPENDIAN_DWORD(smpl.Loop.Type);
                smpl.Loop.Start = FMOD_SWAPENDIAN_DWORD(smpl.Loop.Start);
                smpl.Loop.End = FMOD_SWAPENDIAN_DWORD(smpl.Loop.End);
                smpl.Loop.Fraction = FMOD_SWAPENDIAN_DWORD(smpl.Loop.Fraction);
                smpl.Loop.Count = FMOD_SWAPENDIAN_DWORD(smpl.Loop.Count);

                if (smpl.Loops)
                {
                    mLoopPoints[0] = smpl.Loop.Start;
                    mLoopPoints[1] = smpl.Loop.End;
                }
            }
        }
        else if (!FMOD_strncmp((const char *)chunk.id, "data", 4))
        {
            if (mSrcDataOffset == (unsigned int)-1)
            {
                mWaveFormat.lengthbytes = chunk.size;

                result = mFile->tell(&mSrcDataOffset);
                if (result != FMOD_OK)
                {
                    break;
                }
            }

            if (mFile->mSeekable)
            {
                result = mFile->seek(chunk.size, 1);
                if (result != FMOD_OK)
                {
                    break;
                }
            }
            else
            {
                done = true;
            }
        }
        else
        {
            mFile->seek(chunk.size, 1);
            if (result != FMOD_OK)
            {
                break;
            }
        }

        offset += chunk.size + sizeof(WAVE_CHUNK);
        fileoffset += chunk.size + sizeof(WAVE_CHUNK);

        if (chunk.size & 1)
        {
            offset++;
            fileoffset++;
        }

    } while (offset < chunksize && offset && !done);

    if (result == FMOD_ERR_FILE_EOF)
    {
        result = FMOD_OK;
    }

    return result;
}

} // namespace FMOD
