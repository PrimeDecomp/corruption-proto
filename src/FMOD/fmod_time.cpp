// G2MEAB prototype NonMatching translation-unit scaffold; function bodies are empty placeholders.
// .text: 0x80621568..0x80621870 (7 native functions).
// Split out of the former fmod_systemi scaffold; original basename from the 4.06 reference library object.
// 0x80621568 +0x30: profiling state constructor emitted with system
// 0x80621598 +0x38: retained native; no unsupported symbol identity assigned
// 0x806215D0 +0x150: retained native; no unsupported symbol identity assigned
// 0x80621720 +0x20: retained native; no unsupported symbol identity assigned
// 0x80621740 +0xC4: retained native; no unsupported symbol identity assigned
// 0x80621804 +0x48: retained native; no unsupported symbol identity assigned
// 0x8062184C +0x24: thread sleep adapter to OS623228

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_time.h"
#include "fmod.h"
#include "fmod_types.h"

namespace FMOD {

TimeStamp::TimeStamp()
{
}

FMOD_RESULT TimeStamp::stampIn()
{
}

FMOD_RESULT TimeStamp::stampOut(int damppercentage)
{
}

FMOD_RESULT TimeStamp::getCPUUsage(FMOD_UFLOAT * cpuusage)
{
}

FMOD_RESULT TimeStamp::setPaused(bool paused)
{
}

} // namespace FMOD

FMOD_RESULT FMOD_Time_Get(unsigned int * ms)
{
}

FMOD_RESULT FMOD_Time_Sleep(unsigned int sleeptime)
{
}
