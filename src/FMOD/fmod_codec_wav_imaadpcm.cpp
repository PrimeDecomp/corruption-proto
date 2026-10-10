// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805E9924..0x805EAD1C (4 retained native functions).
// Provisional source-family name; original basename and standalone placement are unproven.
// Retained bodies: IMA mono signed-short decoder, IMA stereo signed-short decoder, IMA mono float
// decoder, IMA stereo float decoder. Private-data boundaries and native call inventory support this
// inferred emitter. Historical header/source alternatives and full evidence remain in external
// research.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_codec_wav_imaadpcm.h"
#include "fmod.h"

namespace FMOD {

static const short IMAAdpcm_StepTab[89] = {0};
static const int IMAAdpcm_IndexTab[16] = {0};

FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels)
{
}

FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock)
{
}

FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels)
{
}

FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock)
{
}

} // namespace FMOD
