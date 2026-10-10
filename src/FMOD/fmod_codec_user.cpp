// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805E8310..0x805E85D4 (10 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: User Reader descriptor80753554 registration8310, format13/state500,
// binds84B0/84DC/8508/8534 to open83A8/close8494/read849C/seek84A8. Open resolves file length and
// populates shared format state; read reports requested count; empty close/seek are explicit
// retained natives. Final8560 initializes same descriptor and ends85D4, WAV registration. Original
// basename inferred from closed User Reader family, not recovered from a source literal. Preserve
// every retained callback, emitted helper and initializer; complete inventory and inlining
// uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_user.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX usercodec;

FMOD_CODEC_DESCRIPTION_EX * CodecUser::getDescriptionEx()
{
}

FMOD_RESULT CodecUser::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecUser::closeInternal()
{
}

FMOD_RESULT CodecUser::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecUser::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecUser::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecUser::closeCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecUser::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecUser::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

} // namespace FMOD
