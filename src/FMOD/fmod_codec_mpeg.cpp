// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805D8160..0x805D975C (15 native functions; closeAll is dead-stripped).
// Original basename directly named by the allocation/free file strings ("fmod_codec_mpeg.cpp").
// The decoder state is embedded in CodecMPEG (see fmod_codec_mpeg.h); the open and seek scratch
// buffers are function statics (.bss 0x807416AC, 0x80741DAC, 0x80742FAC) instead of 4.06 stack arrays.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; CodecMPEG layout from G2MEAB accesses.

#include "fmod_codec_mpeg.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_codec_wav.h"
#include "fmod_file.h"
#include "fmod_globals.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_syncpoint.h"
#include "fmod_systemi.h"
#include "fmod_types.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#define WAVE_FORMAT_MPEG 0x0050
#define WAVE_FORMAT_MPEGLAYER3 0x0055

namespace FMOD {

int CodecMPEG::gIntWinBase[257] =
{
    0, -1, -1, -1, -1, -1, -1, -2, -2, -2,
    -2, -3, -3, -4, -4, -5, -5, -6, -7, -7,
    -8, -9, -10, -11, -13, -14, -16, -17, -19, -21,
    -24, -26, -29, -31, -35, -38, -41, -45, -49, -53,
    -58, -63, -68, -73, -79, -85, -91, -97, -104, -111,
    -117, -125, -132, -139, -147, -154, -161, -169, -176, -183,
    -190, -196, -202, -208, -213, -218, -222, -225, -227, -228,
    -228, -227, -224, -221, -215, -208, -200, -189, -177, -163,
    -146, -127, -106, -83, -57, -29, 2, 36, 72, 111,
    153, 197, 244, 294, 347, 401, 459, 519, 581, 645,
    711, 779, 848, 919, 991, 1064, 1137, 1210, 1283, 1356,
    1428, 1498, 1567, 1634, 1698, 1759, 1817, 1870, 1919, 1962,
    2001, 2032, 2057, 2075, 2085, 2087, 2080, 2063, 2037, 2000,
    1952, 1893, 1822, 1739, 1644, 1535, 1414, 1280, 1131, 970,
    794, 605, 402, 185, -45, -288, -545, -814, -1095, -1388,
    -1692, -2006, -2330, -2663, -3004, -3351, -3705, -4063, -4425, -4788,
    -5153, -5517, -5879, -6237, -6589, -6935, -7271, -7597, -7910, -8209,
    -8491, -8755, -8998, -9219, -9416, -9585, -9727, -9838, -9916, -9959,
    -9966, -9935, -9863, -9750, -9592, -9389, -9139, -8840, -8492, -8092,
    -7640, -7134, -6574, -5959, -5288, -4561, -3776, -2935, -2037, -1082,
    -70, 998, 2122, 3300, 4533, 5818, 7154, 8540, 9975, 11455,
    12980, 14548, 16155, 17799, 19478, 21189, 22929, 24694, 26482, 28289,
    30112, 31947, 33791, 35640, 37489, 39336, 41176, 43006, 44821, 46617,
    48390, 50137, 51853, 53534, 55178, 56778, 58333, 59838, 61289, 62684,
    64019, 65290, 66494, 67629, 68692, 69679, 70590, 71420, 72169, 72835,
    73415, 73908, 74313, 74630, 74856, 74992, 75038
};

bool CodecMPEG::gInitialized;
float CodecMPEG::gCos64[16];
float CodecMPEG::gCos32[8];
float CodecMPEG::gCos16[4];
float CodecMPEG::gCos8[2];
float CodecMPEG::gCos4[1];
float * CodecMPEG::gPnts[5] = { gCos64, gCos32, gCos16, gCos8, gCos4 };
float CodecMPEG::gDecWinMem[560];
} // namespace FMOD

