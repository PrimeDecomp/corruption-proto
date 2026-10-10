// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F9E1C..0x805FA884 (13 native functions; this build has no release or reset callback).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference (local names, the inline
// filter() helper and the prewarp/bilinear/szxform design functions); this older build's readInternal
// calls filter() per sample instead of the hand-inlined 4.06 loops.

#include "fmod_dsp_lowpass.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsplowpass;

FMOD_DSP_PARAMETERDESC dsplowpass_param[2] =
{
    { 1.0f, 22000.0f, 5000.0f, "Cutoff freq", "hz", "Lowpass cutoff frequency in hz.   1.0 to output 22000.0.  Default = 5000.0." },
    { 1.0f, 10.0f, 1.0f, "Resonance", "", "Lowpass resonance Q value. 1.0 to 10.0.  Default = 1.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPLowPass::getDescriptionEx()
{
    memset(&dsplowpass, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsplowpass.name, "FMOD Lowpass");
    dsplowpass.version = 0x00010100;
    dsplowpass.create = DSPLowPass::createCallback;
    dsplowpass.read = DSPLowPass::readCallback;

    dsplowpass.numparameters = sizeof(dsplowpass_param) / sizeof(dsplowpass_param[0]);
    dsplowpass.paramdesc = dsplowpass_param;
    dsplowpass.setparameter = DSPLowPass::setParameterCallback;
    dsplowpass.getparameter = DSPLowPass::getParameterCallback;

    dsplowpass.mType = FMOD_DSP_TYPE_LOWPASS;
    dsplowpass.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dsplowpass.mSize = sizeof(DSPLowPass);

    return &dsplowpass;
}

FMOD_RESULT DSPLowPass::createInternal()
{
    int count;

    init();

    mProtoCoef[0].a0 = 1.0f;
    mProtoCoef[0].a1 = 0.0f;
    mProtoCoef[0].a2 = 0.0f;
    mProtoCoef[0].b0 = 1.0f;
    mProtoCoef[0].b1 = 0.765367f;
    mProtoCoef[0].b2 = 1.0f;

    mProtoCoef[1].a0 = 1.0f;
    mProtoCoef[1].a1 = 0.0f;
    mProtoCoef[1].a2 = 0.0f;
    mProtoCoef[1].b0 = 1.0f;
    mProtoCoef[1].b1 = 1.847759f;
    mProtoCoef[1].b2 = 1.0f;

    mNumSections = LOWPASS_FILTER_SECTIONS;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

inline float DSPLowPass::filter(float input, int channel)
{
    unsigned int i;
    float * history1;
    float * history2;
    float * coef_ptr;
    float output, new_hist;
    static float dc = 1e-25f;

    input += dc;
    dc = -dc;

    coef_ptr = mCoefficients;

    history1 = &mHistory[channel * 2 * LOWPASS_FILTER_SECTIONS];
    history2 = history1 + 1;

    output = input * (*coef_ptr++);

    for (i = 0; i < mNumSections; i++)
    {
        float h1 = *history1;
        float h2 = *history2;

        output = output - h1 * coef_ptr[0];
        new_hist = output - h2 * coef_ptr[1];

        output = new_hist + h1 * coef_ptr[2];
        output = output + h2 * coef_ptr[3];
        coef_ptr += 4;

        *history2++ = *history1;
        *history1++ = new_hist;
        history1++;
        history2++;
    }

    return output;
}

FMOD_RESULT DSPLowPass::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;
    int count2;

    if (inchannels == 1)
    {
        for (count = 0; count < length; count++)
        {
            outbuffer[count] = filter(inbuffer[count], 0);
        }
    }
    else if (inchannels == 2)
    {
        for (count = 0; count < length; count++)
        {
            outbuffer[(count * 2) + 0] = filter(inbuffer[(count * 2) + 0], 0);
            outbuffer[(count * 2) + 1] = filter(inbuffer[(count * 2) + 1], 1);
        }
    }
    else
    {
        for (count = 0; count < length; count++)
        {
            for (count2 = 0; count2 < inchannels; count2++)
            {
                outbuffer[(count * inchannels) + count2] = filter(inbuffer[(count * inchannels) + count2], count2);
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::prewarp(float * a0, float * a1, float * a2, float fc, float fs)
{
    float wp, pi;

    pi = 4.0f * (float)atan(1.0);
    wp = 2.0f * fs * (float)tan(pi * fc / fs);

    *a2 = *a2 / (wp * wp);
    *a1 = *a1 / wp;

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::bilinear(float a0, float a1, float a2, float b0, float b1, float b2, float * k, float fs, float * coef)
{
    float ad, bd;

    // alpha (Numerator in s-domain)
    ad = 4.0f * a2 * fs * fs + 2.0f * a1 * fs + a0;
    // beta (Denominator in s-domain)
    bd = 4.0f * b2 * fs * fs + 2.0f * b1 * fs + b0;

    // update gain constant for this section
    *k *= ad / bd;

    // Denominator
    *coef++ = (2.0f * b0 - 8.0f * b2 * fs * fs) / bd;
    *coef++ = (4.0f * b2 * fs * fs - 2.0f * b1 * fs + b0) / bd;

    // Numerator
    *coef++ = (2.0f * a0 - 8.0f * a2 * fs * fs) / ad;
    *coef = (4.0f * a2 * fs * fs - 2.0f * a1 * fs + a0) / ad;

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::szxform(float * a0, float * a1, float * a2, float * b0, float * b1, float * b2, float fc, float fs, float * k, float * coef)
{
    // Calculate a1 and a2 and overwrite the original values
    prewarp(a0, a1, a2, fc, fs);
    prewarp(b0, b1, b2, fc, fs);
    bilinear(*a0, *a1, *a2, *b0, *b1, *b2, k, fs, coef);

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int outputrate;

    result = mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    switch (index)
    {
        case 0:
        {
            mCutoff = value;
            break;
        }
        case 1:
        {
            mResonance = value;
            break;
        }
    }

    if (mResonance >= 1.0f)
    {
        unsigned int nInd;
        float a0, a1, a2, b0, b1, b2;
        float fs;
        float k;
        float * coef;

        k = 1.0f;
        coef = mCoefficients + 1;
        fs = (float)outputrate;

        for (nInd = 0; nInd < mNumSections; nInd++)
        {
            a0 = mProtoCoef[nInd].a0;
            a1 = mProtoCoef[nInd].a1;
            a2 = mProtoCoef[nInd].a2;

            b0 = mProtoCoef[nInd].b0;
            b1 = mProtoCoef[nInd].b1 / mResonance;
            b2 = mProtoCoef[nInd].b2;

            szxform(&a0, &a1, &a2, &b0, &b1, &b2, mCutoff, fs, &k, coef);

            coef += 4;
        }

        mCoefficients[0] = k;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mCutoff;
            sprintf(valuestr, "%.02f", mCutoff);
            break;
        }
        case 1:
        {
            *value = mResonance;
            sprintf(valuestr, "%.02f", mResonance);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPLowPass::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPLowPass * lowpass = (DSPLowPass *)dsp;

    return lowpass->createInternal();
}

FMOD_RESULT DSPLowPass::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPLowPass * lowpass = (DSPLowPass *)dsp;

    return lowpass->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPLowPass::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPLowPass * lowpass = (DSPLowPass *)dsp;

    return lowpass->setParameterInternal(index, value);
}

FMOD_RESULT DSPLowPass::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPLowPass * lowpass = (DSPLowPass *)dsp;

    return lowpass->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
