// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. CodecMPEG uses the verified G2MEAB layout;
// the helper structs keep the 4.06 layout (MPEG_FRAME is confirmed by decodeHeader 0x805D9864).

#ifndef _FMOD_CODEC_MPEG_H
#define _FMOD_CODEC_MPEG_H

#include "fmod.h"
#include "fmod_codeci.h"

struct FMOD_CODEC_STATE;
struct FMOD_CREATESOUNDEXINFO;
namespace FMOD {
    struct CodecMPEG_MemoryBlock;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct III_sideinfo;
    struct MPEG_FRAME;
    struct SyncPoint;
    struct al_table;
    struct bandInfoStruct;
    struct gr_info_s;
}

namespace FMOD {

struct al_table
{
    short bits; // offset 0x0
    short d; // offset 0x2
};

struct MPEG_FRAME
{
    al_table * alloc; // offset 0x0
    int stereo; // offset 0x4
    int jsbound; // offset 0x8
    int II_sblimit; // offset 0xC
    int lsf; // offset 0x10
    int mpeg25; // offset 0x14
    int header_change; // offset 0x18
    int lay; // offset 0x1C
    int error_protection; // offset 0x20
    int bitrate_index; // offset 0x24
    int sampling_frequency; // offset 0x28
    int padding; // offset 0x2C
    int extension; // offset 0x30
    int mode; // offset 0x34
    int mode_ext; // offset 0x38
    int copyright; // offset 0x3C
    int original; // offset 0x40
    int emphasis; // offset 0x44
    int framesize; // offset 0x48
};

struct gr_info_s
{
    int scfsi; // offset 0x0
    unsigned int part2_3_length; // offset 0x4
    unsigned int big_values; // offset 0x8
    unsigned int scalefac_compress; // offset 0xC
    unsigned int block_type; // offset 0x10
    unsigned int mixed_block_flag; // offset 0x14
    unsigned int table_select[3]; // offset 0x18
    unsigned int subblock_gain[3]; // offset 0x24
    unsigned int maxband[3]; // offset 0x30
    unsigned int maxbandl; // offset 0x3C
    unsigned int maxb; // offset 0x40
    unsigned int region1start; // offset 0x44
    unsigned int region2start; // offset 0x48
    unsigned int preflag; // offset 0x4C
    unsigned int scalefac_scale; // offset 0x50
    unsigned int count1table_select; // offset 0x54
    float * full_gain[3]; // offset 0x58
    float * pow2gain; // offset 0x64
};

struct III_sideinfo
{
    unsigned int main_data_begin; // offset 0x0
    unsigned int private_bits; // offset 0x4
    struct {
        gr_info_s gr[2]; // offset 0x0
    } ch[2]; // offset 0x8
};

struct bandInfoStruct
{
    int longIdx[23]; // offset 0x0
    int longDiff[22]; // offset 0x5C
    int shortIdx[14]; // offset 0xB4
    int shortDiff[13]; // offset 0xEC
};

// 4.06 reference only: G2MEAB embeds these members in CodecMPEG (see below). Still named by DSPCodecMPEG.
struct CodecMPEG_MemoryBlock
{
    int mFrameSize; // offset 0x0
    int mFrameSizeOld; // offset 0x4
    MPEG_FRAME mFrame; // offset 0x8
    unsigned int mFrameHeader; // offset 0x54
    unsigned int mNumFrames; // offset 0x58
    unsigned int * mFrameOffset; // offset 0x5C
    int mPcmPoint; // offset 0x60
    int mLayer; // offset 0x64
    unsigned char mBSSpace[2][2304]; // offset 0x68
    int mBSNum; // offset 0x1268
    float mSynthBuffs[2][2][288]; // offset 0x126C
    int mSynthBo; // offset 0x246C
    int mBitIndex; // offset 0x2470
    unsigned char * mWordPointer; // offset 0x2474
    float mBlock[2][2][576]; // offset 0x2478
    int mBlc[2]; // offset 0x4878
    unsigned char mXingToc[100]; // offset 0x4880
    bool mHasXingNumFrames; // offset 0x48E4
    bool mHasXingToc; // offset 0x48E5
};

// G2MEAB layout (sizeof 0x6EF8, the mSize stored by getDescriptionEx 0x805D8160). The 4.06 decoder state
// (CodecMPEG_MemoryBlock, reached through mMemoryBlock) is embedded directly: decodeHeader 0x805D9864 stores
// the MPEG_FRAME fields at +0x1FC..+0x248, the bit readers 0x805D975C.. use +0x2670/+0x2674, and
// setPositionInternal 0x805D9244 clears +0x268 (0x1200), +0x146C (0x1200) and +0x2678 (0x2400). The sync
// point and frame-length members sit between mFrameOffset and mBSSpace (soundCreateInternal 0x805D9560 walks
// +0x254 with a 0x11C stride; openInternal passes +0x260 to decodeFrame as outlen). The PCM buffer is inline:
// openInternal aligns this+0x4AE6 up to 16 bytes into Codec::mPCMBuffer.
struct CodecMPEG : public Codec
{
    int mFrameSize; // offset 0x1F4
    int mFrameSizeOld; // offset 0x1F8
    MPEG_FRAME mFrame; // offset 0x1FC
    unsigned int mFrameHeader; // offset 0x248
    unsigned int mNumFrames; // offset 0x24C
    unsigned int * mFrameOffset; // offset 0x250
    SyncPoint * mSyncPoint; // offset 0x254
    int mNumSyncPoints; // offset 0x258
    int mUnk25C; // offset 0x25C; never accessed (4.06 mPcmPoint position?)
    unsigned int mPCMFrameLengthBytes; // offset 0x260
    int mLayer; // offset 0x264
    unsigned char mBSSpace[2][2304]; // offset 0x268
    int mBSNum; // offset 0x1468
    float mSynthBuffs[2][2][288]; // offset 0x146C
    int mSynthBo; // offset 0x266C
    int mBitIndex; // offset 0x2670
    unsigned char * mWordPointer; // offset 0x2674
    float mBlock[2][2][576]; // offset 0x2678
    int mBlc[2]; // offset 0x4A78
    unsigned char mXingToc[100]; // offset 0x4A80
    bool mHasXingNumFrames; // offset 0x4AE4
    bool mHasXingToc; // offset 0x4AE5
    unsigned char mPCMBufferMemory[(1152 * 2 * 4) + 16]; // offset 0x4AE6
    static bool gInitialized;
    static float gDecWinMem[560];
    static float gCos64[16];
    static float gCos32[8];
    static float gCos16[4];
    static float gCos8[2];
    static float gCos4[1];
    static float * gPnts[5];
    static int gIntWinBase[];
    static int gTabSel123[2][3][16];
    static int gFreqs[9];
    static int gGrp3Tab[96];
    static int gGrp5Tab[384];
    static int gGrp9Tab[3072];
    static float gMuls[27][64];
    static al_table gAlloc0[];
    static al_table gAlloc1[];
    static al_table gAlloc2[];
    static al_table gAlloc3[];
    static al_table gAlloc4[];
    static float gIsPowTable[8207];
    static float gAaCa[8];
    static float gAaCs[8];
    static float gWin[4][36];
    static float gWin1[4][36];
    static float gGainPow2[378];
    static float gCos6_1;
    static float gCos6_2;
    static float gTfCos36[9];
    static float gTfCos12[3];
    static float gCos9[3];
    static float gCos18[3];
    static int gLongLimit[9][23];
    static int gShortLimit[9][14];
    static bandInfoStruct gBandInfo[9];
    static int gMapBuf0[9][152];
    static int gMapBuf1[9][156];
    static int gMapBuf2[9][44];
    static int * gMap[9][3];
    static int * gMapEnd[9][3];
    static unsigned int gN_SLen2[512];
    static unsigned int gI_SLen2[256];
    static float gTan1_1[16];
    static float gTan2_1[16];
    static float gTan1_2[16];
    static float gTan2_2[16];
    static float gPow1_1[2][16];
    static float gPow2_1[2][16];
    static float gPow1_2[2][16];
    static float gPow2_2[2][16];
    static FMOD_RESULT initAll();
    static FMOD_RESULT closeAll();
    static FMOD_RESULT makeTables(int scaleval);
    static FMOD_RESULT initLayer2();
    static FMOD_RESULT initLayer3(int down_sample_sblimit);
    FMOD_RESULT decodeHeader(void * in, int * samplerate, int * channels, int * framesize);
    FMOD_RESULT decodeXingHeader(unsigned char * in, unsigned char * toc, unsigned int * numframes);
    FMOD_RESULT decodeFrame(unsigned char * in, void * out, unsigned int * outlen);
    FMOD_RESULT decodeLayer2(void * pcm_sample, unsigned int * outlen);
    FMOD_RESULT decodeLayer3(void * pcm_sample, unsigned int * outlen);
    FMOD_RESULT getPCMLength();
    FMOD_RESULT resetFrame();
    FMOD_RESULT getIIStuff();
    FMOD_RESULT II_step_one(unsigned int * bit_alloc, int * scale);
    FMOD_RESULT II_step_two(unsigned int * bit_alloc, float (* fraction)[4][32], int * scale, int x1);
    FMOD_RESULT III_get_side_info_1(III_sideinfo * si, int stereo, int ms_stereo, int sfreq);
    FMOD_RESULT III_get_side_info_2(III_sideinfo * si, int stereo, int ms_stereo, int sfreq);
    FMOD_RESULT III_get_scale_factors_1(int * scf, gr_info_s * gr_info, int * numbits);
    FMOD_RESULT III_get_scale_factors_2(int * scf, gr_info_s * gr_info, int i_stereo, int * numbits);
    FMOD_RESULT III_dequantize_sample(float (* xr)[18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits);
    FMOD_RESULT III_dequantize_sample_ms(float (* xr)[32][18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits);
    FMOD_RESULT III_i_stereo(float (* xr_buf)[32][18], int * scalefac, gr_info_s * gr_info, int sfreq, int ms_stereo, int lsf);
    FMOD_RESULT III_antialias(float (* xr)[18], gr_info_s * gr_info);
    FMOD_RESULT III_hybrid(float (* fsIn)[18], float (* tsOut)[32], int ch, gr_info_s * gr_info);
    FMOD_RESULT synth(void * samples, float * bandPtr, int channels);
    FMOD_RESULT synthC(float * b0, int bo1, int channels, short * samples);
    FMOD_RESULT synthFloat(float * b0, int bo1, int channels, float * samples); // Guessed name (0x805DAC18, the PCMFLOAT/MPEG-output path of synth)
    unsigned int getBits(int number_of_bits);
    unsigned int getBitsFast(int number_of_bits);
    unsigned int get1Bit();
    static void dct64(float * out0, float * out1, float * samples);
    static void dct36(float * inbuf, float * o1, float * o2, float * wintab, float * tsbuf);
    static void dct12(float * in, float * rawout1, float * rawout2, float * wi, float * ts);
    FMOD_RESULT openInternal(FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    FMOD_RESULT closeInternal();
    FMOD_RESULT soundCreateInternal(int subsound, FMOD_SOUND * sound);
    FMOD_RESULT readInternal(void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    FMOD_RESULT setPositionInternal(int subsound, unsigned int position, FMOD_TIMEUNIT postype);
    static FMOD_RESULT openCallback(FMOD_CODEC_STATE * codec, FMOD_MODE usermode, FMOD_CREATESOUNDEXINFO * userexinfo);
    static FMOD_RESULT closeCallback(FMOD_CODEC_STATE * codec);
    static FMOD_RESULT soundCreateCallback(FMOD_CODEC_STATE * codec, int subsound, FMOD_SOUND * sound);
    static FMOD_CODEC_DESCRIPTION_EX * getDescriptionEx();
    static FMOD_RESULT readCallback(FMOD_CODEC_STATE * codec, void * buffer, unsigned int sizebytes, unsigned int * bytesread);
    static FMOD_RESULT setPositionCallback(FMOD_CODEC_STATE * codec, int subsound, unsigned int position, FMOD_TIMEUNIT postype);
};

} // namespace FMOD

extern float * FMOD_Mpeg_DecWin;

#endif
