// G2MEAB DSPFFT (group D). The 4.06 PS3 reference only keeps the table constants; the class shape follows
// the native code: complex work buffer at 0x0 (0x4000 entries), cosine table at 0x20000 (ctor 0x805F5914),
// in-place transform 0x805F59DC, windowed magnitude spectrum 0x805F5E04 (caller ChannelSoftware 0x805BA9EC).

#ifndef _FMOD_DSP_FFT_H
#define _FMOD_DSP_FFT_H

#include "fmod.h"

namespace FMOD {

const int DSPFFT_COSTABBITS = 13;
const int DSPFFT_COSTABSIZE = 8192;
const int DSPFFT_TABLERANGE = 32768;
const int DSPFFT_TABLEMASK = 32767;
const int DSPFFT_MAXBUFFERSIZE = 16384; // Guessed name; mCosTab follows at 0x20000

struct FMOD_COMPLEX // Guessed name
{
    float re; // offset 0x0
    float im; // offset 0x4
};

class DSPFFT
{
    FMOD_COMPLEX mFFTBuffer[DSPFFT_MAXBUFFERSIZE]; // offset 0x0, Guessed name
    float mCosTab[DSPFFT_COSTABSIZE]; // offset 0x20000, Guessed name

    inline float cosine(float x);
    inline float sine(float x);
    inline unsigned int reverse(unsigned int val, int bits);

public:
    DSPFFT();

    FMOD_RESULT process(int bits);
    FMOD_RESULT getSpectrum(float * pcmbuffer, unsigned int pcmposition, unsigned int pcmlength, float * spectrum, int length, int channel, int numchannels, FMOD_DSP_FFT_WINDOW windowtype);
};

} // namespace FMOD

#endif
