// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x80612B54..0x80615750 (14 retained native functions).
// inferred descriptive source basename; original filename unverified.
// Evidence: Sample ctor12B54 derives SoundI16098, installs806EEF18, clears child count+348 and sets
// flag+378. Large lock12C48/unlock13F58 use child samples+34C, sample conversion and shared scratch
// buffer countersSystem+E20/+E24; clear151E0 releases this ownership and calls SoundI clear175A8.
// Preserve retained helpers, thunks and inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_sound_sample.h"
#include "fmod.h"

namespace FMOD {

Sample::Sample()
{
}

FMOD_RESULT Sample::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

FMOD_RESULT Sample::unlock(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
}

FMOD_RESULT Sample::release(bool freethis)
{
}

FMOD_RESULT Sample::setDefaults(float frequency, float volume, float pan, int priority)
{
}

FMOD_RESULT Sample::setVariations(float frequencyvar, float volumevar, float panvar)
{
}

FMOD_RESULT Sample::set3DMinMaxDistance(float min, float max)
{
}

FMOD_RESULT Sample::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
}

FMOD_RESULT Sample::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT Sample::setLoopCount(int loopcount)
{
}

FMOD_RESULT Sample::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
}

} // namespace FMOD
