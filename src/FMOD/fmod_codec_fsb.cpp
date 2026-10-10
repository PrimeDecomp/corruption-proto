// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805C5794..0x805C84F4 (14 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: FSB descriptor8073F7C4 registration5794 binds five callbacks82E4..8394,
// format8/state23C. Open5838 parses FSB headers and names fmod_codec_fsb.cpp, as do close70D0 and
// metadata80F8. Decoder/read7318, seek7C2C and metadata80F8 retain bank/subsound state and shared
// ADPCM calls. Nested/list destructors706C and83C0 are retained before initializer8480 registers
// the descriptor. Next84F4 begins IT descriptor registration. The shared ADPCM backend is not
// absorbed here. Preserve every retained callback, emitted helper and initializer; complete
// inventory and inlining uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_fsb.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"

namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX fsbcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecFSB::getDescriptionEx()
{
}

FMOD_RESULT CodecFSB::getWaveFormatInternal(int index, FMOD_CODEC_WAVEFORMAT * waveformat_out)
{
}

FMOD_RESULT CodecFSB::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecFSB::closeInternal()
{
}

FMOD_RESULT CodecFSB::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecFSB::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecFSB::soundcreateInternal(int subsound, FMOD_SOUND * sound)
{
}

FMOD_RESULT CodecFSB::resetInternal()
{
}

FMOD_RESULT CodecFSB::canPointInternal()
{
}

FMOD_RESULT CodecFSB::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecFSB::closeCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecFSB::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecFSB::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecFSB::soundcreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound)
{
}

FMOD_RESULT CodecFSB::getWaveFormatCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_CODEC_WAVEFORMAT * waveformat)
{
}

FMOD_RESULT CodecFSB::resetCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecFSB::canPointCallback(FMOD_CODEC_STATE * codec)
{
}

} // namespace FMOD
