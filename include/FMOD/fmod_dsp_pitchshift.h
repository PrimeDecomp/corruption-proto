// G2MEAB FFT pitch shifter effect. The 4.06 PS3 library ships no pitchshift debug information, so the class
// and method names follow the DSP<Type> / xxxInternal / xxxCallback pattern of the referenced effects and
// the smb* names of the S. M. Bernsee phase vocoder the per-channel worker follows (Guessed names).

#ifndef _FMOD_DSP_PITCHSHIFT_H
#define _FMOD_DSP_PITCHSHIFT_H

#include "fmod.h"
#include "fmod_dsp_filter.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

#define DSPPITCHSHIFT_MAXFRAMELENGTH 4096 // Guessed name (gInFIFO size, smbInit 0x805FC0F4)
#define DSPPITCHSHIFT_COSTABLESIZE 8192 // Guessed name (createInternal 0x805FC1C0)

const int DSPPITCHSHIFT_COSTABLEMASK = 0x7FFF; // Guessed name; loaded from .sdata2 0x807A3140 by the inlined cosine

// Guessed name. One per output channel, 0x2C018 bytes (setParameterInternal 0x805FD4CC allocation stride).
// Buffer offsets from smbInit 0x805FC0F4 and smbPitchShift 0x805FC340.
class DSPPitchShiftSMB
{
public:
    float gInFIFO[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x0
    float gOutFIFO[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x4000
    float gFFTworksp[2 * DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x8000
    float gLastPhase[DSPPITCHSHIFT_MAXFRAMELENGTH / 2 + 1]; // offset 0x10000
    float gSumPhase[DSPPITCHSHIFT_MAXFRAMELENGTH / 2 + 1]; // offset 0x12004
    float gOutputAccum[2 * DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x14008
    float gAnaFreq[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x1C008
    float gAnaMagn[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x20008
    float gSynFreq[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x24008
    float gSynMagn[DSPPITCHSHIFT_MAXFRAMELENGTH]; // offset 0x28008
    int gRover; // offset 0x2C008
    int mFFTFrameSize; // offset 0x2C00C, Guessed name (copied from the DSP by readInternal 0x805FD3CC)
    int mLog2FFTFrameSize; // offset 0x2C010, Guessed name (copied from the DSP by readInternal)
    float * mCosTab; // offset 0x2C014, Guessed name (the owning DSP's table, resetInternal 0x805FD338)

    // Guessed name. Inlined into smbFft and smbPitchShift: a cosine of x cycles read from the quarter-wave
    // table, 32768 steps per cycle.
    inline float cosine(float x)
    {
        int index = (int)(x * 32768.0f);

        if (index < 0)
        {
            index = -index;
        }
        index &= DSPPITCHSHIFT_COSTABLEMASK;

        switch (index >> 13)
        {
            case 0:
            {
                return mCosTab[index];
            }
            case 1:
            {
                return -mCosTab[(DSPPITCHSHIFT_COSTABLESIZE - 1) - (index - 8192)];
            }
            case 2:
            {
                return -mCosTab[index - 16384];
            }
            case 3:
            {
                return mCosTab[(DSPPITCHSHIFT_COSTABLESIZE - 1) - (index - 24576)];
            }
        }

        return 0.0f;
    }

    inline float sine(float x)
    {
        return cosine(x - 0.25f);
    }

    void smbInit();
    void smbPitchShift(float pitchShift, int numSampsToProcess, int osamp, float sampleRate, float * indata, float * outdata, int channel, int numchannels);
    void smbFft(float * fftBuffer, int sign);
    float smbAtan2(float x, float y);
};

// Guessed name. Size 0x8144 (getDescriptionEx 0x805FC024). Parameter offsets from setParameterInternal
// 0x805FD4CC and getParameterInternal 0x805FD748.
class DSPPitchShift : public DSPFilter
{
    float mCosTab[DSPPITCHSHIFT_COSTABLESIZE]; // offset 0x124, Guessed name (quarter cosine wave)
    float mPitch; // offset 0x8124, Guessed name (param 0)
    int mFFTSize; // offset 0x8128, Guessed name (param 1, rounded up to a power of two)
    int mOverlap; // offset 0x812C, Guessed name (param 2)
    int mMaxChannels; // offset 0x8130, Guessed name (param 3)
    DSPPitchShiftSMB * mPitchShift; // offset 0x8134, Guessed name (mChannels workers)
    int mOutputRate; // offset 0x8138, Guessed name (getSoftwareFormat samplerate)
    int mChannels; // offset 0x813C, Guessed name (getSoftwareFormat numoutputchannels or mMaxChannels)
    int mLog2FFTSize; // offset 0x8140, Guessed name

public:
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT releaseInternal();
    FMOD_RESULT resetInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT releaseCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT resetCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
