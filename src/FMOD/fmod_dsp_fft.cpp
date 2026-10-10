// G2MEAB fmod_dsp_fft.cpp: complete reconstruction (group D).
// .text: 0x805F5914..0x805F69AC (3 native functions: DSPFFT ctor, process, getSpectrum).
// The cosine/sine/bit-reverse helpers are inline and expanded in every caller.

#include "fmod_dsp_fft.h"
#include "fmod.h"
#include "fmod_msl_math.h"

#include <math.h>

namespace FMOD {

DSPFFT::DSPFFT()
{
    int count;

    for (count = 0; count < DSPFFT_COSTABSIZE; count++)
    {
        mCosTab[count] = (float)cos(1.5707964f * (float)count / (float)DSPFFT_COSTABSIZE);
    }
}

inline float DSPFFT::cosine(float x)
{
    int y;

    x *= DSPFFT_TABLERANGE;
    y = (int)x;
    if (y < 0)
    {
        y = -y;
    }

    y &= DSPFFT_TABLEMASK;

    switch (y >> DSPFFT_COSTABBITS)
    {
        case 0:
            return mCosTab[y];
        case 1:
            return -mCosTab[(DSPFFT_COSTABSIZE - 1) - (y - (DSPFFT_COSTABSIZE * 1))];
        case 2:
            return -mCosTab[y - (DSPFFT_COSTABSIZE * 2)];
        case 3:
            return mCosTab[(DSPFFT_COSTABSIZE - 1) - (y - (DSPFFT_COSTABSIZE * 3))];
    }

    return 0.0f;
}

inline float DSPFFT::sine(float x)
{
    return cosine(x - 0.25f);
}

inline unsigned int DSPFFT::reverse(unsigned int val, int bits)
{
    unsigned int retn = 0;

    while (bits--)
    {
        retn <<= 1;
        retn |= (val & 1);
        val >>= 1;
    }

    return retn;
}

FMOD_RESULT DSPFFT::process(int bits)
{
    int count, count2, count3;
    int i1, i2, i3, i4, y;
    int fftlen = 1 << bits;
    float a1, a2, b1, b2, z1, z2;
    float oneoverN = 1.0f / fftlen;

    i1 = fftlen / 2;
    i2 = 1;

    for (count = 0; count < bits; count++)
    {
        i3 = 0;
        i4 = i1;

        for (count2 = 0; count2 < i2; count2++)
        {
            y = reverse(i3 / i1, bits);
            z1 = cosine((float)y * oneoverN);
            z2 = -sine((float)y * oneoverN);

            for (count3 = i3; count3 < i4; count3++)
            {
                a1 = mFFTBuffer[count3].re;
                a2 = mFFTBuffer[count3].im;

                b1 = (z1 * mFFTBuffer[count3 + i1].re) - (z2 * mFFTBuffer[count3 + i1].im);
                b2 = (z2 * mFFTBuffer[count3 + i1].re) + (z1 * mFFTBuffer[count3 + i1].im);

                mFFTBuffer[count3].re = a1 + b1;
                mFFTBuffer[count3].im = a2 + b2;

                mFFTBuffer[count3 + i1].re = a1 - b1;
                mFFTBuffer[count3 + i1].im = a2 - b2;
            }

            i3 += (i1 << 1);
            i4 += (i1 << 1);
        }

        i1 >>= 1;
        i2 <<= 1;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPFFT::getSpectrum(float * pcmbuffer, unsigned int pcmposition, unsigned int pcmlength, float * spectrum, int length, int channel, int numchannels, FMOD_DSP_FFT_WINDOW windowtype)
{
    int count, bits, bitslength;

    bitslength = length;
    bits = 0;
    while (bitslength > 1)
    {
        bitslength >>= 1;
        bits++;
    }

    switch (windowtype)
    {
        case FMOD_DSP_FFT_WINDOW_TRIANGLE:
        {
            for (count = 0; count < length; count++)
            {
                float window = (2.0f * ((float)count / (float)length)) - 1.0f;

                if (window < 0.0f)
                {
                    window = -window;
                }
                window = 1.0f - window;

                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel] * window;
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
        case FMOD_DSP_FFT_WINDOW_HAMMING:
        {
            for (count = 0; count < length; count++)
            {
                float window = 0.54f - (0.46f * cosine((float)count / (float)length));

                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel] * window;
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
        case FMOD_DSP_FFT_WINDOW_HANNING:
        {
            for (count = 0; count < length; count++)
            {
                float window = 0.5f * (1.0f - cosine((float)count / (float)length));

                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel] * window;
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
        case FMOD_DSP_FFT_WINDOW_BLACKMAN:
        {
            for (count = 0; count < length; count++)
            {
                float n = (float)count / (float)length;
                float window = 0.42f - (0.5f * cosine(n)) + (0.08f * cosine(2.0f * n));

                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel] * window;
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
        case FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS:
        {
            for (count = 0; count < length; count++)
            {
                float n = (float)count / (float)length;
                float window = 0.35875f - (0.48829f * cosine(n)) + (0.14128f * cosine(2.0f * n)) - (0.01168f * cosine(3.0f * n));

                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel] * window;
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
        case FMOD_DSP_FFT_WINDOW_RECT:
        default:
        {
            for (count = 0; count < length; count++)
            {
                mFFTBuffer[count].re = pcmbuffer[(pcmposition * numchannels) + channel];
                mFFTBuffer[count].re /= (float)length;
                mFFTBuffer[count].im = 0.00000001f;

                pcmposition++;
                if (pcmposition >= pcmlength)
                {
                    pcmposition = 0;
                }
            }
            break;
        }
    }

    process(bits);

    for (count = 0; count < (length / 2) - 1; count++)
    {
        int index = reverse(count, bits);
        float magnitude = MSL_sqrtf((mFFTBuffer[index].re * mFFTBuffer[index].re) + (mFFTBuffer[index].im * mFFTBuffer[index].im)) * 2.5f;

        if (magnitude > 1.0f)
        {
            magnitude = 1.0f;
        }

        spectrum[count] = magnitude;
    }

    return FMOD_OK;
}

} // namespace FMOD
