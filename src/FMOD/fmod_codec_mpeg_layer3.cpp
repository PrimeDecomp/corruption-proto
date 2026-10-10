// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x805DBDB8..0x805E113C (13 retained native functions).
// Basename inferred; the prototype does not assert this original filename.
// Complete historical MPEG source order and exact private constants support this emitter.
// Preserve every retained helper; full target/reference evidence and remaining emission uncertainty
// are recorded externally.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod.h"
#include "fmod_codec_mpeg.h"

namespace FMOD {

struct newhuff
{
    unsigned int linbits; // offset 0x0
    short * table; // offset 0x4
};

static newhuff htc[2];
int scale[76];
float CodecMPEG::gPow2_2[2][16];
static newhuff ht[32];
float CodecMPEG::gPow1_2[2][16];
float CodecMPEG::gPow2_1[2][16];
bandInfoStruct CodecMPEG::gBandInfo[9];
float CodecMPEG::gPow1_1[2][16];
float CodecMPEG::gTan2_2[16];
float CodecMPEG::gTan1_2[16];
float CodecMPEG::gTan2_1[16];
float CodecMPEG::gTan1_1[16];
unsigned int CodecMPEG::gI_SLen2[256];
unsigned int CodecMPEG::gN_SLen2[512];
static int pretab1[22];
static short tab_c1[31];
static short tab_c0[31];
static short tab24[511];
int * CodecMPEG::gMapEnd[9][3];
int * CodecMPEG::gMap[9][3];
int CodecMPEG::gMapBuf2[9][44];
static short tab16[511];
static short tab15[511];
int CodecMPEG::gMapBuf1[9][156];
static short tab13[511];
static short tab12[127];
static short tab11[127];
static short tab10[127];
static short tab9[71];
static short tab8[71];
static short tab7[71];
static short tab6[31];
static short tab5[31];
static short tab3[17];
static short tab2[17];
static short tab1[7];
int CodecMPEG::gMapBuf0[9][152];
int CodecMPEG::gShortLimit[9][14];
int CodecMPEG::gLongLimit[9][23];
float CodecMPEG::gCos18[3];
float CodecMPEG::gCos9[3];
float CodecMPEG::gTfCos12[3];
float CodecMPEG::gTfCos36[9];
float CodecMPEG::gCos6_2;
float CodecMPEG::gCos6_1;
float CodecMPEG::gGainPow2[378];
float CodecMPEG::gWin1[4][36];
float CodecMPEG::gWin[4][36];
float CodecMPEG::gAaCs[8];
float CodecMPEG::gAaCa[8];
float CodecMPEG::gIsPowTable[8207];
static int pretab2[22];
static short tab0[1];

FMOD_RESULT CodecMPEG::initLayer3(int down_sample_sblimit)
{
}

FMOD_RESULT CodecMPEG::III_get_side_info_1(III_sideinfo * si, int stereo, int ms_stereo, int sfreq)
{
}

FMOD_RESULT CodecMPEG::III_get_side_info_2(III_sideinfo * si, int stereo, int ms_stereo, int sfreq)
{
}

FMOD_RESULT CodecMPEG::III_get_scale_factors_1(int * scf, gr_info_s * gr_info, int * numbits)
{
}

FMOD_RESULT CodecMPEG::III_get_scale_factors_2(int * scf, gr_info_s * gr_info, int i_stereo, int * numbits)
{
}

FMOD_RESULT CodecMPEG::III_dequantize_sample(float (* xr)[18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits)
{
}

FMOD_RESULT CodecMPEG::III_dequantize_sample_ms(float (* xr)[32][18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits)
{
}

FMOD_RESULT CodecMPEG::III_i_stereo(float (* xr_buf)[32][18], int * scalefac, gr_info_s * gr_info, int sfreq, int ms_stereo, int lsf)
{
}

FMOD_RESULT CodecMPEG::III_antialias(float (* xr)[18], gr_info_s * gr_info)
{
}

void CodecMPEG::dct36(float * inbuf, float * o1, float * o2, float * wintab, float * tsbuf)
{
}

void CodecMPEG::dct12(float * in, float * rawout1, float * rawout2, float * wi, float * ts)
{
}

FMOD_RESULT CodecMPEG::III_hybrid(float (* fsIn)[18], float (* tsOut)[32], int ch, gr_info_s * gr_info)
{
}

FMOD_RESULT CodecMPEG::decodeLayer3(void * pcm_sample, unsigned int * outlen)
{
}

} // namespace FMOD
