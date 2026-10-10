// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_DSP_COMPRESSOR_H
#define _FMOD_DSP_COMPRESSOR_H

#include "fmod.h"
#include "fmod_dspi.h"

struct FMOD_DSP_STATE;
namespace FMOD {
    struct FMOD_DSP_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_dsp_compressor.cpp: the 4.06 DWARF has no type entry,
// so the base class is guessed and members are unknown.
struct DSPCompressor : public DSPI
{
    static FMOD_DSP_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT createInternal();
    FMOD_RESULT readInternal(float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    FMOD_RESULT setParameterInternal(int index, float value);
    FMOD_RESULT getParameterInternal(int index, float * value, char * valuestr);
    static FMOD_RESULT createCallback(FMOD_DSP_STATE * dsp);
    static FMOD_RESULT readCallback(FMOD_DSP_STATE * dsp, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int outchannels);
    static FMOD_RESULT setParameterCallback(FMOD_DSP_STATE * dsp, int index, float value);
    static FMOD_RESULT getParameterCallback(FMOD_DSP_STATE * dsp, int index, float * value, char * valuestr);
};

} // namespace FMOD

#endif
