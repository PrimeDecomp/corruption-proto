// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x806262F4..0x806267E0 (12 native functions).
// Source identity: inferred from non-real-time no-sound output descriptor. Extent confidence: high.
// Complete native inventory retained, including callbacks and emitted helpers.
// Function bodies are empty placeholders from the 4.06 reference inventory.
// 0x806262F4 +0xA4: non-real-time no-sound output descriptor builder
// 0x80626398 +0x10: retained native; no unsupported symbol identity assigned
// 0x806263A8 +0x38: retained native; no unsupported symbol identity assigned
// 0x806263E0 +0x50: retained native; no unsupported symbol identity assigned
// 0x80626430 +0x21C: retained native; no unsupported symbol identity assigned
// 0x8062664C +0x3C: retained native; no unsupported symbol identity assigned
// 0x80626688 +0x2C: retained native; no unsupported symbol identity assigned
// 0x806266B4 +0x2C: retained native; no unsupported symbol identity assigned
// 0x806266E0 +0x2C: retained native; no unsupported symbol identity assigned
// 0x8062670C +0x34: retained native; no unsupported symbol identity assigned
// 0x80626740 +0x2C: retained native; no unsupported symbol identity assigned
// 0x8062676C +0x74: registered no-sound output static initializer

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_nosound_nrt.h"
#include "fmod.h"
#include "fmod_output.h"
#include "fmod_outputi.h"

namespace FMOD {

FMOD_OUTPUT_DESCRIPTION_EX nosoundoutput_nrt;

FMOD_OUTPUT_DESCRIPTION_EX * OutputNoSound_NRT::getDescriptionEx()
{
}

FMOD_RESULT OutputNoSound_NRT::getNumDrivers(int * numdrivers)
{
}

FMOD_RESULT OutputNoSound_NRT::getDriverName(int driver, char * name, int namelen)
{
}

FMOD_RESULT OutputNoSound_NRT::getDriverCaps(int id, FMOD_CAPS * caps)
{
}

FMOD_RESULT OutputNoSound_NRT::init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
}

FMOD_RESULT OutputNoSound_NRT::close()
{
}

FMOD_RESULT OutputNoSound_NRT::update()
{
}

FMOD_RESULT OutputNoSound_NRT::getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers)
{
}

FMOD_RESULT OutputNoSound_NRT::getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen)
{
}

FMOD_RESULT OutputNoSound_NRT::getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps)
{
}

FMOD_RESULT OutputNoSound_NRT::initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
}

FMOD_RESULT OutputNoSound_NRT::closeCallback(FMOD_OUTPUT_STATE * output)
{
}

FMOD_RESULT OutputNoSound_NRT::updateCallback(FMOD_OUTPUT_STATE * output)
{
}

} // namespace FMOD
