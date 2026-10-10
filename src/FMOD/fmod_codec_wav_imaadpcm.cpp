// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805E9924..0x805EAD1C (4 native functions).
// Provisional source-family name; original basename and standalone placement are unproven.
// IMA ADPCM block decoders (Microsoft reference structure): mono/stereo to 16-bit PCM and mono/stereo
// to float. CodecWav::readInternal calls all four; the sample helpers are header-style inlines that
// G2MEAB expands everywhere. Block headers are little-endian and swapped by hand.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec_wav_imaadpcm.h"
#include "fmod.h"
#include "fmod_types.h"

namespace FMOD {

static const int IMAAdpcm_IndexTab[16] =
{
    -1, -1, -1, -1, 2, 4, 6, 8,
    -1, -1, -1, -1, 2, 4, 6, 8
};

static const short IMAAdpcm_StepTab[89] =
{
    7, 8, 9, 10, 11, 12, 13, 14,
    16, 17, 19, 21, 23, 25, 28, 31,
    34, 37, 41, 45, 50, 55, 60, 66,
    73, 80, 88, 97, 107, 118, 130, 143,
    157, 173, 190, 209, 230, 253, 279, 307,
    337, 371, 408, 449, 494, 544, 598, 658,
    724, 796, 876, 963, 1060, 1166, 1282, 1411,
    1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024,
    3327, 3660, 4026, 4428, 4871, 5358, 5894, 6484,
    7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
    15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794,
    32767
};

inline int IMAAdpcm_NextStepIndex(int nEncodedSample, int nStepIndex)
{
    nStepIndex += IMAAdpcm_IndexTab[nEncodedSample];

    if (nStepIndex < 0)
    {
        nStepIndex = 0;
    }
    else if (nStepIndex > 88)
    {
        nStepIndex = 88;
    }

    return nStepIndex;
}

inline bool ValidStepIndex(int nStepIndex)
{
    return (nStepIndex >= 0 && nStepIndex <= 88);
}

inline int IMAAdpcm_DecodeSample(int nEncodedSample, int nPredictedSample, int nStepSize)
{
    int lDifference;
    int lNewSample;

    lDifference = nStepSize >> 3;

    switch (nEncodedSample)
    {
        case 0:
            break;
        case 1:
            lDifference += nStepSize >> 2;
            break;
        case 2:
            lDifference += nStepSize >> 1;
            break;
        case 3:
            lDifference += nStepSize >> 1;
            lDifference += nStepSize >> 2;
            break;
        case 4:
            lDifference += nStepSize;
            break;
        case 5:
            lDifference += nStepSize;
            lDifference += nStepSize >> 2;
            break;
        case 6:
            lDifference += nStepSize;
            lDifference += nStepSize >> 1;
            break;
        case 7:
            lDifference += nStepSize;
            lDifference += nStepSize >> 1;
            lDifference += nStepSize >> 2;
            break;
        case 8:
            lDifference = -lDifference;
            break;
        case 9:
            lDifference += nStepSize >> 2;
            lDifference = -lDifference;
            break;
        case 10:
            lDifference += nStepSize >> 1;
            lDifference = -lDifference;
            break;
        case 11:
            lDifference += nStepSize >> 1;
            lDifference += nStepSize >> 2;
            lDifference = -lDifference;
            break;
        case 12:
            lDifference += nStepSize;
            lDifference = -lDifference;
            break;
        case 13:
            lDifference += nStepSize;
            lDifference += nStepSize >> 2;
            lDifference = -lDifference;
            break;
        case 14:
            lDifference += nStepSize;
            lDifference += nStepSize >> 1;
            lDifference = -lDifference;
            break;
        case 15:
            lDifference += nStepSize;
            lDifference += nStepSize >> 1;
            lDifference += nStepSize >> 2;
            lDifference = -lDifference;
            break;
    }

    lNewSample = nPredictedSample + lDifference;

    if ((int)(short)lNewSample != lNewSample)
    {
        if (lNewSample < -32768)
        {
            lNewSample = -32768;
        }
        else
        {
            lNewSample = 32767;
        }
    }

    return lNewSample;
}

FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned char * pbBlock;
    unsigned int cSamples;
    unsigned char bSample;
    int nStepSize;
    int nEncSample;
    int nPredSample;
    int nStepIndex;
    unsigned int dwHeader;