float * FMOD_Mpeg_DecWin;
namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX mpegcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecMPEG::getDescriptionEx()
{
    memset(&mpegcodec, 0, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    mpegcodec.name = "FMOD MPEG Codec";
    mpegcodec.version = 0x00010100;
    mpegcodec.timeunits = FMOD_TIMEUNIT_PCM | FMOD_TIMEUNIT_RAWBYTES;
    mpegcodec.open = &CodecMPEG::openCallback;
    mpegcodec.close = &CodecMPEG::closeCallback;
    mpegcodec.read = &CodecMPEG::readCallback;
    mpegcodec.setposition = &CodecMPEG::setPositionCallback;
    mpegcodec.soundcreate = &CodecMPEG::soundCreateCallback;

    mpegcodec.mType = FMOD_SOUND_TYPE_MPEG;
    mpegcodec.mSize = sizeof(CodecMPEG);

    return &mpegcodec;
}

FMOD_RESULT CodecMPEG::makeTables(int scaleval)
{
    int i, j, k, kr, divv;
    float * table;
    float * costab;

    for (i = 0; i < 5; i++)
    {
        kr = 0x10 >> i;
        divv = 0x40 >> i;
        costab = gPnts[i];

        for (k = 0; k < kr; k++)
        {
            costab[k] = 1.0f / (2.0f * (float)cos(3.1415927f * ((float)k * 2.0f + 1.0f) / (float)divv));
        }
    }

    table = FMOD_Mpeg_DecWin = (float *)((unsigned int)(gDecWinMem + 15) & ~15);

    scaleval = -scaleval;
    for (i = 0, j = 0; i < 256; i++, j++, table += 32)
    {
        if (table < FMOD_Mpeg_DecWin + 512 + 16)
        {
            table[16] = table[0] = (float)scaleval * ((float)gIntWinBase[j] / 65536.0f);
        }
        if (i % 32 == 31)
        {
            table -= 1023;
        }
        if (i % 64 == 63)
        {
            scaleval = -scaleval;
        }
    }

    for (; i < 512; i++, j--, table += 32)
    {
        if (table < FMOD_Mpeg_DecWin + 512 + 16)
        {
            table[16] = table[0] = (float)scaleval * ((float)gIntWinBase[j] / 65536.0f);
        }
        if (i % 32 == 31)
        {
            table -= 1023;
        }
        if (i % 64 == 63)
        {
            scaleval = -scaleval;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::initAll()
{
    FMOD_RESULT result;

    FMOD_Mpeg_DecWin = (float *)((unsigned int)(gDecWinMem + 15) & ~15);

    result = makeTables(1);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = initLayer2();
    if (result != FMOD_OK)
    {
        return result;
    }

    result = initLayer3(32);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::closeAll()
{
    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::getPCMLength()
{
    FMOD_RESULT result;
    unsigned int frames = 0;
    unsigned int byteoffset = 0;
    unsigned int oldpos;

    result = mFile->tell(&oldpos);
    if (result != FMOD_OK)
    {
        return result;
    }

    mNumFrames = 0;
    mWaveFormat.lengthpcm = 0;

    while (byteoffset < mWaveFormat.lengthbytes)
    {
        unsigned int header;
        int framesize;

        result = mFile->read(&header, 1, 4, 0);
        if (result != FMOD_OK)
        {
            break;
        }

        result = decodeHeader(&header, 0, 0, &framesize);
        if (result == FMOD_OK && byteoffset + framesize < mWaveFormat.lengthbytes)
        {
            if (frames >= mNumFrames)
            {
                mNumFrames += 1000;
                mFrameOffset = (unsigned int *)FMOD_Memory_ReAlloc(mFrameOffset, mNumFrames * sizeof(unsigned int));
            }

            mFrameOffset[frames] = byteoffset;
            mWaveFormat.lengthpcm += mPCMFrameLengthBytes;

            byteoffset += framesize + 4;
            frames++;

            result = mFile->seek(framesize, SEEK_CUR);
            if (result != FMOD_OK)
            {
                break;
            }
        }
        else
        {
            mFile->seek(-3, SEEK_CUR);
        }
    }

    result = mFile->seek(oldpos, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    mNumFrames = frames;

    return result;
}

FMOD_RESULT CodecMPEG::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    FMOD_RESULT result;
    char header[4];
    unsigned int count;
    int framesize;
    bool validheader;
    bool manualsizecalc = false;

    init(FMOD_SOUND_TYPE_MPEG);

    memset(&mWaveFormat, 0, sizeof(FMOD_CODEC_WAVEFORMAT));

    numsubsounds = 0;
    waveformat = 0;
    mWaveFormat.lengthbytes = 0;
    mSrcDataOffset = 0;
    mFrameSizeOld = -1;
    mSynthBo = 1;
    mHasXingNumFrames = false;
    mHasXingToc = false;

    result = mFile->seek(0, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    /*
        Check for a RIFF wrapper around the MPEG data.
    */
    {
        CodecWav tempwav;
        WAVE_CHUNK chunk;

        memset(&tempwav, 0, sizeof(CodecWav));
        tempwav.mFile = mFile;
        tempwav.mSrcDataOffset = (unsigned int)-1;

        result = mFile->read(&chunk, 1, 8, 0);
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
                    if (tempwav.mSrcFormat->Format.wFormatTag == WAVE_FORMAT_MPEG || tempwav.mSrcFormat->Format.wFormatTag == WAVE_FORMAT_MPEGLAYER3)
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

    /*
        Search for the first valid frame, and confirm it by checking the frame that follows it.
    */
    do
    {
        unsigned int searchlen;

        validheader = false;

        searchlen = mWaveFormat.lengthbytes;
        if (searchlen > 4096 && !(usermode & FMOD_MPEGSEARCH))
        {
            searchlen = 4096;
        }

        for (count = 0; count < searchlen; count++)
        {
            result = mFile->seek(mSrcDataOffset, SEEK_SET);
            if (result != FMOD_OK && result != FMOD_ERR_FILE_COULDNOTSEEK)
            {
                return result;
            }

            if (result != FMOD_ERR_FILE_COULDNOTSEEK)
            {
                result = mFile->read(header, 1, 4, 0);
                if (result != FMOD_OK)
                {
                    return result;
                }

                mLayer = 0;

                result = decodeHeader(header, &mWaveFormat.frequency, &mWaveFormat.channels, &framesize);
                if (result == FMOD_OK)
                {
                    validheader = true;
                    break;
                }
            }

            mSrcDataOffset++;
        }

        if (!validheader)
        {
            return FMOD_ERR_FORMAT;
        }

        result = mFile->seek(framesize, SEEK_CUR);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = mFile->read(header, 1, 4, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = decodeHeader(header, &mWaveFormat.frequency, &mWaveFormat.channels, 0);
        if (result != FMOD_OK)
        {
            mSrcDataOffset++;
            validheader = false;
        }
    } while (!validheader);

    mFrameSizeOld = -1;
    mSynthBo = 1;
    mLayer = 0;

    framesize += 4;

    if (!gInitialized)
    {
        initAll();
        gInitialized = true;
    }

    result = mFile->seek(mSrcDataOffset, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (usermode & FMOD_CREATECOMPRESSEDSAMPLE)
    {
        mWaveFormat.format = FMOD_SOUND_FORMAT_MPEG;

        if (!(usermode & (FMOD_HARDWARE | FMOD_SOFTWARE)))
        {
            mWaveFormat.mode |= FMOD_SOFTWARE;
        }
    }
    else if (userexinfo && userexinfo->format == FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        mWaveFormat.format = FMOD_SOUND_FORMAT_PCMFLOAT;
    }
    else
    {
        mWaveFormat.format = FMOD_SOUND_FORMAT_PCM16;
    }

    /*
        Decode the first frame (skipping a Xing header frame) to find the PCM frame size.
    */
    {
        static unsigned char in[1792];
        static unsigned char out[4608];

        result = mFile->read(in, 1, framesize, 0);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (decodeXingHeader(in, mXingToc, &mNumFrames) == FMOD_OK)
        {
            mSrcDataOffset += framesize;

            result = mFile->read(in, 1, framesize, 0);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        decodeFrame(in, out, &mPCMFrameLengthBytes);
    }

    result = mFile->seek(mSrcDataOffset, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mPCMFrameLengthBytes)
    {
        mPCMFrameLengthBytes = 2304;
        if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCMFLOAT || mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG)
        {
            mPCMFrameLengthBytes *= 2;
        }
        mPCMFrameLengthBytes *= mWaveFormat.channels;
    }

    framesize++;
    framesize &= ~1;

    if (usermode & FMOD_ACCURATETIME && mFile->mSeekable)
    {
        mFile->seek(mSrcDataOffset, SEEK_SET);

        result = getPCMLength();
        if (result != FMOD_OK)
        {
            return result;
        }
    }
    else if (mWaveFormat.lengthbytes == (unsigned int)-1)
    {
        mWaveFormat.lengthpcm = (unsigned int)-1;
    }
    else if (mHasXingNumFrames)
    {
        mWaveFormat.lengthpcm = mNumFrames * 1152;
    }
    else
    {
        mWaveFormat.lengthpcm = mPCMFrameLengthBytes * ((mWaveFormat.lengthbytes + framesize - 1) / framesize + 1);
        mAccurateLength = false;
    }

    if (manualsizecalc && mWaveFormat.lengthbytes != (unsigned int)-1)
    {
        mWaveFormat.lengthbytes -= mSrcDataOffset;
    }

    if (mWaveFormat.lengthpcm != (unsigned int)-1 && (!mHasXingNumFrames || usermode & FMOD_ACCURATETIME))
    {
        mWaveFormat.lengthpcm = mWaveFormat.lengthpcm / 2 / mWaveFormat.channels;
        if (mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG || mWaveFormat.format == FMOD_SOUND_FORMAT_PCMFLOAT)
        {
            mWaveFormat.lengthpcm /= 2;
        }
    }

    mPCMBufferLength = 1152;
    mPCMBuffer = mPCMBufferMemory;
    mPCMBuffer = (unsigned char *)(((unsigned int)mPCMBuffer + 15) & ~15);

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG || mWaveFormat.format == FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        mPCMBufferLengthBytes = mWaveFormat.channels * 4608;
        mWaveFormat.blockalign = mWaveFormat.channels * 4608;
    }
    else
    {
        mWaveFormat.blockalign = mWaveFormat.channels * 2304;
        mPCMBufferLengthBytes = mWaveFormat.channels * 2304;
    }

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG && !mSystem->mDSPCodecPool_MPEG.mNumDSPCodecs)
    {
        result = mSystem->mDSPCodecPool_MPEG.init(this, mSystem->mAdvancedSettings.maxMPEGcodecs);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    numsubsounds = 0;
    waveformat = &mWaveFormat;

    resetFrame();

    return result;
}

FMOD_RESULT CodecMPEG::closeInternal()
{
    if (mFrameOffset)
    {
        FMOD_Memory_Free(mFrameOffset);
        mFrameOffset = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result = FMOD_OK;
    int retries = 0;
    int framesize = 0;
    unsigned char readbuffer[3072];
    unsigned char * decodebuffer;

    if (mPCMBuffer)
    {
        decodebuffer = mPCMBuffer;
    }
    else
    {
        decodebuffer = (unsigned char *)buffer;
    }

    do
    {
        result = mFile->read(readbuffer, 1, 4, 0);
        if (result != FMOD_OK)
        {
            break;
        }

        if (retries)
        {
            mLayer = 0;
        }

        result = decodeHeader(readbuffer, 0, 0, &framesize);
        if (result == FMOD_OK && retries)
        {
            unsigned int oldpos;
            unsigned int header;

            /*
                After a resync, check that the next frame header is valid too.
            */
            result = mFile->tell(&oldpos);
            if (result != FMOD_OK)
            {
                break;
            }

            mFile->seek(framesize, SEEK_CUR);
            if (result != FMOD_OK)
            {
                break;
            }

            mFile->read(&header, 1, 4, 0);
            if (result != FMOD_OK)
            {
                break;
            }

            header = FMOD_SWAPENDIAN_DWORD(header);
            if ((header & 0xFF) != 0xFF || ((header >> 8) & 0xE0) != 0xE0)
            {
                result = FMOD_ERR_FILE_BAD;
            }

            if (mFile->mSeekable)
            {
                mFile->seek(oldpos, SEEK_SET);
            }
        }

        if (result != FMOD_OK)
        {
            mFile->seek(-3, SEEK_CUR);
        }

        retries++;
    } while (result != FMOD_OK);

    if (result != FMOD_OK)
    {
        return result;
    }

    result = mFile->read(readbuffer + 4, 1, framesize, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = decodeFrame(readbuffer, decodebuffer, bytesread);
    if (result != FMOD_OK)
    {
        result = FMOD_OK;
    }

    return result;
}

FMOD_RESULT CodecMPEG::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int raw;
    unsigned int frame;
    unsigned int pcmbytes;
    unsigned int excessbytes;

    if (postype == FMOD_TIMEUNIT_RAWBYTES)
    {
        return mFile->seek(mSrcDataOffset + position, SEEK_SET);
    }

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG)
    {
        pcmbytes = mWaveFormat.channels * (position * 4);
    }
    else
    {
        pcmbytes = mWaveFormat.channels * (position * 2);
    }

    frame = pcmbytes / mPCMFrameLengthBytes;
    if (frame)
    {
        unsigned int framerewind = 9;

        excessbytes = pcmbytes - frame * mPCMFrameLengthBytes;

        if (frame < 9)
        {
            framerewind = frame;
        }

        frame -= framerewind;
        excessbytes += mPCMFrameLengthBytes * framerewind;
    }
    else
    {
        excessbytes = 0;
        position = 0;
    }

    if (mMode & FMOD_ACCURATETIME)
    {
        if (frame > mNumFrames)
        {
            frame = mNumFrames - 1;
        }

        raw = mFrameOffset[frame];
    }
    else if (mHasXingToc)
    {
        float percent;
        int index;
        float fa, fb, fx;

        percent = 100.0f * ((float)position / (float)mWaveFormat.lengthpcm);
        if (percent < 0.0f)
        {
            percent = 0.0f;
        }
        if (percent > 100.0f)
        {
            percent = 100.0f;
        }

        index = (int)percent;
        if (index > 99)
        {
            index = 99;
        }

        fa = mXingToc[index];
        if (index < 99)
        {
            fb = mXingToc[index + 1];
        }
        else
        {
            fb = 256.0f;
        }

        fx = fa + (fb - fa) * (percent - index);

        raw = (unsigned int)((1.0f / 256.0f) * fx * (float)mWaveFormat.lengthbytes);
        if (index > 0)
        {
            raw += mPCMFrameLengthBytes;
        }
    }
    else
    {
        raw = (unsigned int)((FMOD_UINT64)position * mWaveFormat.lengthbytes / mWaveFormat.lengthpcm);
    }

    raw += mSrcDataOffset;
    if (raw > mSrcDataOffset + mWaveFormat.lengthbytes)
    {
        raw = mSrcDataOffset;
    }

    result = mFile->seek(raw, SEEK_SET);
    if (result != FMOD_OK)
    {
        return result;
    }

    memset(mBSSpace, 0, sizeof(mBSSpace));
    memset(mSynthBuffs, 0, sizeof(mSynthBuffs));
    memset(mBlock, 0, sizeof(mBlock));
    mFrameSizeOld = -1;

    /*
        Decode the rewound frames to prime the synthesis and overlap buffers.
    */
    while (excessbytes)
    {
        static char buff[4608];
        unsigned int read = 0;

        result = Codec::read(buff, excessbytes > sizeof(buff) ? sizeof(buff) : excessbytes, &read);
        if (result != FMOD_OK)
        {
            break;
        }

        if (read > excessbytes)
        {
            excessbytes = 0;
        }
        else
        {
            excessbytes -= read;
        }
    }

    return result;
}

FMOD_RESULT CodecMPEG::soundCreateInternal(int subsound, FMOD_SOUND * sound)
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

FMOD_RESULT CodecMPEG::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
    CodecMPEG * mpeg = (CodecMPEG *)codec;

    return mpeg->openInternal(usermode, userexinfo);
}

FMOD_RESULT CodecMPEG::closeCallback(FMOD_CODEC_STATE * codec)
{
    CodecMPEG * mpeg = (CodecMPEG *)codec;

    return mpeg->closeInternal();
}

FMOD_RESULT CodecMPEG::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    CodecMPEG * mpeg = (CodecMPEG *)codec;

    return mpeg->readInternal(buffer, sizebytes, bytesread);
}

FMOD_RESULT CodecMPEG::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    CodecMPEG * mpeg = (CodecMPEG *)codec;

    return mpeg->setPositionInternal(subsound, position, postype);
}

FMOD_RESULT CodecMPEG::soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound)
{
    CodecMPEG * mpeg = (CodecMPEG *)codec;

    return mpeg->soundCreateInternal(subsound, sound);
}

} // namespace FMOD
