// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_WAV_IMAADPCM_H
#define _FMOD_CODEC_WAV_IMAADPCM_H

#include "fmod_codec_wav.h"

namespace FMOD {

struct WAVE_FORMAT_IMAADPCM
{
    WAVE_FORMATEX wfx; // offset 0x0
    unsigned short wSamplesPerBlock; // offset 0x12
};

} // namespace FMOD

#include "fmod.h"

namespace FMOD {

FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels);
FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, short * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock);
FMOD_RESULT IMAAdpcm_DecodeM16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock, int channels);
FMOD_RESULT IMAAdpcm_DecodeS16(unsigned char * pbSrc, float * pbDst, unsigned int cBlocks, unsigned int nBlockAlignment, unsigned int cSamplesPerBlock);

} // namespace FMOD

#endif