    while (cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSample = (int)(short)(dwHeader & 0xFFFF);
        nStepIndex = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndex))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        *pbDst = (short)nPredSample;
        pbDst += channels;

        while (cSamples)
        {
            bSample = *pbBlock++;

            nEncSample = (bSample & 0x0F);
            nStepSize = IMAAdpcm_StepTab[nStepIndex];
            nPredSample = IMAAdpcm_DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = IMAAdpcm_NextStepIndex(nEncSample, nStepIndex);

            *pbDst = (short)nPredSample;
            pbDst += channels;

            cSamples--;
            if (!cSamples)
            {
                break;
            }

            nEncSample = (bSample >> 4);
            nStepSize = IMAAdpcm_StepTab[nStepIndex];
            nPredSample = IMAAdpcm_DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = IMAAdpcm_NextStepIndex(nEncSample, nStepIndex);

            *pbDst = (short)nPredSample;
            pbDst += channels;

            cSamples--;
        }

        pbSrc += nBlockAlignment;
    }

    return result;
}

FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned char * pbBlock;
    unsigned int cSamples;
    unsigned int cSubSamples;
    int nStepSize;
    unsigned int dwHeader;
    unsigned int dwLeft;
    unsigned int dwRight;
    int nEncSampleL;
    int nPredSampleL;
    int nStepIndexL;
    int nEncSampleR;
    int nPredSampleR;
    int nStepIndexR;
    unsigned int i;

    while (cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSampleL = (int)(short)(dwHeader & 0xFFFF);
        nStepIndexL = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndexL))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSampleR = (int)(short)(dwHeader & 0xFFFF);
        nStepIndexR = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndexR))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        *pbDst++ = (short)nPredSampleL;
        *pbDst++ = (short)nPredSampleR;

        while (cSamples)
        {
            dwLeft = *(unsigned int *)pbBlock;
            dwLeft = FMOD_SWAPENDIAN_DWORD(dwLeft);
            dwRight = *(unsigned int *)(pbBlock + sizeof(unsigned int));
            dwRight = FMOD_SWAPENDIAN_DWORD(dwRight);
            pbBlock += 2 * sizeof(unsigned int);

            cSubSamples = 8;
            if (cSamples < 8)
            {
                cSubSamples = cSamples;
            }

            for (i = 0; i < cSubSamples; i++)
            {
                nEncSampleL = (dwLeft & 0x0F);
                nStepSize = IMAAdpcm_StepTab[nStepIndexL];
                nPredSampleL = IMAAdpcm_DecodeSample(nEncSampleL, nPredSampleL, nStepSize);
                nStepIndexL = IMAAdpcm_NextStepIndex(nEncSampleL, nStepIndexL);

                nEncSampleR = (dwRight & 0x0F);
                nStepSize = IMAAdpcm_StepTab[nStepIndexR];
                nPredSampleR = IMAAdpcm_DecodeSample(nEncSampleR, nPredSampleR, nStepSize);
                nStepIndexR = IMAAdpcm_NextStepIndex(nEncSampleR, nStepIndexR);

                *pbDst++ = (short)nPredSampleL;
                *pbDst++ = (short)nPredSampleR;

                dwLeft >>= 4;
                dwRight >>= 4;
            }

            cSamples -= cSubSamples;
        }

        pbSrc += nBlockAlignment;
    }

    return result;
}

FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned char * pbBlock;
    unsigned int cSamples;
    unsigned char bSample;
    int nStepSize;
    int nEncSample;
    int nPredSample;
    int nStepIndex;
    unsigned int dwHeader;

    while (cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSample = (int)(short)(dwHeader & 0xFFFF);
        nStepIndex = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndex))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        *pbDst = (float)nPredSample * (1.0f / 32768.0f);
        pbDst += channels;

        while (cSamples > 1)
        {
            bSample = *pbBlock++;

            nEncSample = (bSample & 0x0F);
            nStepSize = IMAAdpcm_StepTab[nStepIndex];
            nPredSample = IMAAdpcm_DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = IMAAdpcm_NextStepIndex(nEncSample, nStepIndex);

            *pbDst = (float)nPredSample * (1.0f / 32768.0f);
            pbDst += channels;

            nEncSample = (bSample >> 4);
            nStepSize = IMAAdpcm_StepTab[nStepIndex];
            nPredSample = IMAAdpcm_DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = IMAAdpcm_NextStepIndex(nEncSample, nStepIndex);

            *pbDst = (float)nPredSample * (1.0f / 32768.0f);
            pbDst += channels;

            cSamples -= 2;
        }

        if (cSamples)
        {
            bSample = *pbBlock++;

            nEncSample = (bSample & 0x0F);
            nStepSize = IMAAdpcm_StepTab[nStepIndex];
            nPredSample = IMAAdpcm_DecodeSample(nEncSample, nPredSample, nStepSize);
            nStepIndex = IMAAdpcm_NextStepIndex(nEncSample, nStepIndex);

            *pbDst = (float)nPredSample * (1.0f / 32768.0f);
            pbDst += channels;
        }

        pbSrc += nBlockAlignment;
    }

    return result;
}

FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned char * pbBlock;
    unsigned int cSamples;
    unsigned int cSubSamples;
    int nStepSize;
    unsigned int dwHeader;
    unsigned int dwLeft;
    unsigned int dwRight;
    int nEncSampleL;
    int nPredSampleL;
    int nStepIndexL;
    int nEncSampleR;
    int nPredSampleR;
    int nStepIndexR;
    unsigned int i;

    while (cBlocks--)
    {
        pbBlock = pbSrc;
        cSamples = cSamplesPerBlock - 1;

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSampleL = (int)(short)(dwHeader & 0xFFFF);
        nStepIndexL = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndexL))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        dwHeader = *(unsigned int *)pbBlock;
        dwHeader = FMOD_SWAPENDIAN_DWORD(dwHeader);
        pbBlock += sizeof(unsigned int);

        nPredSampleR = (int)(short)(dwHeader & 0xFFFF);
        nStepIndexR = (int)(unsigned char)(dwHeader >> 16);

        if (!ValidStepIndex(nStepIndexR))
        {
            result = FMOD_ERR_FILE_BAD;
            break;
        }

        *pbDst++ = (float)nPredSampleL * (1.0f / 32768.0f);
        *pbDst++ = (float)nPredSampleR * (1.0f / 32768.0f);

        while (cSamples)
        {
            dwLeft = *(unsigned int *)pbBlock;
            dwLeft = FMOD_SWAPENDIAN_DWORD(dwLeft);
            dwRight = *(unsigned int *)(pbBlock + sizeof(unsigned int));
            dwRight = FMOD_SWAPENDIAN_DWORD(dwRight);
            pbBlock += 2 * sizeof(unsigned int);

            cSubSamples = 8;
            if (cSamples < 8)
            {
                cSubSamples = cSamples;
            }

            for (i = 0; i < cSubSamples; i++)
            {
                nEncSampleL = (dwLeft & 0x0F);
                nStepSize = IMAAdpcm_StepTab[nStepIndexL];
                nPredSampleL = IMAAdpcm_DecodeSample(nEncSampleL, nPredSampleL, nStepSize);
                nStepIndexL = IMAAdpcm_NextStepIndex(nEncSampleL, nStepIndexL);

                nEncSampleR = (dwRight & 0x0F);
                nStepSize = IMAAdpcm_StepTab[nStepIndexR];
                nPredSampleR = IMAAdpcm_DecodeSample(nEncSampleR, nPredSampleR, nStepSize);
                nStepIndexR = IMAAdpcm_NextStepIndex(nEncSampleR, nStepIndexR);

                *pbDst++ = (float)nPredSampleL * (1.0f / 32768.0f);
                *pbDst++ = (float)nPredSampleR * (1.0f / 32768.0f);

                dwLeft >>= 4;
                dwRight >>= 4;
            }

            cSamples -= cSubSamples;
        }

        pbSrc += nBlockAlignment;
    }

    return result;
}

} // namespace FMOD
