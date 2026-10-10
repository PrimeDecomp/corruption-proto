// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_GLOBALS_H
#define _FMOD_GLOBALS_H

#include "fmod.h"
#include "fmod_debug.h"

namespace FMOD {
    struct Global;
    class MemPool;
    struct SystemI;
}

namespace FMOD {

struct Global
{
    void init();
    Global();
    SystemI * gSystemHead; // offset 0x0
    MemPool * gSystemPool; // offset 0x4
    FMOD_DEBUGLEVEL gDebugLevel; // offset 0x8
    FMOD_DEBUGMODE gDebugMode; // offset 0xC
    unsigned int gDSPClock; // offset 0x10
    unsigned int gDSPClockTimeStamp; // offset 0x14
};

extern Global * gGlobal;
} // namespace FMOD

#endif
