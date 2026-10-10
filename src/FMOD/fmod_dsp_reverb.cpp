// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x80605958..0x80606150 (14 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for this reverb; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects. The processing is the Freeverb revmodel (revmodel.cpp).

#include "fmod_dsp_reverb.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dspreverb;

FMOD_DSP_PARAMETERDESC dspreverb_param[6] =
{
    { 0.0f, 1.0f, 0.5f, "Roomsize", "", "Roomsize.  0.0 to 1.0.  Default = 0.5" },
    { 0.0f, 1.0f, 0.5f, "Damp", "", "Damp.  0.0 to 1.0.  Default = 0.5" },
    { 0.0f, 1.0f, 0.33f, "Wet", "", "Wet mix.  0.0 to 1.0.  Default = 0.33" },
    { 0.0f, 1.0f, 0.66f, "Dry", "", "Dry mix.  0.0 to 1.0.  Default = 0.66" },
    { 0.0f, 1.0f, 1.0f, "Width", "", "Width.  0.0 to 1.0.  Default = 1.0" },
    { 0.0f, 1.0f, 0.0f, "Mode", "", "Mode.  0, 1.  Default = 0" }
};

FMOD_DSP_DESCRIPTION_EX * DSPReverb::getDescriptionEx()
{
    memset(&dspreverb, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dspreverb.name, "FMOD Reverb");
    dspreverb.version = 0x00010100;
    dspreverb.create = DSPReverb::createCallback;
    dspreverb.release = DSPReverb::releaseCallback;
    dspreverb.reset = DSPReverb::resetCallback;
    dspreverb.read = DSPReverb::readCallback;

    dspreverb.numparameters = sizeof(dspreverb_param) / sizeof(dspreverb_param[0]);
    dspreverb.paramdesc = dspreverb_param;
    dspreverb.setparameter = DSPReverb::setParameterCallback;
    dspreverb.getparameter = DSPReverb::getParameterCallback;

    dspreverb.mType = FMOD_DSP_TYPE_REVERB;
    dspreverb.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dspreverb.mSize = sizeof(DSPReverb);
    dspreverb.mModule = 0;

    return &dspreverb;
}

FMOD_RESULT DSPReverb::createInternal()
{
    int count;

    init();

    new (&mReverb) revmodel;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPReverb::releaseInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPReverb::resetInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPReverb::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    if (inchannels > 2)
    {
        FMOD_SOUND_FORMAT format;
        unsigned int bytes;

        mSystem->getSoftwareFormat(0, &format, 0, 0, 0, 0);
        SoundI::getBytesFromSamples(length, &bytes, inchannels, format);

        memcpy(outbuffer, inbuffer, bytes);
    }

    mReverb.processreplace(inbuffer, inbuffer + 1, outbuffer, outbuffer + 1, length, inchannels);

    return FMOD_OK;
}

FMOD_RESULT DSPReverb::setParameterInternal(int index, float value)
{
    switch (index)
    {
        case 0:
        {
            if (value > 1.0f)
            {
                value = 1.0f;
            }
            else if (value < 0.0f)
            {
                value = 0.0f;
            }
            mReverb.setroomsize(value);
            break;
        }
        case 1:
        {
            if (value > 1.0f)
            {
                value = 1.0f;
            }
            else if (value < 0.0f)
            {
                value = 0.0f;
            }
            mReverb.setdamp(value);
            break;
        }
        case 2:
        {
            if (value > 1.0f)
            {
                value = 1.0f;
            }
            else if (value < 0.0f)
            {
                value = 0.0f;
            }
            mReverb.setwet(value);
            break;
        }
        case 3:
        {
            if (value > 1.0f)
            {
                value = 1.0f;
            }
            else if (value < 0.0f)
            {
                value = 0.0f;
            }
            mReverb.setdry(value);
            break;
        }
        case 4:
        {
            if (value > 1.0f)
            {
                value = 1.0f;
            }
            else if (value < 0.0f)
            {
                value = 0.0f;
            }
            mReverb.setwidth(value);
            break;
        }
        case 5:
        {
            mReverb.setmode(value >= 0.5f ? 1.0f : 0.0f);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPReverb::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = mReverb.getroomsize();
            sprintf(valuestr, "%0.2f", *value);
            break;
        }
        case 1:
        {
            *value = mReverb.getdamp();
            sprintf(valuestr, "%0.2f", *value);
            break;
        }
        case 2:
        {
            *value = mReverb.getwet();
            sprintf(valuestr, "%0.2f", *value);
            break;
        }
        case 3:
        {
            *value = mReverb.getdry();
            sprintf(valuestr, "%0.2f", *value);
            break;
        }
        case 4:
        {
            *value = mReverb.getwidth();
            sprintf(valuestr, "%0.2f", *value);
            break;
        }
        case 5:
        {
            *value = mReverb.getmode();
            if (*value >= 0.5f)
            {
                *value = 1.0f;
                sprintf(valuestr, "FREEZE");
            }
            else
            {
                *value = 0.0f;
                sprintf(valuestr, "NORMAL");
            }
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPReverb::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->createInternal();
}

FMOD_RESULT DSPReverb::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->releaseInternal();
}

FMOD_RESULT DSPReverb::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->resetInternal();
}

FMOD_RESULT DSPReverb::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPReverb::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->setParameterInternal(index, value);
}

FMOD_RESULT DSPReverb::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPReverb * reverb = (DSPReverb *)dsp;

    return reverb->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
