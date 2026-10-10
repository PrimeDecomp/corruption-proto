// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805F9180..0x805F9E1C (12 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the IT low-pass; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects. The coefficients follow the Impulse Tracker resonant filter.

#include "fmod_dsp_itlowpass.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspitlowpass;

FMOD_DSP_PARAMETERDESC dspitlowpass_param[2] =
{
    { 1.0f, 22000.0f, 5000.0f, "Cutoff freq", "hz", "Lowpass cutoff frequency in hz.  1.0 to 22000.0.  Default = 5000.0" },
    { 1.0f, 127.0f, 1.0f, "Resonance", "", "Lowpass resonance Q value. 0.0 to 127.0.  Default = 1.0" }
};

FMOD_DSP_DESCRIPTION_EX * DSPITLowPass::getDescriptionEx()
{
    memset(&dspitlowpass, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspitlowpass.name, "FMOD IT Lowpass");
    dspitlowpass.version = 0x00010100;
    dspitlowpass.create = DSPITLowPass::createCallback;
    dspitlowpass.reset = DSPITLowPass::resetCallback;
    dspitlowpass.read = DSPITLowPass::readCallback;

    dspitlowpass.numparameters = sizeof(dspitlowpass_param) / sizeof(dspitlowpass_param[0]);
    dspitlowpass.paramdesc = dspitlowpass_param;
    dspitlowpass.setparameter = DSPITLowPass::setParameterCallback;
    dspitlowpass.getparameter = DSPITLowPass::getParameterCallback;

    dspitlowpass.mType = FMOD_DSP_TYPE_ITLOWPASS;
    dspitlowpass.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspitlowpass.mSize = sizeof(DSPITLowPass);

    return &dspitlowpass;
}

FMOD_RESULT DSPITLowPass::createInternal()
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

FMOD_RESULT DSPITLowPass::resetInternal()
{
    int count;

    for (count = 0; count < 2; count++)
    {
        mHistory[count][1] = 0.0f;
        mHistory[count][0] = 0.0f;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITLowPass::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;
    int count2;
    static float dc = 1e-25f;

    if (inchannels == 1)
    {
        float y1 = mHistory[0][0];
        float y2 = mHistory[0][1];

        for (count = 0; count < length; count++)
        {
            float out = mCoefA * (dc + inbuffer[count]) + y1 * mCoefB + y2 * mCoefC;

            y2 = y1;
            y1 = out;
            outbuffer[count] = out;

            dc = -dc;
        }

        mHistory[0][0] = y1;
        mHistory[0][1] = y2;
    }
    else if (inchannels == 2)
    {
        float y1l = mHistory[0][0];
        float y2l = mHistory[0][1];
        float y1r = mHistory[1][0];
        float y2r = mHistory[1][1];

        for (count = 0; count < length; count++)
        {
            float outl = mCoefA * (dc + inbuffer[(count * 2) + 0]) + y1l * mCoefB + y2l * mCoefC;
            float outr;

            y2l = y1l;
            y1l = outl;
            outbuffer[(count * 2) + 0] = outl;

            outr = mCoefA * (dc + inbuffer[(count * 2) + 1]) + y1r * mCoefB + y2r * mCoefC;

            y2r = y1r;
            y1r = outr;
            outbuffer[(count * 2) + 1] = outr;

            dc = -dc;
        }

        mHistory[0][0] = y1l;
        mHistory[0][1] = y2l;
        mHistory[1][0] = y1r;
        mHistory[1][1] = y2r;
    }
    else
    {
        for (count2 = 0; count2 < inchannels; count2++)
        {
            float y1 = mHistory[count2][0];
            float y2 = mHistory[count2][1];

            for (count = 0; count < length; count++)
            {
                float out = mCoefA * (dc + inbuffer[(count * inchannels) + count2]) + y1 * mCoefB + y2 * mCoefC;

                y2 = y1;
                y1 = out;
                outbuffer[(count * inchannels) + count2] = out;

                dc = -dc;
            }

            mHistory[count2][0] = y1;
            mHistory[count2][1] = y2;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPITLowPass::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int outputrate;
    float fc, dmpfac, d, e, den;

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

    fc = mCutoff * (6.2831855f / (float)outputrate);
    dmpfac = (float)pow(10.0, -(0.1875f * mResonance) / 20.0f);

    d = (1.0f - 2.0f * dmpfac) * fc;
    if (d > 2.0f)
    {
        d = 2.0f;
    }
    d = (2.0f * dmpfac - d) / fc;
    e = (float)pow(1.0f / fc, 2.0);

    den = 1.0f + d + e;
    mCoefA = 1.0f / den;
    mCoefB = (d + e + e) / den;
    mCoefC = -e / den;

    return FMOD_OK;
}

FMOD_RESULT DSPITLowPass::getParameterInternal(int index, float * value, char * valuestr)
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

FMOD_RESULT DSPITLowPass::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPITLowPass * itlowpass = (DSPITLowPass *)dsp;

    return itlowpass->createInternal();
}

FMOD_RESULT DSPITLowPass::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPITLowPass * itlowpass = (DSPITLowPass *)dsp;

    return itlowpass->resetInternal();
}

FMOD_RESULT DSPITLowPass::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPITLowPass * itlowpass = (DSPITLowPass *)dsp;

    return itlowpass->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPITLowPass::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPITLowPass * itlowpass = (DSPITLowPass *)dsp;

    return itlowpass->setParameterInternal(index, value);
}

FMOD_RESULT DSPITLowPass::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPITLowPass * itlowpass = (DSPITLowPass *)dsp;

    return itlowpass->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
