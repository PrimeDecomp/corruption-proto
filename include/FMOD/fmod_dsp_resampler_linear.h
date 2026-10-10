// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_DSP_RESAMPLER_LINEAR_H
#define _FMOD_DSP_RESAMPLER_LINEAR_H

union FMOD_SINT64P;
union FMOD_UINT64P;

#include "fmod.h"

#ifdef __cplusplus
extern "C" {
#endif

void FMOD_Resampler_Linear(float * out, int outlength, void * src, FMOD_SOUND_FORMAT srcformat, FMOD_UINT64P * position, FMOD_SINT64P * speed, int channels);

#ifdef __cplusplus
}
#endif

#endif
