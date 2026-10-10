// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805B6C3C..0x805B6E70 (4 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Ctor805B6C3C calls base805B6E70 and installs vtable806E25F0; external emulated-output
// initializer8060EF40 allocates param2*0x78 bytes and constructs these voices, independently
// proving emulated family identity. Vtable differs from base at advance slot+24 (805B6C78) and
// capability slot+80 (805B6E48). Retained805B6E68 is shared by other low-level vtables, not
// discarded as generic li0/blr. Final8-byte native ends805B6E70; next begins shared base
// constructor installing806E270C. Preserve every retained stub, emitted helper and adjustor thunk;
// full inventory and inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_channel_emulated.h"
#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_outputi.h"
#include "fmod_systemi.h"

namespace FMOD {

ChannelEmulated::ChannelEmulated()
{
}

FMOD_RESULT ChannelEmulated::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
}

FMOD_RESULT ChannelEmulated::alloc()
{
}

FMOD_RESULT ChannelEmulated::close()
{
}

FMOD_RESULT ChannelEmulated::update(int delta)
{
}

FMOD_RESULT ChannelEmulated::isVirtual(bool * isvirtual)
{
}

FMOD_RESULT ChannelEmulated::getDSPHead(DSPI * * dsp)
{
}

FMOD_RESULT ChannelEmulated::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
}

} // namespace FMOD
