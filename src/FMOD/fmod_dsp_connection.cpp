// G2MEAB fmod_dsp_connection.cpp: reconstruction (group D).
// .text: 0x805F1364..0x805F3F7C (12 native functions, in this order): init 0x805F1364, reset 0x805F1440,
// mix 0x805F1548, mixAndRamp 0x805F2A38, rampTo 0x805F2C74, checkUnity 0x805F2E14, setPan 0x805F2EB8,
// updatePan 0x805F2ECC, setMix 0x805F3AE0, getMix 0x805F3B38, setLevels 0x805F3B58 and getLevels 0x805F3E9C.
// Level rows hold 8 floats each and come from the pool's level memory; only two rows exist (init), but the
// six-speaker paths of mix and setLevels still index rows 2-5 as the native code does.

#include "fmod_dsp_connection.h"
#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_msl_math.h"

#include <math.h>

namespace FMOD {

FMOD_RESULT DSPConnection::init(float * & levelmemory, int maxinputlevels)
{
    int count;

    mMaxInputLevels = maxinputlevels;
    if (maxinputlevels < 2)
    {
        maxinputlevels = 2;
    }

    for (count = 0; count < 2; count++)
    {
        if (count < maxinputlevels)
        {
            mLevel[count] = levelmemory;
            levelmemory += 8;
            mLevelCurrent[count] = levelmemory;
            levelmemory += 8;
            mLevelDelta[count] = levelmemory;
            levelmemory += 8;
        }
        else
        {
            mLevel[count] = 0;
            mLevelCurrent[count] = 0;
            mLevelDelta[count] = 0;
        }
    }

    mNewPan = mPan = 0.0f;

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::reset()
{
    int count;

    mVolume = 1.0f;

    for (count = 0; count < mMaxInputLevels; count++)
    {
        int count2;

        for (count2 = 0; count2 < 8; count2++)
        {
            mLevel[count][count2] = 0.0f;
            mLevelCurrent[count][count2] = 0.0f;
            mLevelDelta[count][count2] = 0.0f;
        }
    }

    mNewPan = -2.0f;
    mPan = -2.0f;
    mSetLevelsUsed = false;

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::mix(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length)
{
    FMOD_RESULT result;
    unsigned int count;
    unsigned int len;

    if (mRampCount)
    {
        len = mRampCount;

        if (len > length)
        {
            len = length;
        }

        result = mixAndRamp(outbuffer, inbuffer, outchannels, inchannels, len);
        if (result != FMOD_OK)
        {
            return result;
        }

        length -= len;
        outbuffer += len * outchannels;
        inbuffer += len * inchannels;
    }

    if (!length)
    {
        return FMOD_OK;
    }

    if (mVolume == 0.0f)
    {
        return FMOD_OK;
    }

    float l00 = mLevelCurrent[0] ? mLevelCurrent[0][0] : 0.0f;
    float l10 = mLevelCurrent[1] ? mLevelCurrent[1][0] : 0.0f;
    float l01 = mLevelCurrent[0] ? mLevelCurrent[0][1] : 0.0f;
    float l11 = mLevelCurrent[1] ? mLevelCurrent[1][1] : 0.0f;
    // Guessed names: the six-speaker rows beyond the two allocated ones contribute nothing.
    float l20 = 0.0f, l30 = 0.0f, l40 = 0.0f, l50 = 0.0f;
    float l22 = 0.0f, l33 = 0.0f, l44 = 0.0f, l55 = 0.0f;

    if (outchannels == 2 && (inchannels == 1 || inchannels == 2))
    {
        if (inchannels == 1)
        {
            len = length >> 2;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[0] * l10;
                outbuffer[2] += inbuffer[1] * l00;
                outbuffer[3] += inbuffer[1] * l10;
                outbuffer[4] += inbuffer[2] * l00;
                outbuffer[5] += inbuffer[2] * l10;
                outbuffer[6] += inbuffer[3] * l00;
                outbuffer[7] += inbuffer[3] * l10;
                inbuffer += 4;
                outbuffer += 8;
                len--;
            }
            len = length & 3;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[0] * l10;
                inbuffer += 1;
                outbuffer += 2;
                len--;
            }
        }
        else if (inchannels == 2)
        {
            if (l01 < 0.0001f && l10 < 0.0001f)
            {
                len = length >> 2;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[1] += inbuffer[1] * l11;
                    outbuffer[2] += inbuffer[2] * l00;
                    outbuffer[3] += inbuffer[3] * l11;
                    outbuffer[4] += inbuffer[4] * l00;
                    outbuffer[5] += inbuffer[5] * l11;
                    outbuffer[6] += inbuffer[6] * l00;
                    outbuffer[7] += inbuffer[7] * l11;
                    inbuffer += 8;
                    outbuffer += 8;
                    len--;
                }
                len = length & 3;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[1] += inbuffer[1] * l11;
                    inbuffer += 2;
                    outbuffer += 2;
                    len--;
                }
            }
            else
            {
                len = length >> 2;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[0] += inbuffer[1] * l01;
                    outbuffer[1] += inbuffer[0] * l10;
                    outbuffer[1] += inbuffer[1] * l11;
                    outbuffer[2] += inbuffer[2] * l00;
                    outbuffer[2] += inbuffer[3] * l01;
                    outbuffer[3] += inbuffer[2] * l10;
                    outbuffer[3] += inbuffer[3] * l11;
                    outbuffer[4] += inbuffer[4] * l00;
                    outbuffer[4] += inbuffer[5] * l01;
                    outbuffer[5] += inbuffer[4] * l10;
                    outbuffer[5] += inbuffer[5] * l11;
                    outbuffer[6] += inbuffer[6] * l00;
                    outbuffer[6] += inbuffer[7] * l01;
                    outbuffer[7] += inbuffer[6] * l10;
                    outbuffer[7] += inbuffer[7] * l11;
                    inbuffer += 8;
                    outbuffer += 8;
                    len--;
                }
                len = length & 3;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[0] += inbuffer[1] * l01;
                    outbuffer[1] += inbuffer[0] * l10;
                    outbuffer[1] += inbuffer[1] * l11;
                    inbuffer += 2;
                    outbuffer += 2;
                    len--;
                }
            }
        }
    }
    else if (outchannels == 6 && (inchannels == 1 || inchannels == 2 || inchannels == 6))
    {
        if (inchannels == 1)
        {
            len = length >> 2;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[0] * l10;
                outbuffer[2] += inbuffer[0] * l20;
                outbuffer[3] += inbuffer[0] * l30;
                outbuffer[4] += inbuffer[0] * l40;
                outbuffer[5] += inbuffer[0] * l50;
                outbuffer[6] += inbuffer[1] * l00;
                outbuffer[7] += inbuffer[1] * l10;
                outbuffer[8] += inbuffer[1] * l20;
                outbuffer[9] += inbuffer[1] * l30;
                outbuffer[10] += inbuffer[1] * l40;
                outbuffer[11] += inbuffer[1] * l50;
                outbuffer[12] += inbuffer[2] * l00;
                outbuffer[13] += inbuffer[2] * l10;
                outbuffer[14] += inbuffer[2] * l20;
                outbuffer[15] += inbuffer[2] * l30;
                outbuffer[16] += inbuffer[2] * l40;
                outbuffer[17] += inbuffer[2] * l50;
                outbuffer[18] += inbuffer[3] * l00;
                outbuffer[19] += inbuffer[3] * l10;
                outbuffer[20] += inbuffer[3] * l20;
                outbuffer[21] += inbuffer[3] * l30;
                outbuffer[22] += inbuffer[3] * l40;
                outbuffer[23] += inbuffer[3] * l50;
                inbuffer += 4;
                outbuffer += 24;
                len--;
            }
            len = length & 3;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[0] * l10;
                outbuffer[2] += inbuffer[0] * l20;
                outbuffer[3] += inbuffer[0] * l30;
                outbuffer[4] += inbuffer[0] * l40;
                outbuffer[5] += inbuffer[0] * l50;
                inbuffer += 1;
                outbuffer += 6;
                len--;
            }
        }
        else if (inchannels == 2)
        {
            if (l01 < 0.0001f && l10 < 0.0001f)
            {
                len = length >> 2;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[1] += inbuffer[1] * l11;
                    outbuffer[6] += inbuffer[2] * l00;
                    outbuffer[7] += inbuffer[3] * l11;
                    outbuffer[12] += inbuffer[4] * l00;
                    outbuffer[13] += inbuffer[5] * l11;
                    outbuffer[18] += inbuffer[6] * l00;
                    outbuffer[19] += inbuffer[7] * l11;
                    inbuffer += 8;
                    outbuffer += 24;
                    len--;
                }
                len = length & 3;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[1] += inbuffer[1] * l11;
                    inbuffer += 2;
                    outbuffer += 6;
                    len--;
                }
            }
            else
            {
                // The native code steps this path with a stereo output stride.
                len = length >> 2;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[0] += inbuffer[1] * l01;
                    outbuffer[1] += inbuffer[0] * l10;
                    outbuffer[1] += inbuffer[1] * l11;
                    outbuffer[2] += inbuffer[2] * l00;
                    outbuffer[2] += inbuffer[3] * l01;
                    outbuffer[3] += inbuffer[2] * l10;
                    outbuffer[3] += inbuffer[3] * l11;
                    outbuffer[4] += inbuffer[4] * l00;
                    outbuffer[4] += inbuffer[5] * l01;
                    outbuffer[5] += inbuffer[4] * l10;
                    outbuffer[5] += inbuffer[5] * l11;
                    outbuffer[6] += inbuffer[6] * l00;
                    outbuffer[6] += inbuffer[7] * l01;
                    outbuffer[7] += inbuffer[6] * l10;
                    outbuffer[7] += inbuffer[7] * l11;
                    inbuffer += 8;
                    outbuffer += 8;
                    len--;
                }
                len = length & 3;
            while (len)
                {
                    outbuffer[0] += inbuffer[0] * l00;
                    outbuffer[0] += inbuffer[1] * l01;
                    outbuffer[1] += inbuffer[0] * l10;
                    outbuffer[1] += inbuffer[1] * l11;
                    inbuffer += 2;
                    outbuffer += 2;
                    len--;
                }
            }
        }
        else if (inchannels == 6)
        {
            len = length >> 2;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[1] * l11;
                outbuffer[2] += inbuffer[2] * l22;
                outbuffer[3] += inbuffer[3] * l33;
                outbuffer[4] += inbuffer[4] * l44;
                outbuffer[5] += inbuffer[5] * l55;
                outbuffer[6] += inbuffer[6] * l00;
                outbuffer[7] += inbuffer[7] * l11;
                outbuffer[8] += inbuffer[8] * l22;
                outbuffer[9] += inbuffer[9] * l33;
                outbuffer[10] += inbuffer[10] * l44;
                outbuffer[11] += inbuffer[11] * l55;
                outbuffer[12] += inbuffer[12] * l00;
                outbuffer[13] += inbuffer[13] * l11;
                outbuffer[14] += inbuffer[14] * l22;
                outbuffer[15] += inbuffer[15] * l33;
                outbuffer[16] += inbuffer[16] * l44;
                outbuffer[17] += inbuffer[17] * l55;
                outbuffer[18] += inbuffer[18] * l00;
                outbuffer[19] += inbuffer[19] * l11;
                outbuffer[20] += inbuffer[20] * l22;
                outbuffer[21] += inbuffer[21] * l33;
                outbuffer[22] += inbuffer[22] * l44;
                outbuffer[23] += inbuffer[23] * l55;
                inbuffer += 24;
                outbuffer += 24;
                len--;
            }
            len = length & 3;
            while (len)
            {
                outbuffer[0] += inbuffer[0] * l00;
                outbuffer[1] += inbuffer[1] * l11;
                outbuffer[2] += inbuffer[2] * l22;
                outbuffer[3] += inbuffer[3] * l33;
                outbuffer[4] += inbuffer[4] * l44;
                outbuffer[5] += inbuffer[5] * l55;
                inbuffer += 6;
                outbuffer += 6;
                len--;
            }
        }
    }
    else
    {
        for (count = 0; count < length; count++)
        {
            int count2;

            for (count2 = 0; count2 < outchannels; count2++)
            {
                float sum = 0.0f;
                int count3;

                for (count3 = 0; count3 < inchannels; count3++)
                {
                    sum += inbuffer[count3] * mLevelCurrent[count2][count3];
                }

                *outbuffer++ += sum;
            }

            inbuffer += inchannels;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::mixAndRamp(float * outbuffer, float * inbuffer, int outchannels, int inchannels, unsigned int length)
{
    unsigned int count;

    for (count = 0; count < length; count++)
    {
        int count2;

        for (count2 = 0; count2 < outchannels; count2++)
        {
            float sum = 0.0f;
            float value = *outbuffer;
            int count3;

            for (count3 = 0; count3 < inchannels; count3++)
            {
                sum += inbuffer[count3] * mLevelCurrent[count2][count3];
                mLevelCurrent[count2][count3] += mLevelDelta[count2][count3];
            }

            *outbuffer++ = value + sum;
        }

        inbuffer += inchannels;
    }

    mRampCount -= length;

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::rampTo()
{
    int count;
    float oneoverrampcount;

    mRampCount = DSP_RAMPCOUNT;
    oneoverrampcount = 1.0f / (float)mRampCount;

    for (count = 0; count < mMaxInputLevels; count++)
    {
        int count2;

        for (count2 = 0; count2 < 8; count2++)
        {
            mLevelDelta[count][count2] = oneoverrampcount * ((mLevel[count][count2] * mVolume) - mLevelCurrent[count][count2]);
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::checkUnity(int outchannels, int inchannels)
{
    if (mVolume == 1.0f && inchannels == outchannels)
    {
        int count;

        for (count = 0; count < outchannels; count++)
        {
            int count2;

            for (count2 = 0; count2 < inchannels; count2++)
            {
                if (count == count2 && mLevel[count][count2] != 1.0f)
                {
                    return FMOD_ERR_PAN;
                }
                if (count != count2 && mLevel[count][count2] != 0.0f)
                {
                    return FMOD_ERR_PAN;
                }
            }
        }

        return FMOD_OK;
    }

    return FMOD_ERR_PAN;
}

FMOD_RESULT DSPConnection::setPan(float pan)
{
    mPan = -2.0f;
    mNewPan = pan;

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::updatePan(int outchannels, int inchannels, FMOD_SPEAKERMODE speakermode)
{
    int count;
    int count2;

    if (mNewPan == mPan)
    {
        return FMOD_OK;
    }

    mPan = mNewPan;

    if (!mInputUnit->mSystem)
    {
        return FMOD_ERR_INTERNAL;
    }

    for (count = 0; count < outchannels; count++)
    {
        for (count2 = 0; count2 < inchannels; count2++)
        {
            mLevel[count][count2] = 0.0f;
        }
    }

    switch (speakermode)
    {
        case FMOD_SPEAKERMODE_RAW:
        {
            for (count = 0; count < outchannels; count++)
            {
                for (count2 = 0; count2 < inchannels; count2++)
                {
                    if (count == count2)
                    {
                        mLevel[count][count2] = 1.0f;
                    }
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_MONO:
        {
            for (count = 0; count < inchannels; count++)
            {
                mLevel[0][count] = 1.0f;
            }
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        case 1000: // Guessed: an undocumented linear-pan stereo mode
        {
            float pan = (1.0f + mPan) * 0.5f;

            if (inchannels == 1)
            {
                float l = 1.0f - pan;
                float r = pan;

                if (speakermode == FMOD_SPEAKERMODE_STEREO)
                {
                    l = MSL_sqrtf(l);
                    r = MSL_sqrtf(r);
                }

                mLevel[0][0] = l;
                mLevel[1][0] = r;
            }
            else if (inchannels == 2)
            {
                float l, r;

                if (pan <= 0.5f)
                {
                    l = 1.0f;
                    r = 2.0f * pan;
                }
                else
                {
                    l = 2.0f * (1.0f - pan);
                    r = 1.0f;
                }

                mLevel[0][0] = l;
                mLevel[1][1] = r;
            }
            else
            {
                for (count = 0; count < inchannels; count++)
                {
                    mLevel[0][count] = 1.0f;
                    mLevel[1][count] = 1.0f;
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        case FMOD_SPEAKERMODE_SURROUND:
        case FMOD_SPEAKERMODE_5POINT1:
        case FMOD_SPEAKERMODE_7POINT1:
        {
            float pan = (1.0f + mPan) * 0.5f;

            if (inchannels == 1)
            {
                mLevel[0][0] = MSL_sqrtf(1.0f - pan);
                mLevel[1][0] = MSL_sqrtf(pan);
            }
            else if (inchannels == 2)
            {
                float l, r;

                if (pan <= 0.5f)
                {
                    l = 1.0f;
                    r = 2.0f * pan;
                }
                else
                {
                    l = 2.0f * (1.0f - pan);
                    r = 1.0f;
                }

                mLevel[0][0] = l;
                mLevel[1][1] = r;
            }
            else
            {
                for (count = 0; count < outchannels; count++)
                {
                    for (count2 = 0; count2 < inchannels; count2++)
                    {
                        if (count == count2)
                        {
                            mLevel[count][count2] = 1.0f;
                        }
                    }
                }
            }
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            float pan = (1.0f + mPan) * 0.5f;

            if (inchannels == 1)
            {
                mLevel[0][0] = MSL_sqrtf(1.0f - pan);
                mLevel[1][0] = MSL_sqrtf(pan);
            }
            else if (inchannels == 2)
            {
                float l, r;

                if (pan < 0.5f)
                {
                    l = 1.0f;
                    r = 2.0f * pan;
                }
                else
                {
                    l = 2.0f * (1.0f - pan);
                    r = 1.0f;
                }

                mLevel[0][0] = l;
                mLevel[1][1] = r;
            }
            else
            {
                for (count = 0; count < inchannels; count++)
                {
                    mLevel[0][count] = 1.0f;
                    mLevel[1][count] = 1.0f;
                }
            }
            break;
        }
    }

    mSetLevelsUsed = true;

    return rampTo();
}

FMOD_RESULT DSPConnection::setMix(float volume)
{
    if (volume < -1.0f)
    {
        volume = -1.0f;
    }
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }

    if (mVolume == volume)
    {
        return FMOD_OK;
    }

    mVolume = volume;

    return rampTo();
}

FMOD_RESULT DSPConnection::getMix(float * volume)
{
    if (!volume)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *volume = mVolume;

    return FMOD_OK;
}

FMOD_RESULT DSPConnection::setLevels(float * levels, int numinputlevels)
{
    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mNewPan = mPan;

    if (mMaxInputLevels == 6 && numinputlevels < 3)
    {
        int count;

        if (numinputlevels == 1)
        {
            float diff = 0.0f;

            for (count = 0; count < 6; count++)
            {
                diff += mLevel[count][0] - levels[count];
            }

            if (fabs(diff) < 0.00001f)
            {
                return FMOD_OK;
            }

            for (count = 0; count < 6; count++)
            {
                mLevel[count][0] = levels[count];
            }
        }
        else
        {
            for (count = 0; count < 6; count++)
            {
                mLevel[count][0] = levels[(count * 2) + 0];
                mLevel[count][1] = levels[(count * 2) + 1];
            }
        }
    }
    else
    {
        int count;

        for (count = 0; count < mMaxInputLevels; count++)
        {
            int count2;

            for (count2 = 0; count2 < numinputlevels; count2++)
            {
                mLevel[count][count2] = levels[(count * numinputlevels) + count2];
            }
        }
    }

    mSetLevelsUsed = true;

    return rampTo();
}

FMOD_RESULT DSPConnection::getLevels(float * levels)
{
    int count;

    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count = 0; count < mMaxInputLevels; count++)
    {
        int count2;

        for (count2 = 0; count2 < 8; count2++)
        {
            levels[(count * 8) + count2] = mLevel[count][count2];
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
