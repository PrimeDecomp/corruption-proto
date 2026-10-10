// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805D975C..0x805DA014 (7 retained native functions).
// Provisional source-family name; original basename and standalone placement are unproven.
// Retained bodies: 24-bit reader, fast16-bit reader, single-bit reader, frame-header decoder, Xing
// parser, frame decoder dispatcher, decoder buffer reset. Private-data boundaries and native call
// inventory support this inferred emitter. Historical header/source alternatives and full evidence
// remain in external research.
// Merged with the former fmod_mpeg_synthesis scaffold:
// G2MEAB .text: 0x805DA014..0x805DB060 (4 retained native functions).
// Provisional source-family name; original basename and standalone placement are unproven.
// Retained bodies: DCT64 transform, signed-short synthesis, float synthesis, synthesis dispatcher.
// Private-data boundaries and native call inventory support this inferred emitter.
// Historical header/source alternatives and full evidence remain in external research.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod.h"
#include "fmod_codec_mpeg.h"

namespace FMOD {

int CodecMPEG::gFreqs[9];
int CodecMPEG::gTabSel123[2][3][16];

unsigned int CodecMPEG::getBits(int number_of_bits)
{
}

unsigned int CodecMPEG::getBitsFast(int number_of_bits)
{
}

unsigned int CodecMPEG::get1Bit()
{
}

FMOD_RESULT CodecMPEG::decodeHeader(void * in, int * samplerate, int * channels, int * framesize)
{
}

FMOD_RESULT CodecMPEG::decodeXingHeader(unsigned char * in, unsigned char * toc, unsigned int * numframes)
{
}

FMOD_RESULT CodecMPEG::decodeFrame(unsigned char * in, void * out, unsigned int * outlen)
{
}

FMOD_RESULT CodecMPEG::resetFrame()
{
}

void CodecMPEG::dct64(float * out0, float * out1, float * samples)
{
}

FMOD_RESULT CodecMPEG::synthC(float * b0, int bo1, int channels, short * samples)
{
}

FMOD_RESULT CodecMPEG::synth(void * samples, float * bandPtr, int channels)
{
}

} // namespace FMOD
