// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805FB6F0..0x805FC024 (12 native functions; this build has no release callback).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference (local names and control
// flow); this older build reads the coefficients from the members instead of caching them in locals.

#include "fmod_dsp_parameq.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspparameq;

FMOD_DSP_PARAMETERDESC dspparameq_param[3] =
{
    { 20.0f, 22000.0f, 8000.0f, "Center freq", "hz", "Frequency center.  20.0 to 22000.0.  Default = 8000.0." },
    { 0.2f, 5.0f, 1.0f, "Octave range", "octaves", "Octave range around the center frequency to filter.  0.2 to 5.0.  Default = 1.0." },
    { 0.05f, 3.0f, 1.0f, "Frequency gain", "", "Frequency Gain.  0.05 to 3.0.  Default = 1.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPParamEq::getDescriptionEx()
{
    memset(&dspparameq, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspparameq.name, "FMOD ParamEQ");
    dspparameq.version = 0x00010100;
    dspparameq.create = DSPParamEq::createCallback;
    dspparameq.reset = DSPParamEq::resetCallback;
    dspparameq.read = DSPParamEq::readCallback;

    dspparameq.numparameters = sizeof(dspparameq_param) / sizeof(dspparameq_param[0]);
    dspparameq.paramdesc = dspparameq_param;
    dspparameq.setparameter = DSPParamEq::setParameterCallback;
    dspparameq.getparameter = DSPParamEq::getParameterCallback;

    dspparameq.mType = FMOD_DSP_TYPE_PARAMEQ;
    dspparameq.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspparameq.mSize = sizeof(DSPParamEq);

    return &dspparameq;
}

FMOD_RESULT DSPParamEq::createInternal()
{
    int count;

    init();

    mBandwidth = 0.2f;
    mGain = 1.0f;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    resetInternal();

    return FMOD_OK;
}

FMOD_RESULT DSPParamEq::resetInternal()
{
    int count;

    for (count = 0; count < 2; count++)
    {
        mIn1[count] = mIn2[count] = 0.0f;
        mOut1[count] = mOut2[count] = 0.0f;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPParamEq::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
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
            mOut2[0] = mOut1[0];
            mIn1[0] = in0;
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

                dc = -dc;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPParamEq::setParameterInternal(int index, float value)
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
            mCenter = value;
            if (mCenter >= ((float)outputrate / 2.0f) - 100.0f)
            {
                mCenter = ((float)outputrate / 2.0f) - 100.0f;
            }
            break;
        }
        case 1:
        {
            mBandwidth = value;
            break;
        }
        case 2:
        {
            mGain = value;
            break;
        }
    }

    if (mCenter > 20.0f)
    {
        float w0, Q, A, alpha;

        w0 = 6.2831855f * mCenter / (float)outputrate;
        Q = 1.0f / mBandwidth;
        A = mGain;
        alpha = sinf(w0) / (2.0f * Q);

        mB0 = 1.0f + alpha * A;
        mB1 = -2.0f * cosf(w0);
        mB2 = 1.0f - alpha * A;
        mA0 = 1.0f + alpha / A;
        mA1 = -2.0f * cosf(w0);
        mA2 = 1.0f - alpha / A;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPParamEq::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mCenter;
            sprintf(valuestr, "%.02f", mCenter);
            break;
        }
        case 1:
        {
            *value = mBandwidth;
            sprintf(valuestr, "%.02f", mBandwidth);
            break;
        }
        case 2:
        {
            *value = mGain;
            sprintf(valuestr, "%.02f", mGain);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPParamEq::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPParamEq * parameq = (DSPParamEq *)dsp;

    return parameq->createInternal();
}

FMOD_RESULT DSPParamEq::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPParamEq * parameq = (DSPParamEq *)dsp;

    return parameq->resetInternal();
}

FMOD_RESULT DSPParamEq::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPParamEq * parameq = (DSPParamEq *)dsp;

    return parameq->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPParamEq::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPParamEq * parameq = (DSPParamEq *)dsp;

    return parameq->setParameterInternal(index, value);
}

FMOD_RESULT DSPParamEq::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPParamEq * parameq = (DSPParamEq *)dsp;

    return parameq->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
