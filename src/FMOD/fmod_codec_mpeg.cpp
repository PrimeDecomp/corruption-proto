// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805D8160..0x805D975C (15 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: MPEG descriptor80740D2C registration8160 binds960C..96BC to
// open86B0/close900C/read9064/seek9244/metadata953C, formatD/state6EF8. Allocation/free names
// fmod_codec_mpeg.cpp. Synthesis-window setup8204 and combined backend initialization84C4 are
// retained before seek-index8530; initializer96E8 closes descriptor at975C. Backend begins975C with
// shared bit reader and is explicitly unresolved rather than folded into this asserted wrapper TU.
// Preserve every retained callback, emitted helper and initializer; complete inventory and inlining
// uncertainty are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_mpeg.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_os_misc.h"

namespace FMOD {

int CodecMPEG::gIntWinBase[257];
bool CodecMPEG::gInitialized;
float * CodecMPEG::gPnts[5];
float CodecMPEG::gCos4[1];
float CodecMPEG::gCos8[2];
float CodecMPEG::gCos16[4];
float CodecMPEG::gCos32[8];
float CodecMPEG::gCos64[16];
float CodecMPEG::gDecWinMem[560];
FMOD_OS_CRITICALSECTION * gDecodeCrit;
} // namespace FMOD

float * FMOD_Mpeg_DecWin;
namespace FMOD {

FMOD_CODEC_DESCRIPTION_EX mpegcodec;

FMOD_CODEC_DESCRIPTION_EX * CodecMPEG::getDescriptionEx()
{
}

FMOD_RESULT CodecMPEG::makeTables(int scaleval)
{
}

FMOD_RESULT CodecMPEG::initAll()
{
}

FMOD_RESULT CodecMPEG::closeAll()
{
}

FMOD_RESULT CodecMPEG::getPCMLength()
{
}

FMOD_RESULT CodecMPEG::openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecMPEG::closeInternal()
{
}

FMOD_RESULT CodecMPEG::readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecMPEG::setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecMPEG::soundCreateInternal(int subsound, FMOD_SOUND * sound)
{
}

FMOD_RESULT CodecMPEG::openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo)
{
}

FMOD_RESULT CodecMPEG::closeCallback(FMOD_CODEC_STATE * codec)
{
}

FMOD_RESULT CodecMPEG::readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
}

FMOD_RESULT CodecMPEG::setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
}

FMOD_RESULT CodecMPEG::soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound)
{
}

} // namespace FMOD
