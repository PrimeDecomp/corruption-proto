// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_OS_OUTPUT_H
#define _FMOD_OS_OUTPUT_H

namespace FMOD {
    class PluginFactory;
}

#include "fmod.h"

FMOD_RESULT FMOD_OS_Output_Register(FMOD::PluginFactory * pluginfactory);
FMOD_RESULT FMOD_OS_Output_GetDefault(FMOD_OUTPUTTYPE * outputtype);

#endif
