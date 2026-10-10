// G2MEAB globals. This FMOD build predates the 4.06 Global struct: the system list head and the
// memory pool are two separate .sdata pointers, statically initialized to the objects defined in
// fmod_globals.cpp (gSystemHead 0x80796F08 = &0x807542DC, gSystemPool 0x80796F0C = &0x80754290).
// Codecs copy them from their Plugin base (+0x18/+0x1C) before working (e.g. 0x805C2754).

#ifndef _FMOD_GLOBALS_H
#define _FMOD_GLOBALS_H

#include "fmod.h"

namespace FMOD {
    class MemPool;
    struct SystemI;
}

namespace FMOD {

extern SystemI * gSystemHead;
extern MemPool * gSystemPool;

} // namespace FMOD

#endif
