// NonMatching: complete reconstruction of the G2MEAB MPEG frame decoder and polyphase synthesis
// (0x805D975C..0x805DB060, 11 native functions). The unit merges the 4.06 fmod_mpeg_common and
// fmod_mpeg_synthesis code; the algorithms are the mpglib bit reader, header parser, DCT64 and windowed
// synthesis.

#include "fmod.h"
#include "fmod_codec_mpeg.h"
#include "fmod_string.h"

#include <string.h>

namespace FMOD {

int CodecMPEG::gTabSel123[2][3][16] =
{
    {
        { 0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 },
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0 },
        { 0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0 }
    },
    {
        { 0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0 },
        { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 },
        { 0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0 }
    }
};

int CodecMPEG::gFreqs[9] = { 44100, 48000, 32000, 22050, 24000, 16000, 11025, 12000, 8000 };

unsigned int CodecMPEG::getBits(int number_of_bits)
{
    unsigned int rval;

    if (!number_of_bits)
    {
        return 0;
    }

    rval = mWordPointer[0];
    rval <<= 8;
    rval |= mWordPointer[1];
    rval <<= 8;
    rval |= mWordPointer[2];
    rval <<= mBitIndex;
    rval &= 0xFFFFFF;

    mBitIndex += number_of_bits;

    rval >>= (24 - number_of_bits);

    mWordPointer += (mBitIndex >> 3);
    mBitIndex &= 7;

    return rval;
}

unsigned int CodecMPEG::getBitsFast(int number_of_bits)
{
    unsigned int rval;

    rval = mWordPointer[0];
    rval <<= 8;
    rval |= mWordPointer[1];
    rval <<= mBitIndex;
    rval &= 0xFFFF;

    mBitIndex += number_of_bits;

    rval >>= (16 - number_of_bits);

    mWordPointer += (mBitIndex >> 3);
    mBitIndex &= 7;

    return rval;
}

unsigned int CodecMPEG::get1Bit()
{
    unsigned char rval;

    rval = *mWordPointer << mBitIndex;

    mBitIndex++;
    mWordPointer += (mBitIndex >> 3);
    mBitIndex &= 7;

    return rval >> 7;
}

