// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805E308C..0x805E3420 (10 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Raw descriptor80753434 registration308C, format10/state500, binds32FC/3328/3354/3380 to
// open3124/empty close3210/read3218/seek3288. Open initializes PCM format/channel/rate/stride; read
// handles 16-bit versus byte elements; seek converts time to bytes. Retain close no-op and all
// wrappers. Final33AC initializes same descriptor and ends3420, independent S3M registration.
// Family is proven by semantics/callback closure, original basename inferred. Preserve every
// retained callback, emitted helper and initializer; complete inventory and inlining uncertainty
// are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_raw.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX rawcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecRaw::getDescriptionEx()
{
}

FMOD_RESULT CodecRaw::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecRaw::closeInternal()
{
}

FMOD_RESULT CodecRaw::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecRaw::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecRaw::canPointInternal()
{
}

FMOD_RESULT CodecRaw::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecRaw::closeCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecRaw::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecRaw::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecRaw::canPointCallback(FMOD_CODEC_STATE * codec)
{
}

} // namespace FMOD
