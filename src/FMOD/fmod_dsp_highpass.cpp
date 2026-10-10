// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F7D24..0x805F8734 (12 native functions; this build has no release callback).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference (local names and control
// flow); this older build reads the coefficients from the members instead of caching them in locals.

#include "fmod_dsp_highpass.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsphighpass;

FMOD_DSP_PARAMETERDESC dsphighpass_param[2] =
{
    { 10.0f, 22000.0f, 5000.0f, "Cutoff freq", "hz", "Highpass cutoff frequency in hz.   10.0 to 22000.0.  Default = 5000.0." },
    { 1.0f, 10.0f, 1.0f, "Resonance", "", "Highpass resonance Q value. 1.0 to 10.0.  Default = 1.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPHighPass::getDescriptionEx()
{
    memset(&dsphighpass, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsphighpass.name, "FMOD Highpass");
    dsphighpass.version = 0x00010100;
    dsphighpass.create = DSPHighPass::createCallback;
    dsphighpass.reset = DSPHighPass::resetCallback;
    dsphighpass.read = DSPHighPass::readCallback;

    dsphighpass.numparameters = sizeof(dsphighpass_param) / sizeof(dsphighpass_param[0]);
    dsphighpass.paramdesc = dsphighpass_param;
    dsphighpass.setparameter = DSPHighPass::setParameterCallback;
    dsphighpass.getparameter = DSPHighPass::getParameterCallback;

    dsphighpass.mType = FMOD_DSP_TYPE_HIGHPASS;
    dsphighpass.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dsphighpass.mSize = sizeof(DSPHighPass);

    return &dsphighpass;
}

FMOD_RESULT DSPHighPass::createInternal()
{
    int count;

    init();

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    resetInternal();

    return FMOD_OK;
}

FMOD_RESULT DSPHighPass::resetInternal()
{
    int count;

    for (count = 0; count < 2; count++)
    {
        mIn1[count] = mIn2[count] = 0.0f;
        mOut1[count] = mOut2[count] = 0.0f;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPHighPass::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;
    int count2;
    static float dc = 1e-25f;

    if (inchannels == 1)
    {
        for (count = 0; count < length; count++)
        {
            float in0, out0;

            in0 = dc + inbuffer[count];
            out0 = (mB0 * in0 + mB1 * mIn1[0] + mB2 * mIn2[0] - mA1 * mOut1[0] - mA2 * mOut2[0]) / mA0;

            mIn2[0] = mIn1[0];
            mIn1[0] = in0;
            mOut2[0] = mOut1[0];
            mOut1[0] = out0;

            outbuffer[count] = out0;

            dc = -dc;
        }
    }
    else if (inchannels == 2)
    {
        for (count = 0; count < length; count++)
        {
            float in0l, out0l;
            float in0r, out0r;

            in0l = dc + inbuffer[(count * 2) + 0];
            in0r = dc + inbuffer[(count * 2) + 1];
            out0l = (mB0 * in0l + mB1 * mIn1[0] + mB2 * mIn2[0] - mA1 * mOut1[0] - mA2 * mOut2[0]) / mA0;
            out0r = (mB0 * in0r + mB1 * mIn1[1] + mB2 * mIn2[1] - mA1 * mOut1[1] - mA2 * mOut2[1]) / mA0;

            mIn2[0] = mIn1[0];
            mIn1[0] = in0l;
            mIn2[1] = mIn1[1];
            mIn1[1] = in0r;
            mOut2[0] = mOut1[0];
            mOut1[0] = out0l;
            mOut2[1] = mOut1[1];
            mOut1[1] = out0r;

            outbuffer[(count * 2) + 0] = out0l;
            outbuffer[(count * 2) + 1] = out0r;

            dc = -dc;
        }
    }
    else
    {
        for (count = 0; count < length; count++)
        {
            for (count2 = 0; count2 < inchannels; count2++)
            {
                float in0, out0;

                in0 = dc + inbuffer[(count * inchannels) + count2];
                out0 = (mB0 * in0 + mB1 * mIn1[count2] + mB2 * mIn2[count2] - mA1 * mOut1[count2] - mA2 * mOut2[count2]) / mA0;

                mIn2[count2] = mIn1[count2];
                mIn1[count2] = in0;
                mOut2[count2] = mOut1[count2];
                mOut1[count2] = out0;

                outbuffer[(count * inchannels) + count2] = out0;
            }

            dc = -dc;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPHighPass::setParameterInternal(int index, float value)
{
    float w;
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
            if (mCutoff >= ((float)outputrate / 2.0f) - 10.0f)
            {
                mCutoff = ((float)outputrate / 2.0f) - 10.0f;
            }
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
        w = 6.2831855f * mCutoff / (float)outputrate;

        mB0 = (1.0f + cosf(w)) / 2.0f;
        mB1 = -(1.0f + cosf(w));
        mB2 = (1.0f + cosf(w)) / 2.0f;
        mA0 = 1.0f + sinf(w) / (2.0f * mResonance);
        mA1 = -2.0f * cosf(w);
        mA2 = 1.0f - sinf(w) / (2.0f * mResonance);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPHighPass::getParameterInternal(int index, float * value, char * valuestr)
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

FMOD_RESULT DSPHighPass::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPHighPass * highpass = (DSPHighPass *)dsp;

    return highpass->createInternal();
}

FMOD_RESULT DSPHighPass::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPHighPass * highpass = (DSPHighPass *)dsp;

    return highpass->resetInternal();
}

FMOD_RESULT DSPHighPass::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPHighPass * highpass = (DSPHighPass *)dsp;

    return highpass->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPHighPass::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPHighPass * highpass = (DSPHighPass *)dsp;

    return highpass->setParameterInternal(index, value);
}

FMOD_RESULT DSPHighPass::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPHighPass * highpass = (DSPHighPass *)dsp;

    return highpass->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