FMOD_RESULT CodecMPEG::decodeHeader(void * in, int * samplerate, int * channels, int * framesize)
{
    unsigned char * buff = (unsigned char *)in;
    unsigned int head;

    head = buff[0];
    head <<= 8;
    head |= buff[1];
    head <<= 8;
    head |= buff[2];
    head <<= 8;
    head |= buff[3];

    mFrameHeader = head;

    if ((head >> 24) != 0xFF)
    {
        return FMOD_ERR_FORMAT;
    }
    if (((head >> 16) & 0xE0) != 0xE0)
    {
        return FMOD_ERR_FORMAT;
    }

    if (head & (1 << 20))
    {
        mFrame.lsf = (head & (1 << 19)) ? 0 : 1;
        mFrame.mpeg25 = 0;
    }
    else
    {
        mFrame.lsf = 1;
        mFrame.mpeg25 = 1;
    }

    mFrame.lay = 4 - ((head >> 17) & 3);
    if (mFrame.lay != 3 && mFrame.lay != 2)
    {
        return FMOD_ERR_FORMAT;
    }

    if (!mLayer)
    {
        mLayer = mFrame.lay;
    }
    if (mFrame.lay != mLayer)
    {
        return FMOD_ERR_FORMAT;
    }

    if (((head >> 10) & 3) == 3)
    {
        return FMOD_ERR_FORMAT;
    }

    if (mFrame.mpeg25)
    {
        mFrame.sampling_frequency = 6 + ((head >> 10) & 3);
    }
    else
    {
        mFrame.sampling_frequency = ((head >> 10) & 3) + (mFrame.lsf * 3);
    }

    if (samplerate)
    {
        *samplerate = gFreqs[mFrame.sampling_frequency];
    }
    else if (mWaveFormat.frequency != gFreqs[mFrame.sampling_frequency])
    {
        return FMOD_ERR_FORMAT;
    }

    mFrame.error_protection = ((head >> 16) & 1) ^ 1;
    mFrame.bitrate_index = ((head >> 12) & 0xF);
    mFrame.padding = ((head >> 9) & 1);
    mFrame.extension = ((head >> 8) & 1);
    mFrame.mode = ((head >> 6) & 3);
    mFrame.mode_ext = ((head >> 4) & 3);
    mFrame.copyright = ((head >> 3) & 1);
    mFrame.original = ((head >> 2) & 1);
    mFrame.emphasis = head & 3;

    mFrame.stereo = (mFrame.mode == 3) ? 1 : 2;

    if (channels)
    {
        *channels = mFrame.stereo;
    }
    else if (mFrame.stereo != mWaveFormat.channels)
    {
        return FMOD_ERR_FORMAT;
    }

    if (!mFrame.bitrate_index)
    {
        return FMOD_ERR_FORMAT;
    }

    switch (mFrame.lay)
    {
        case 2:
        {
            getIIStuff();
            mFrame.jsbound = (mFrame.mode == 1) ? (mFrame.mode_ext << 2) + 4 : mFrame.II_sblimit;
            mFrame.framesize = gTabSel123[mFrame.lsf][1][mFrame.bitrate_index] * 144000;
            mFrame.framesize /= gFreqs[mFrame.sampling_frequency];
            mFrame.framesize += mFrame.padding - 4;
            break;
        }
        case 3:
        {
            mFrame.framesize = gTabSel123[mFrame.lsf][2][mFrame.bitrate_index] * 144000;
            mFrame.framesize /= gFreqs[mFrame.sampling_frequency] << mFrame.lsf;
            mFrame.framesize = mFrame.framesize + mFrame.padding - 4;
            break;
        }
        default:
        {
            return FMOD_ERR_FORMAT;
        }
    }

    if (mFrame.framesize < 16)
    {
        return FMOD_ERR_FORMAT;
    }

    mFrameSize = mFrame.framesize;

    if (framesize)
    {
        *framesize = mFrame.framesize;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::decodeXingHeader(unsigned char * in, unsigned char * toc, unsigned int * numframes)
{
    int i, flags;
    int id, mode;

    id = (in[1] >> 3) & 1;
    mode = (in[3] >> 6) & 3;

    if (id)
    {
        if (mode != 3)
        {
            in += 32 + 4;
        }
        else
        {
            in += 17 + 4;
        }
    }
    else
    {
        if (mode != 3)
        {
            in += 17 + 4;
        }
        else
        {
            in += 9 + 4;
        }
    }

    if (FMOD_strncmp((char *)in, "Xing", 4))
    {
        return FMOD_ERR_FORMAT;
    }

    flags = in[4];
    flags <<= 8;
    flags |= in[5];
    flags <<= 8;
    flags |= in[6];
    flags <<= 8;
    flags |= in[7];
    in += 8;

    if (flags & 1)
    {
        if (numframes)
        {
            *numframes = in[0];
            *numframes <<= 8;
            *numframes |= in[1];
            *numframes <<= 8;
            *numframes |= in[2];
            *numframes <<= 8;
            *numframes |= in[3];
            in += 4;
        }
        mHasXingNumFrames = true;
    }

    if (flags & 4)
    {
        if (toc)
        {
            for (i = 0; i < 100; i++)
            {
                toc[i] = *in++;
            }
        }
        mHasXingToc = true;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::decodeFrame(unsigned char * in, void * out, unsigned int * outlen)
{
    FMOD_RESULT result = FMOD_OK;

    if (!mFrameSize)
    {
        result = decodeHeader(in, 0, 0, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mWordPointer = mBSSpace[mBSNum] + 512;
    mBSNum = (mBSNum + 1) & 1;
    mBitIndex = 0;

    memcpy(mWordPointer, in + 4, mFrameSize);

    if (mFrame.error_protection)
    {
        getBits(16);
    }

    switch (mFrame.lay)
    {
        case 2:
        {
            result = decodeLayer2(out, outlen);
            break;
        }
        case 3:
        {
            result = decodeLayer3(out, outlen);
            break;
        }
    }

    mFrameSizeOld = mFrameSize;
    mFrameSize = 0;

    return result;
}

FMOD_RESULT CodecMPEG::resetFrame()
{
    memset(mBSSpace, 0, sizeof(mBSSpace));
    memset(mSynthBuffs, 0, sizeof(mSynthBuffs));
    memset(mBlock, 0, sizeof(mBlock));

    mFrameSizeOld = -1;

    return FMOD_OK;
}

void CodecMPEG::dct64(float * out0, float * out1, float * samples)
{
    float b1[32], b2[32];

    {
        float * costab = gPnts[0];

        b1[0x00] = samples[0x00] + samples[0x1F];
        b1[0x1F] = (samples[0x00] - samples[0x1F]) * costab[0x00];
        b1[0x01] = samples[0x01] + samples[0x1E];
        b1[0x1E] = (samples[0x01] - samples[0x1E]) * costab[0x01];
        b1[0x02] = samples[0x02] + samples[0x1D];
        b1[0x1D] = (samples[0x02] - samples[0x1D]) * costab[0x02];
        b1[0x03] = samples[0x03] + samples[0x1C];
        b1[0x1C] = (samples[0x03] - samples[0x1C]) * costab[0x03];
        b1[0x04] = samples[0x04] + samples[0x1B];
        b1[0x1B] = (samples[0x04] - samples[0x1B]) * costab[0x04];
        b1[0x05] = samples[0x05] + samples[0x1A];
        b1[0x1A] = (samples[0x05] - samples[0x1A]) * costab[0x05];
        b1[0x06] = samples[0x06] + samples[0x19];
        b1[0x19] = (samples[0x06] - samples[0x19]) * costab[0x06];
        b1[0x07] = samples[0x07] + samples[0x18];
        b1[0x18] = (samples[0x07] - samples[0x18]) * costab[0x07];
        b1[0x08] = samples[0x08] + samples[0x17];
        b1[0x17] = (samples[0x08] - samples[0x17]) * costab[0x08];
        b1[0x09] = samples[0x09] + samples[0x16];
        b1[0x16] = (samples[0x09] - samples[0x16]) * costab[0x09];
        b1[0x0A] = samples[0x0A] + samples[0x15];
        b1[0x15] = (samples[0x0A] - samples[0x15]) * costab[0x0A];
        b1[0x0B] = samples[0x0B] + samples[0x14];
        b1[0x14] = (samples[0x0B] - samples[0x14]) * costab[0x0B];
        b1[0x0C] = samples[0x0C] + samples[0x13];
        b1[0x13] = (samples[0x0C] - samples[0x13]) * costab[0x0C];
        b1[0x0D] = samples[0x0D] + samples[0x12];
        b1[0x12] = (samples[0x0D] - samples[0x12]) * costab[0x0D];
        b1[0x0E] = samples[0x0E] + samples[0x11];
        b1[0x11] = (samples[0x0E] - samples[0x11]) * costab[0x0E];
        b1[0x0F] = samples[0x0F] + samples[0x10];
        b1[0x10] = (samples[0x0F] - samples[0x10]) * costab[0x0F];
    }

    {
        float * costab = gPnts[1];

        b2[0x00] = b1[0x00] + b1[0x0F];
        b2[0x0F] = (b1[0x00] - b1[0x0F]) * costab[0x00];
        b2[0x01] = b1[0x01] + b1[0x0E];
        b2[0x0E] = (b1[0x01] - b1[0x0E]) * costab[0x01];
        b2[0x02] = b1[0x02] + b1[0x0D];
        b2[0x0D] = (b1[0x02] - b1[0x0D]) * costab[0x02];
        b2[0x03] = b1[0x03] + b1[0x0C];
        b2[0x0C] = (b1[0x03] - b1[0x0C]) * costab[0x03];
        b2[0x04] = b1[0x04] + b1[0x0B];
        b2[0x0B] = (b1[0x04] - b1[0x0B]) * costab[0x04];
        b2[0x05] = b1[0x05] + b1[0x0A];
        b2[0x0A] = (b1[0x05] - b1[0x0A]) * costab[0x05];
        b2[0x06] = b1[0x06] + b1[0x09];
        b2[0x09] = (b1[0x06] - b1[0x09]) * costab[0x06];
        b2[0x07] = b1[0x07] + b1[0x08];
        b2[0x08] = (b1[0x07] - b1[0x08]) * costab[0x07];
        b2[0x10] = b1[0x10] + b1[0x1F];
        b2[0x1F] = (b1[0x1F] - b1[0x10]) * costab[0x00];
        b2[0x11] = b1[0x11] + b1[0x1E];
        b2[0x1E] = (b1[0x1E] - b1[0x11]) * costab[0x01];
        b2[0x12] = b1[0x12] + b1[0x1D];
        b2[0x1D] = (b1[0x1D] - b1[0x12]) * costab[0x02];
        b2[0x13] = b1[0x13] + b1[0x1C];
        b2[0x1C] = (b1[0x1C] - b1[0x13]) * costab[0x03];
        b2[0x14] = b1[0x14] + b1[0x1B];
        b2[0x1B] = (b1[0x1B] - b1[0x14]) * costab[0x04];
        b2[0x15] = b1[0x15] + b1[0x1A];
        b2[0x1A] = (b1[0x1A] - b1[0x15]) * costab[0x05];
        b2[0x16] = b1[0x16] + b1[0x19];
        b2[0x19] = (b1[0x19] - b1[0x16]) * costab[0x06];
        b2[0x17] = b1[0x17] + b1[0x18];
        b2[0x18] = (b1[0x18] - b1[0x17]) * costab[0x07];
    }

    {
        float * costab = gPnts[2];

        b1[0x00] = b2[0x00] + b2[0x07];
        b1[0x07] = (b2[0x00] - b2[0x07]) * costab[0x00];
        b1[0x01] = b2[0x01] + b2[0x06];
        b1[0x06] = (b2[0x01] - b2[0x06]) * costab[0x01];
        b1[0x02] = b2[0x02] + b2[0x05];
        b1[0x05] = (b2[0x02] - b2[0x05]) * costab[0x02];
        b1[0x03] = b2[0x03] + b2[0x04];
        b1[0x04] = (b2[0x03] - b2[0x04]) * costab[0x03];

        b1[0x08] = b2[0x08] + b2[0x0F];
        b1[0x0F] = (b2[0x0F] - b2[0x08]) * costab[0x00];
        b1[0x09] = b2[0x09] + b2[0x0E];
        b1[0x0E] = (b2[0x0E] - b2[0x09]) * costab[0x01];
        b1[0x0A] = b2[0x0A] + b2[0x0D];
        b1[0x0D] = (b2[0x0D] - b2[0x0A]) * costab[0x02];
        b1[0x0B] = b2[0x0B] + b2[0x0C];
        b1[0x0C] = (b2[0x0C] - b2[0x0B]) * costab[0x03];

        b1[0x10] = b2[0x10] + b2[0x17];
        b1[0x17] = (b2[0x10] - b2[0x17]) * costab[0x00];
        b1[0x11] = b2[0x11] + b2[0x16];
        b1[0x16] = (b2[0x11] - b2[0x16]) * costab[0x01];
        b1[0x12] = b2[0x12] + b2[0x15];
        b1[0x15] = (b2[0x12] - b2[0x15]) * costab[0x02];
        b1[0x13] = b2[0x13] + b2[0x14];
        b1[0x14] = (b2[0x13] - b2[0x14]) * costab[0x03];

        b1[0x18] = b2[0x18] + b2[0x1F];
        b1[0x1F] = (b2[0x1F] - b2[0x18]) * costab[0x00];
        b1[0x19] = b2[0x19] + b2[0x1E];
        b1[0x1E] = (b2[0x1E] - b2[0x19]) * costab[0x01];
        b1[0x1A] = b2[0x1A] + b2[0x1D];
        b1[0x1D] = (b2[0x1D] - b2[0x1A]) * costab[0x02];
        b1[0x1B] = b2[0x1B] + b2[0x1C];
        b1[0x1C] = (b2[0x1C] - b2[0x1B]) * costab[0x03];
    }

    {
        const float cos0 = gPnts[3][0];
        const float cos1 = gPnts[3][1];

        b2[0x00] = b1[0x00] + b1[0x03];
        b2[0x03] = (b1[0x00] - b1[0x03]) * cos0;
        b2[0x01] = b1[0x01] + b1[0x02];
        b2[0x02] = (b1[0x01] - b1[0x02]) * cos1;

        b2[0x04] = b1[0x04] + b1[0x07];
        b2[0x07] = (b1[0x07] - b1[0x04]) * cos0;
        b2[0x05] = b1[0x05] + b1[0x06];
        b2[0x06] = (b1[0x06] - b1[0x05]) * cos1;

        b2[0x08] = b1[0x08] + b1[0x0B];
        b2[0x0B] = (b1[0x08] - b1[0x0B]) * cos0;
        b2[0x09] = b1[0x09] + b1[0x0A];
        b2[0x0A] = (b1[0x09] - b1[0x0A]) * cos1;

        b2[0x0C] = b1[0x0C] + b1[0x0F];
        b2[0x0F] = (b1[0x0F] - b1[0x0C]) * cos0;
        b2[0x0D] = b1[0x0D] + b1[0x0E];
        b2[0x0E] = (b1[0x0E] - b1[0x0D]) * cos1;

        b2[0x10] = b1[0x10] + b1[0x13];
        b2[0x13] = (b1[0x10] - b1[0x13]) * cos0;
        b2[0x11] = b1[0x11] + b1[0x12];
        b2[0x12] = (b1[0x11] - b1[0x12]) * cos1;

        b2[0x14] = b1[0x14] + b1[0x17];
        b2[0x17] = (b1[0x17] - b1[0x14]) * cos0;
        b2[0x15] = b1[0x15] + b1[0x16];
        b2[0x16] = (b1[0x16] - b1[0x15]) * cos1;

        b2[0x18] = b1[0x18] + b1[0x1B];
        b2[0x1B] = (b1[0x18] - b1[0x1B]) * cos0;
        b2[0x19] = b1[0x19] + b1[0x1A];
        b2[0x1A] = (b1[0x19] - b1[0x1A]) * cos1;

        b2[0x1C] = b1[0x1C] + b1[0x1F];
        b2[0x1F] = (b1[0x1F] - b1[0x1C]) * cos0;
        b2[0x1D] = b1[0x1D] + b1[0x1E];
        b2[0x1E] = (b1[0x1E] - b1[0x1D]) * cos1;
    }

    {
        const float cos0 = gPnts[4][0];

        b1[0x00] = b2[0x00] + b2[0x01];
        b1[0x01] = (b2[0x00] - b2[0x01]) * cos0;
        b1[0x02] = b2[0x02] + b2[0x03];
        b1[0x03] = (b2[0x03] - b2[0x02]) * cos0;
        b1[0x02] += b1[0x03];

        b1[0x04] = b2[0x04] + b2[0x05];
        b1[0x05] = (b2[0x04] - b2[0x05]) * cos0;
        b1[0x06] = b2[0x06] + b2[0x07];
        b1[0x07] = (b2[0x07] - b2[0x06]) * cos0;
        b1[0x06] += b1[0x07];
        b1[0x04] += b1[0x06];
        b1[0x06] += b1[0x05];
        b1[0x05] += b1[0x07];

        b1[0x08] = b2[0x08] + b2[0x09];
        b1[0x09] = (b2[0x08] - b2[0x09]) * cos0;
        b1[0x0A] = b2[0x0A] + b2[0x0B];
        b1[0x0B] = (b2[0x0B] - b2[0x0A]) * cos0;
        b1[0x0A] += b1[0x0B];

        b1[0x0C] = b2[0x0C] + b2[0x0D];
        b1[0x0D] = (b2[0x0C] - b2[0x0D]) * cos0;
        b1[0x0E] = b2[0x0E] + b2[0x0F];
        b1[0x0F] = (b2[0x0F] - b2[0x0E]) * cos0;
        b1[0x0E] += b1[0x0F];
        b1[0x0C] += b1[0x0E];
        b1[0x0E] += b1[0x0D];
        b1[0x0D] += b1[0x0F];

        b1[0x10] = b2[0x10] + b2[0x11];
        b1[0x11] = (b2[0x10] - b2[0x11]) * cos0;
        b1[0x12] = b2[0x12] + b2[0x13];
        b1[0x13] = (b2[0x13] - b2[0x12]) * cos0;
        b1[0x12] += b1[0x13];

        b1[0x14] = b2[0x14] + b2[0x15];
        b1[0x15] = (b2[0x14] - b2[0x15]) * cos0;
        b1[0x16] = b2[0x16] + b2[0x17];
        b1[0x17] = (b2[0x17] - b2[0x16]) * cos0;
        b1[0x16] += b1[0x17];
        b1[0x14] += b1[0x16];
        b1[0x16] += b1[0x15];
        b1[0x15] += b1[0x17];

        b1[0x18] = b2[0x18] + b2[0x19];
        b1[0x19] = (b2[0x18] - b2[0x19]) * cos0;
        b1[0x1A] = b2[0x1A] + b2[0x1B];
        b1[0x1B] = (b2[0x1B] - b2[0x1A]) * cos0;
        b1[0x1A] += b1[0x1B];

        b1[0x1C] = b2[0x1C] + b2[0x1D];
        b1[0x1D] = (b2[0x1C] - b2[0x1D]) * cos0;
        b1[0x1E] = b2[0x1E] + b2[0x1F];
        b1[0x1F] = (b2[0x1F] - b2[0x1E]) * cos0;
        b1[0x1E] += b1[0x1F];
        b1[0x1C] += b1[0x1E];
        b1[0x1E] += b1[0x1D];
        b1[0x1D] += b1[0x1F];
    }

    out0[0x10 * 16] = b1[0x00];
    out0[0x10 * 12] = b1[0x04];
    out0[0x10 * 8] = b1[0x02];
    out0[0x10 * 4] = b1[0x06];
    out0[0x10 * 0] = b1[0x01];
    out1[0x10 * 0] = b1[0x01];
    out1[0x10 * 4] = b1[0x05];
    out1[0x10 * 8] = b1[0x03];
    out1[0x10 * 12] = b1[0x07];

    b1[0x08] += b1[0x0C];
    out0[0x10 * 14] = b1[0x08];
    b1[0x0C] += b1[0x0A];
    out0[0x10 * 10] = b1[0x0C];
    b1[0x0A] += b1[0x0E];
    out0[0x10 * 6] = b1[0x0A];
    b1[0x0E] += b1[0x09];
    out0[0x10 * 2] = b1[0x0E];
    b1[0x09] += b1[0x0D];
    out1[0x10 * 2] = b1[0x09];
    b1[0x0D] += b1[0x0B];
    out1[0x10 * 6] = b1[0x0D];
    b1[0x0B] += b1[0x0F];
    out1[0x10 * 10] = b1[0x0B];
    out1[0x10 * 14] = b1[0x0F];

    b1[0x18] += b1[0x1C];
    out0[0x10 * 15] = b1[0x10] + b1[0x18];
    out0[0x10 * 13] = b1[0x18] + b1[0x14];
    b1[0x1C] += b1[0x1A];
    out0[0x10 * 11] = b1[0x14] + b1[0x1C];
    out0[0x10 * 9] = b1[0x1C] + b1[0x12];
    b1[0x1A] += b1[0x1E];
    out0[0x10 * 7] = b1[0x12] + b1[0x1A];
    out0[0x10 * 5] = b1[0x1A] + b1[0x16];
    b1[0x1E] += b1[0x19];
    out0[0x10 * 3] = b1[0x16] + b1[0x1E];
    out0[0x10 * 1] = b1[0x1E] + b1[0x11];
    b1[0x19] += b1[0x1D];
    out1[0x10 * 1] = b1[0x11] + b1[0x19];
    out1[0x10 * 3] = b1[0x19] + b1[0x15];
    b1[0x1D] += b1[0x1B];
    out1[0x10 * 5] = b1[0x15] + b1[0x1D];
    out1[0x10 * 7] = b1[0x1D] + b1[0x13];
    b1[0x1B] += b1[0x1F];
    out1[0x10 * 9] = b1[0x13] + b1[0x1B];
    out1[0x10 * 11] = b1[0x1B] + b1[0x17];
    out1[0x10 * 13] = b1[0x17] + b1[0x1F];
    out1[0x10 * 15] = b1[0x1F];
}

FMOD_RESULT CodecMPEG::synthC(float * b0, int bo1, int channels, short * samples)
{
    int j;
    float * window = FMOD_Mpeg_DecWin + 16 - bo1;

    for (j = 16; j; j--, b0 += 0x10, window += 0x20, samples += channels)
    {
        float sum;

        sum = window[0x0] * b0[0x0];
        sum -= window[0x1] * b0[0x1];
        sum += window[0x2] * b0[0x2];
        sum -= window[0x3] * b0[0x3];
        sum += window[0x4] * b0[0x4];
        sum -= window[0x5] * b0[0x5];
        sum += window[0x6] * b0[0x6];
        sum -= window[0x7] * b0[0x7];
        sum += window[0x8] * b0[0x8];
        sum -= window[0x9] * b0[0x9];
        sum += window[0xA] * b0[0xA];
        sum -= window[0xB] * b0[0xB];
        sum += window[0xC] * b0[0xC];
        sum -= window[0xD] * b0[0xD];
        sum += window[0xE] * b0[0xE];
        sum -= window[0xF] * b0[0xF];
        sum *= 32767.0f;

        if (sum > 32767.0)
        {
            *samples = 0x7FFF;
        }
        else if (sum < -32768.0)
        {
            *samples = -0x8000;
        }
        else
        {
            *samples = (short)sum;
        }
    }

    {
        float sum;

        sum = window[0x0] * b0[0x0];
        sum += window[0x2] * b0[0x2];
        sum += window[0x4] * b0[0x4];
        sum += window[0x6] * b0[0x6];
        sum += window[0x8] * b0[0x8];
        sum += window[0xA] * b0[0xA];
        sum += window[0xC] * b0[0xC];
        sum += window[0xE] * b0[0xE];
        sum *= 32767.0f;

        if (sum > 32767.0)
        {
            *samples = 0x7FFF;
        }
        else if (sum < -32768.0)
        {
            *samples = -0x8000;
        }
        else
        {
            *samples = (short)sum;
        }

        b0 -= 0x10;
        window -= 0x20;
        samples += channels;
    }
    window += bo1 << 1;

    for (j = 15; j; j--, b0 -= 0x10, window -= 0x20, samples += channels)
    {
        float sum;

        sum = -window[-0x1] * b0[0x0];
        sum -= window[-0x2] * b0[0x1];
        sum -= window[-0x3] * b0[0x2];
        sum -= window[-0x4] * b0[0x3];
        sum -= window[-0x5] * b0[0x4];
        sum -= window[-0x6] * b0[0x5];
        sum -= window[-0x7] * b0[0x6];
        sum -= window[-0x8] * b0[0x7];
        sum -= window[-0x9] * b0[0x8];
        sum -= window[-0xA] * b0[0x9];
        sum -= window[-0xB] * b0[0xA];
        sum -= window[-0xC] * b0[0xB];
        sum -= window[-0xD] * b0[0xC];
        sum -= window[-0xE] * b0[0xD];
        sum -= window[-0xF] * b0[0xE];
        sum -= window[0x0] * b0[0xF];
        sum *= 32767.0f;

        if (sum > 32767.0)
        {
            *samples = 0x7FFF;
        }
        else if (sum < -32768.0)
        {
            *samples = -0x8000;
        }
        else
        {
            *samples = (short)sum;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::synthFloat(float * b0, int bo1, int channels, float * samples)
{
    int j;
    float * window = FMOD_Mpeg_DecWin + 16 - bo1;

    for (j = 16; j; j--, b0 += 0x10, window += 0x20, samples += channels)
    {
        float sum;

        sum = window[0x0] * b0[0x0];
        sum -= window[0x1] * b0[0x1];
        sum += window[0x2] * b0[0x2];
        sum -= window[0x3] * b0[0x3];
        sum += window[0x4] * b0[0x4];
        sum -= window[0x5] * b0[0x5];
        sum += window[0x6] * b0[0x6];
        sum -= window[0x7] * b0[0x7];
        sum += window[0x8] * b0[0x8];
        sum -= window[0x9] * b0[0x9];
        sum += window[0xA] * b0[0xA];
        sum -= window[0xB] * b0[0xB];
        sum += window[0xC] * b0[0xC];
        sum -= window[0xD] * b0[0xD];
        sum += window[0xE] * b0[0xE];
        sum -= window[0xF] * b0[0xF];
        *samples = sum;
    }

    {
        float sum;

        sum = window[0x0] * b0[0x0];
        sum += window[0x2] * b0[0x2];
        sum += window[0x4] * b0[0x4];
        sum += window[0x6] * b0[0x6];
        sum += window[0x8] * b0[0x8];
        sum += window[0xA] * b0[0xA];
        sum += window[0xC] * b0[0xC];
        sum += window[0xE] * b0[0xE];
        *samples = sum;

        b0 -= 0x10;
        window -= 0x20;
        samples += channels;
    }
    window += bo1 << 1;

    for (j = 15; j; j--, b0 -= 0x10, window -= 0x20, samples += channels)
    {
        float sum;

        sum = -window[-0x1] * b0[0x0];
        sum -= window[-0x2] * b0[0x1];
        sum -= window[-0x3] * b0[0x2];
        sum -= window[-0x4] * b0[0x3];
        sum -= window[-0x5] * b0[0x4];
        sum -= window[-0x6] * b0[0x5];
        sum -= window[-0x7] * b0[0x6];
        sum -= window[-0x8] * b0[0x7];
        sum -= window[-0x9] * b0[0x8];
        sum -= window[-0xA] * b0[0x9];
        sum -= window[-0xB] * b0[0xA];
        sum -= window[-0xC] * b0[0xB];
        sum -= window[-0xD] * b0[0xC];
        sum -= window[-0xE] * b0[0xD];
        sum -= window[-0xF] * b0[0xE];
        sum -= window[0x0] * b0[0xF];
        *samples = sum;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::synth(void * samples, float * bandPtr, int channels)
{
    int count;
    float * b0, (* buf)[288];
    int bo1, bob1, bob2;

    mSynthBo--;
    mSynthBo &= 0xF;

    bob1 = mSynthBo & 1;
    bob2 = bob1 ^ 1;
    bo1 = mSynthBo + bob2;

    if (!samples)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count = 0; count < channels; count++)
    {
        buf = mSynthBuffs[count];
        b0 = buf[bob2];

        if (mFrame.lay == 2)
        {
            dct64(buf[bob1] + ((mSynthBo + bob1) & 0xF), b0 + bo1, bandPtr + (count * 128));
        }
        if (mFrame.lay == 3)
        {
            dct64(buf[bob1] + ((mSynthBo + bob1) & 0xF), b0 + bo1, bandPtr + (count * 576));
        }

        if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCMFLOAT || mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG)
        {
            synthFloat(b0, bo1, channels, (float *)samples + count);
        }
        else
        {
            synthC(b0, bo1, channels, (short *)samples + count);
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
