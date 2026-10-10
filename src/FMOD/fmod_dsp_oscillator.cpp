// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805FAED8..0x805FB6F0 (12 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the oscillator; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects (description builder, xxxInternal methods, static callbacks).

#include "fmod_dsp_oscillator.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsposcillator;

FMOD_DSP_PARAMETERDESC dsposcillator_param[2] =
{
    { 0.0f, 5.0f, 0.0f, "Oscillator type", "", "Select a waveform type" },
    { 1.0f, 22000.0f, 220.0f, "Frequency", "hz", "Playback frequency of tone, for example music note A above middle C is 440.0." }
};

FMOD_DSP_DESCRIPTION_EX * DSPOscillator::getDescriptionEx()
{
    memset(&dsposcillator, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsposcillator.name, "FMOD Oscillator");
    dsposcillator.version = 0x00010100;
    dsposcillator.channels = 1;
    dsposcillator.create = DSPOscillator::createCallback;
    dsposcillator.release = DSPOscillator::releaseCallback;
    dsposcillator.read = DSPOscillator::readCallback;

    dsposcillator.numparameters = sizeof(dsposcillator_param) / sizeof(dsposcillator_param[0]);
    dsposcillator.paramdesc = dsposcillator_param;
    dsposcillator.setparameter = DSPOscillator::setParameterCallback;
    dsposcillator.getparameter = DSPOscillator::getParameterCallback;

    dsposcillator.mType = FMOD_DSP_TYPE_OSCILLATOR;
    dsposcillator.mSize = sizeof(DSPOscillator);
    dsposcillator.mCategory = FMOD_DSP_CATEGORY_FILTER;

    return &dsposcillator;
}

FMOD_RESULT DSPOscillator::createInternal()
{
    int count;

    init();

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPOscillator::releaseInternal()
{
    return FMOD_OK;
}

FMOD_RESULT DSPOscillator::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    unsigned int count;

    switch (mType)
    {
        case 0:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = (float)sin(mPosition);

                mPosition += 6.2831855f * mRate;
                if (mPosition >= 6.2831855f)
                {
                    mPosition -= 6.2831855f;
                }
            }
            break;
        }
        case 1:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = (float)mDirection;

                mPosition += mRate;
                if (mPosition >= 1.0f)
                {
                    mPosition -= 1.0f;
                    mDirection = -mDirection;
                }
            }
            break;
        }
        case 2:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = 2.0f * mPosition - 1.0f;

                mPosition += mRate;
                if (mPosition >= 1.0f)
                {
                    mPosition -= 1.0f;
                }
            }
            break;
        }
        case 3:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = 1.0f + -2.0f * mPosition;

                mPosition += mRate;
                if (mPosition >= 1.0f)
                {
                    mPosition -= 1.0f;
                }
            }
            break;
        }
        case 4:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = mPosition;

                mPosition += 2.0f * (mRate * (float)mDirection);
                if (mPosition > 1.0f || mPosition < -1.0f)
                {
                    mDirection = -mDirection;
                    mPosition += mRate * (float)mDirection;
                    mPosition += mRate * (float)mDirection;
                }
            }
            break;
        }
        case 5:
        {
            for (count = 0; count < length; count++)
            {
                outbuffer[count] = ((float)(rand() % 32768) / 16384.0f) - 1.0f;
            }
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPOscillator::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int outputrate;

    switch (index)
    {
        case 0:
        {
            mType = (int)value;
            break;
        }
        case 1:
        {
            mFrequency = value;
            break;
        }
    }

    mDirection = 1;

    result = mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mRate = mFrequency / (float)outputrate;

    return FMOD_OK;
}

FMOD_RESULT DSPOscillator::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            *value = (float)mType;
            switch (mType)
            {
                case 0:
                {
                    FMOD_strcpy(valuestr, "sine");
                    break;
                }
                case 1:
                {
                    FMOD_strcpy(valuestr, "square");
                    break;
                }
                case 2:
                {
                    FMOD_strcpy(valuestr, "saw up");
                    break;
                }
                case 3:
                {
                    FMOD_strcpy(valuestr, "saw down");
                    break;
                }
                case 4:
                {
                    FMOD_strcpy(valuestr, "triangle");
                    break;
                }
                case 5:
                {
                    FMOD_strcpy(valuestr, "noise");
                    break;
                }
            }
            break;
        }
        case 1:
        {
            *value = mFrequency;
            sprintf(valuestr, "%.02f", mFrequency);
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPOscillator::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPOscillator * oscillator = (DSPOscillator *)dsp;

    return oscillator->createInternal();
}

FMOD_RESULT DSPOscillator::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPOscillator * oscillator = (DSPOscillator *)dsp;

    return oscillator->releaseInternal();
}

FMOD_RESULT DSPOscillator::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPOscillator * oscillator = (DSPOscillator *)dsp;

    return oscillator->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPOscillator::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPOscillator * oscillator = (DSPOscillator *)dsp;

    return oscillator->setParameterInternal(index, value);
}

FMOD_RESULT DSPOscillator::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPOscillator * oscillator = (DSPOscillator *)dsp;

    return oscillator->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
