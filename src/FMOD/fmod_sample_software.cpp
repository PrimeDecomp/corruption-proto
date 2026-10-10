// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x80611C3C..0x80612998 (8 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Ctor11C3C calls Sample base12B54, installs806EECE4 and clears PCM storage+37C/+380.
// Clear11D3C names fmod_sample_software.cpp806EEDA0 line0x4F and calls retained base
// clear151E0;11DE4/11EDC lock/unlock wrappers use full PCM/interleave conversion
// helpers12514/11F0C. Preserve retained helpers, thunks and inline expansions in observed native
// order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_sample_software.h"
#include "fmod.h"

namespace FMOD {

SampleSoftware::SampleSoftware()
{
}

FMOD_RESULT SampleSoftware::release(bool freethis)
{
}

FMOD_RESULT SampleSoftware::setLoopPointData()
{
}

FMOD_RESULT SampleSoftware::setMode(FMOD_MODE mode)
{
}

FMOD_RESULT SampleSoftware::restoreLoopPointData()
{
}

FMOD_RESULT SampleSoftware::lockInternal(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

FMOD_RESULT SampleSoftware::unlockInternal(void * ptr1, void * ptr2, unsigned int len1, unsigned int len2)
{
}

FMOD_RESULT SampleSoftware::setBufferData(void * data)
{
}

FMOD_RESULT SampleSoftware::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
}

} // namespace FMOD
