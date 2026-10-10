// G2MEAB prototype translation unit; complete reconstruction (NonMatching).
// .text: 0x805FC024..0x805FDA60 (18 native functions).
// The descriptor and parameter table live in the auto .data/.bss splits, not in this object.

// No 4.06 reference object exists for the pitch shifter; reconstructed from the G2MEAB assembly using the
// structure of the referenced 4.06 effects. The per-channel worker follows S. M. Bernsee's smbPitchShift
// phase vocoder, adapted to float math, the shared cosine table and interleaved buffers.

#include "fmod_dsp_pitchshift.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

namespace FMOD {

FMOD_DSP_DESCRIPTION_EX dsppitchshift;

FMOD_DSP_PARAMETERDESC dsppitchshift_param[4] =
{
    { 0.5f, 2.0f, 1.0f, "Pitch", "x", "Pitch value.  0.5 to 2.0.  Default = 1.0. 0.5 = one octave down, 2.0 = one octave up.  1.0 does not change the pitch." },
    { 256.0f, 4096.0f, 1024.0f, "FFT size", "", "FFT window size.  256, 512, 1024, 2048, 4096.  Default = 1024.  Increase this to reduce 'smearing'.  This effect is a warbling sound similar to when an mp3 is encoded at very low bitrates." },
    { 1.0f, 32.0f, 4.0f, "Overlap", "", "Window overlap.  1 to 32.  Default = 4.  Increase this to reduce 'tremolo' effect.  Increasing it by a factor of 2 doubles the CPU usage." },
    { 0.0f, 16.0f, 0.0f, "Max channels", "channels", "Maximum channels supported.  0 to 16.  0 = same as fmod's default output polyphony, 1 = mono, 2 = stereo etc.  See remarks for more.  Default = 0.  It is suggested to leave at 0!" }
};

FMOD_DSP_DESCRIPTION_EX * DSPPitchShift::getDescriptionEx()
{
    memset(&dsppitchshift, 0, sizeof(FMOD_DSP_DESCRIPTION_EX));

    FMOD_strcpy(dsppitchshift.name, "FMOD Pitch Shifter");
    dsppitchshift.version = 0x00010100;
    dsppitchshift.create = DSPPitchShift::createCallback;
    dsppitchshift.release = DSPPitchShift::releaseCallback;
    dsppitchshift.reset = DSPPitchShift::resetCallback;
    dsppitchshift.read = DSPPitchShift::readCallback;

    dsppitchshift.numparameters = sizeof(dsppitchshift_param) / sizeof(dsppitchshift_param[0]);
    dsppitchshift.paramdesc = dsppitchshift_param;
    dsppitchshift.setparameter = DSPPitchShift::setParameterCallback;
    dsppitchshift.getparameter = DSPPitchShift::getParameterCallback;

    dsppitchshift.mType = FMOD_DSP_TYPE_PITCHSHIFT;
    dsppitchshift.mCategory = FMOD_DSP_CATEGORY_FILTER;
    dsppitchshift.mSize = sizeof(DSPPitchShift);

    return &dsppitchshift;
}

void DSPPitchShiftSMB::smbInit()
{
    memset(gInFIFO, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    memset(gOutFIFO, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    memset(gFFTworksp, 0, 2 * DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    memset(gLastPhase, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float) / 2);
    memset(gSumPhase, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float) / 2);
    memset(gOutputAccum, 0, 2 * DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    memset(gAnaFreq, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    memset(gAnaMagn, 0, DSPPITCHSHIFT_MAXFRAMELENGTH * sizeof(float));
    gRover = 0;
}

FMOD_RESULT DSPPitchShift::createInternal()
{
    int count;

    init();

    for (count = 0; count < DSPPITCHSHIFT_COSTABLESIZE; count++)
    {
        mCosTab[count] = (float)cos(1.5707963268f * (float)count / (float)DSPPITCHSHIFT_COSTABLESIZE);
    }

    mPitchShift = 0;

    for (count = 0; count < mDescription.numparameters; count++)
    {
        setParameter(count, mDescription.paramdesc[count].defaultval);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPPitchShift::releaseInternal()
{
    if (mPitchShift)
    {
        FMOD_Memory_Free(mPitchShift);
        mPitchShift = 0;
    }

    return FMOD_OK;
}

void DSPPitchShiftSMB::smbPitchShift(float pitchShift, int numSampsToProcess, int osamp, float sampleRate, float * indata, float * outdata, int channel, int numchannels)
{
    float magn, phase, tmp, window, real, imag;
    float freqPerBin, expct;
    int i, k, qpd, index, inFifoLatency, stepSize, fftFrameSize2;

    fftFrameSize2 = mFFTFrameSize / 2;
    stepSize = mFFTFrameSize / osamp;
    freqPerBin = sampleRate / (float)mFFTFrameSize;
    expct = 6.2831855f * (float)stepSize / (float)mFFTFrameSize;
    inFifoLatency = mFFTFrameSize - stepSize;

    if (!gRover)
    {
        gRover = inFifoLatency;
    }

    for (i = 0; i < numSampsToProcess; i++)
    {
        gInFIFO[gRover] = indata[(i * numchannels) + channel];
        outdata[(i * numchannels) + channel] = gOutFIFO[gRover - inFifoLatency];
        gRover++;

        if (gRover >= mFFTFrameSize)
        {
            gRover = inFifoLatency;

            for (k = 0; k < mFFTFrameSize; k++)
            {
                window = -0.5f * cosine((float)k / (float)mFFTFrameSize) + 0.5f;
                gFFTworksp[2 * k] = window * gInFIFO[k];
                gFFTworksp[2 * k + 1] = 0.0f;
            }

            smbFft(gFFTworksp, -1);

            for (k = 0; k <= fftFrameSize2; k++)
            {
                real = gFFTworksp[2 * k];
                imag = gFFTworksp[2 * k + 1];

                FMOD_VECTOR v = { 0.0f, 0.0f, 0.0f };
                v.x = real;
                v.y = imag;
                magn = 2.0f * sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
                phase = smbAtan2(imag, real);

                tmp = phase - gLastPhase[k];
                gLastPhase[k] = phase;

                tmp -= (float)k * expct;

                qpd = (int)(tmp / 3.1415927f);
                if (qpd >= 0)
                {
                    qpd += qpd & 1;
                }
                else
                {
                    qpd -= qpd & 1;
                }
                tmp -= 3.1415927f * (float)qpd;

                tmp = (float)osamp * tmp / 6.2831855f;
                tmp = (float)k * freqPerBin + tmp * freqPerBin;

                gAnaMagn[k] = magn;
                gAnaFreq[k] = tmp;
            }

            memset(gSynMagn, 0, mFFTFrameSize * sizeof(float));
            memset(gSynFreq, 0, mFFTFrameSize * sizeof(float));

            for (k = 0; k <= fftFrameSize2; k++)
            {
                index = (int)((float)k / pitchShift);
                if (index <= fftFrameSize2)
                {
                    gSynMagn[k] += gAnaMagn[index];
                    gSynFreq[k] = gAnaFreq[index] * pitchShift;
                }
            }

            for (k = 0; k <= fftFrameSize2; k++)
            {
                magn = gSynMagn[k];
                tmp = gSynFreq[k];

                tmp -= (float)k * freqPerBin;
                tmp /= freqPerBin;
                tmp = 6.2831855f * tmp / (float)osamp;
                tmp += (float)k * expct;

                gSumPhase[k] += tmp;
                phase = gSumPhase[k] / 6.2831855f;

                gFFTworksp[2 * k] = magn * cosine(phase);
                gFFTworksp[2 * k + 1] = magn * sine(phase);
            }

            for (k = mFFTFrameSize + 2; k < 2 * mFFTFrameSize; k++)
            {
                gFFTworksp[k] = 0.0f;
            }

            smbFft(gFFTworksp, 1);

            for (k = 0; k < mFFTFrameSize; k++)
            {
                window = -0.5f * cosine((float)k / (float)mFFTFrameSize) + 0.5f;
                gOutputAccum[k] += 2.0f * window * gFFTworksp[2 * k] / (float)(fftFrameSize2 * osamp);
            }

            for (k = 0; k < stepSize; k++)
            {
                gOutFIFO[k] = gOutputAccum[k];
            }

            memmove(gOutputAccum, gOutputAccum + stepSize, mFFTFrameSize * sizeof(float));

            for (k = 0; k < inFifoLatency; k++)
            {
                gInFIFO[k] = gInFIFO[k + stepSize];
            }
        }
    }
}

void DSPPitchShiftSMB::smbFft(float * fftBuffer, int sign)
{
    float wr, wi, arg, * p1, * p2, temp;
    float tr, ti, ur, ui, * p1r, * p1i, * p2r, * p2i;
    int i, bitm, j, le, le2, k;

    for (i = 2; i < 2 * mFFTFrameSize - 2; i += 2)
    {
        for (bitm = 2, j = 0; bitm < 2 * mFFTFrameSize; bitm <<= 1)
        {
            if (i & bitm)
            {
                j++;
            }
            j <<= 1;
        }

        if (i < j)
        {
            p1 = fftBuffer + i;
            p2 = fftBuffer + j;
            temp = *p1;
            *(p1++) = *p2;
            *(p2++) = temp;
            temp = *p1;
            *p1 = *p2;
            *p2 = temp;
        }
    }

    for (k = 0, le = 2; k < mLog2FFTFrameSize; k++)
    {
        le <<= 1;
        le2 = le >> 1;
        ur = 1.0f;
        ui = 0.0f;
        arg = 3.1415927f / (float)(le2 >> 1) / 6.2831855f;
        wr = cosine(arg);
        wi = (float)sign * sine(arg);

        for (j = 0; j < le2; j += 2)
        {
            p1r = fftBuffer + j;
            p1i = p1r + 1;
            p2r = p1r + le2;
            p2i = p2r + 1;

            for (i = j; i < 2 * mFFTFrameSize; i += le)
            {
                tr = *p2r * ur - *p2i * ui;
                ti = *p2r * ui + *p2i * ur;
                *p2r = *p1r - tr;
                *p2i = *p1i - ti;
                *p1r += tr;
                *p1i += ti;
                p1r += le;
                p1i += le;
                p2r += le;
                p2i += le;
            }

            tr = ur * wr - ui * wi;
            ui = ur * wi + ui * wr;
            ur = tr;
        }
    }
}

float DSPPitchShiftSMB::smbAtan2(float x, float y)
{
    float signx;

    if (x > 0.0f)
    {
        signx = 1.0f;
    }
    else
    {
        signx = -1.0f;
    }

    if (x == 0.0f)
    {
        return 0.0f;
    }
    if (y == 0.0f)
    {
        return 3.1415927f * signx / 2.0f;
    }

    return (float)atan2(x, y);
}

FMOD_RESULT DSPPitchShift::resetInternal()
{
    int count;

    if (mPitchShift)
    {
        for (count = 0; count < mChannels; count++)
        {
            mPitchShift[count].smbInit();
            mPitchShift[count].mCosTab = mCosTab;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPPitchShift::readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    int count;

    if (inchannels > mChannels)
    {
        memcpy(outbuffer, inbuffer, length * outchannels * sizeof(float));
        return FMOD_OK;
    }

    for (count = 0; count < inchannels; count++)
    {
        mPitchShift[count].mFFTFrameSize = mFFTSize;
        mPitchShift[count].mLog2FFTFrameSize = mLog2FFTSize;
        mPitchShift[count].smbPitchShift(mPitch, length, mOverlap, (float)mOutputRate, inbuffer, outbuffer, count, inchannels);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPPitchShift::setParameterInternal(int index, float value)
{
    FMOD_RESULT result;
    int oldchannels = mChannels;
    int oldfftsize = mFFTSize;
    int count;

    switch (index)
    {
        case 0:
        {
            mPitch = value;
            break;
        }
        case 1:
        {
            mFFTSize = (int)value;
            break;
        }
        case 2:
        {
            mOverlap = (int)value;
            break;
        }
        case 3:
        {
            mMaxChannels = (int)value;
            break;
        }
    }

    if (mFFTSize <= 256)
    {
        mFFTSize = 256;
    }
    else if (mFFTSize <= 512)
    {
        mFFTSize = 512;
    }
    else if (mFFTSize <= 1024)
    {
        mFFTSize = 1024;
    }
    else if (mFFTSize <= 2048)
    {
        mFFTSize = 2048;
    }
    else
    {
        mFFTSize = 4096;
    }

    result = mSystem->getSoftwareFormat(&mOutputRate, 0, &mChannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mMaxChannels)
    {
        mChannels = mMaxChannels;
    }

    if (oldfftsize != mFFTSize)
    {
        reset();
    }

    if (mChannels != oldchannels || !mPitchShift)
    {
        if (mPitchShift)
        {
            FMOD_Memory_Free(mPitchShift);
        }

        mPitchShift = (DSPPitchShiftSMB *)FMOD_Memory_Alloc(mChannels * sizeof(DSPPitchShiftSMB));
        if (!mPitchShift)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < mChannels; count++)
        {
            mPitchShift[count].smbInit();
            mPitchShift[count].mCosTab = mCosTab;
        }
    }

    count = mFFTSize;
    mLog2FFTSize = 0;
    while (count > 1)
    {
        count >>= 1;
        mLog2FFTSize++;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPPitchShift::getParameterInternal(int index, float * value, char * valuestr)
{
    switch (index)
    {
        case 0:
        {
            if (value)
            {
                *value = mPitch;
            }
            if (valuestr)
            {
                sprintf(valuestr, "%.02f", mPitch);
            }
            break;
        }
        case 1:
        {
            if (value)
            {
                *value = (float)mFFTSize;
            }
            if (valuestr)
            {
                sprintf(valuestr, "%d", mFFTSize);
            }
            break;
        }
        case 2:
        {
            if (value)
            {
                *value = (float)mOverlap;
            }
            if (valuestr)
            {
                sprintf(valuestr, "%d", mOverlap);
            }
            break;
        }
        case 3:
        {
            if (value)
            {
                *value = (float)mMaxChannels;
            }
            if (valuestr)
            {
                sprintf(valuestr, "%d", mMaxChannels);
            }
            break;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT DSPPitchShift::createCallback(FMOD_DSP_STATE * dsp)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->createInternal();
}

FMOD_RESULT DSPPitchShift::releaseCallback(FMOD_DSP_STATE * dsp)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->releaseInternal();
}

FMOD_RESULT DSPPitchShift::resetCallback(FMOD_DSP_STATE * dsp)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->resetInternal();
}

FMOD_RESULT DSPPitchShift::readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->readInternal(inbuffer, outbuffer, length, inchannels, outchannels);
}

FMOD_RESULT DSPPitchShift::setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->setParameterInternal(index, value);
}

FMOD_RESULT DSPPitchShift::getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr)
{
    DSPPitchShift * pitchshift = (DSPPitchShift *)dsp;

    return pitchshift->getParameterInternal(index, value, valuestr);
}

} // namespace FMOD
