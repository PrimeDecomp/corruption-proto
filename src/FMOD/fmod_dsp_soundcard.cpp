// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x80606150..0x80606428 (4 retained native functions).
// directly named by target allocation/free body.
// Evidence: Setup06150 allocates native-output conversion buffer+124 except outputformat5, naming
// fmod_dsp_soundcard.cpp806EDDD8 line2D, then assigns graph order072E4. Release061FC frees same
// field line4F then calls Filter release569AC. Process06264 calls Filter569F0 then PCM
// converter626F40 when needed. Preserve all retained helpers, callback thunks, raw-only natives and
// inline expansions in target order.

// Reconstructed from a later FMOD Ex (Gormiti, Wii/MWCC) debug information. Member layout and offsets are the Gormiti reference, not yet verified against G2MEAB.

#include "fmod_dsp_soundcard.h"
#include "fmod.h"
#include "fmod_dspi.h"

namespace FMOD {

FMOD_RESULT DSPSoundCard::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
}

FMOD_RESULT DSPSoundCard::release(bool freethis)
{
}

FMOD_RESULT DSPSoundCard::execute(void * * outbuffer, unsigned int * length, int * outchannels, unsigned int tick)
{
}

} // namespace FMOD
