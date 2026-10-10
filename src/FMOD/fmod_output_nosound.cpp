// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x8060F1B8..0x8060F80C (16 retained native functions).
// direct target filename in allocation/free body.
// Evidence: F1B8 fills shared descriptor80755E3C named FMOD NoSound Output and installs seven
// callbacks. F274/F284/F2BC report one NoSound Driver and capabilities; F30C/F500 allocate/free
// buffer using fmod_output_nosound.cpp806EE73B lines0xB9/0xDB. Shared buffer fields+204/+208 and
// clock-position/lock operations close the same output. Preserve retained helpers, thunks and
// inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_nosound.h"
#include "fmod.h"
#include "fmod_output.h"
#include "fmod_outputi.h"

namespace FMOD {

FMOD_OUTPUT_DESCRIPTION_EX nosoundoutput;

FMOD_OUTPUT_DESCRIPTION_EX * OutputNoSound::getDescriptionEx()
{
}

FMOD_RESULT OutputNoSound::getNumDrivers(int * numdrivers)
{
}

FMOD_RESULT OutputNoSound::getDriverName(int driver, char * name, int namelen)
{
}

FMOD_RESULT OutputNoSound::getDriverCaps(int id, FMOD_CAPS * caps)
{
}

FMOD_RESULT OutputNoSound::init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
}

FMOD_RESULT OutputNoSound::close()
{
}

FMOD_RESULT OutputNoSound::getPosition(unsigned int * pcm)
{
}

FMOD_RESULT OutputNoSound::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

FMOD_RESULT OutputNoSound::getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers)
{
}

FMOD_RESULT OutputNoSound::getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen)
{
}

FMOD_RESULT OutputNoSound::getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps)
{
}

FMOD_RESULT OutputNoSound::initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
}

FMOD_RESULT OutputNoSound::closeCallback(FMOD_OUTPUT_STATE * output)
{
}

FMOD_RESULT OutputNoSound::getPositionCallback(FMOD_OUTPUT_STATE * output, unsigned int * pcm)
{
}

FMOD_RESULT OutputNoSound::lockCallback(FMOD_OUTPUT_STATE * output, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
}

} // namespace FMOD
