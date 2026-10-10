// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x8060B08C..0x8060B560 (4 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: Static initializer0B08C initializes FMOD::gGlobalMem using MemPool ctor0B6A8,constructs
// global SystemI807542DC via1E5B4 and registers destructor0B0D4 with node807542D0. Complete SystemI
// destructor0B0D4 restores806EE00C, releases owned scratch buffer+E20/+E24, lists and embedded
// Geometry manager+FC8; keep emitted nested destructor0B4A8 for table806EE000. Preserve all
// retained helpers, callback thunks, raw-only natives and inline expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_globals.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

namespace FMOD {

static Global gGlobalMem;
Global * gGlobal;
static MemPool gSystemPoolMem;
static SystemI gSystemHeadMem;

void Global::init()
{
}

} // namespace FMOD
