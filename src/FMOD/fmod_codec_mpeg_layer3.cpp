// NonMatching: complete reconstruction of the G2MEAB MPEG layer III decoder (0x805DBDB8..0x805E113C,
// 13 native functions), the mpglib/mpg123 layer III code adapted to CodecMPEG members.
// The band, Huffman and scale-factor tables are read from G2MEAB .data 0x806E9368..0x806EB5E8
// (tab0 in .sdata 0x80796EF8); the remaining tables are .bss filled by initLayer3.
// Basename inferred; the prototype does not assert this original filename.

#include "fmod.h"
#include "fmod_codec_mpeg.h"

#include <math.h>
#include <string.h>

#define FMOD_PI 3.14159265358979323846f // Guessed name
#define FMOD_SQRT2 1.41421356237309504880f // Guessed name

namespace FMOD {

struct newhuff
{
    unsigned int linbits; // offset 0x0
    short * table; // offset 0x4
};

float CodecMPEG::gIsPowTable[8207];
float CodecMPEG::gAaCa[8];
float CodecMPEG::gAaCs[8];
float CodecMPEG::gWin[4][36];
float CodecMPEG::gWin1[4][36];
float CodecMPEG::gGainPow2[378];
float CodecMPEG::gCos6_1;
float CodecMPEG::gCos6_2;
float CodecMPEG::gTfCos36[9];
float CodecMPEG::gTfCos12[3];
float CodecMPEG::gCos9[3];
float CodecMPEG::gCos18[3];
int CodecMPEG::gLongLimit[9][23];
int CodecMPEG::gShortLimit[9][14];

bandInfoStruct CodecMPEG::gBandInfo[9] =
{
    {
        { 0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 52, 62, 74, 90, 110, 134, 162, 196, 238, 288, 342, 418, 576 },
        { 4, 4, 4, 4, 4, 4, 6, 6, 8, 8, 10, 12, 16, 20, 24, 28, 34, 42, 50, 54, 76, 158 },
        { 0, 12, 24, 36, 48, 66, 90, 120, 156, 198, 252, 318, 408, 576 },
        { 4, 4, 4, 4, 6, 8, 10, 12, 14, 18, 22, 30, 56 }
    },
    {
        { 0, 4, 8, 12, 16, 20, 24, 30, 36, 42, 50, 60, 72, 88, 106, 128, 156, 190, 230, 276, 330, 384, 576 },
        { 4, 4, 4, 4, 4, 4, 6, 6, 6, 8, 10, 12, 16, 18, 22, 28, 34, 40, 46, 54, 54, 192 },
        { 0, 12, 24, 36, 48, 66, 84, 114, 150, 192, 240, 300, 378, 576 },
        { 4, 4, 4, 4, 6, 6, 10, 12, 14, 16, 20, 26, 66 }
    },
    {
        { 0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 54, 66, 82, 102, 126, 156, 194, 240, 296, 364, 448, 550, 576 },
        { 4, 4, 4, 4, 4, 4, 6, 6, 8, 10, 12, 16, 20, 24, 30, 38, 46, 56, 68, 84, 102, 26 },
        { 0, 12, 24, 36, 48, 66, 90, 126, 174, 234, 312, 414, 540, 576 },
        { 4, 4, 4, 4, 6, 8, 12, 16, 20, 26, 34, 42, 12 }
    },
    {
        { 0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
        { 6, 6, 6, 6, 6, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 38, 46, 52, 60, 68, 58, 54 },
        { 0, 12, 24, 36, 54, 72, 96, 126, 168, 222, 300, 396, 522, 576 },
        { 4, 4, 4, 6, 6, 8, 10, 14, 18, 26, 32, 42, 18 }
    },
    {
        { 0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 114, 136, 162, 194, 232, 278, 330, 394, 464, 540, 576 },
        { 6, 6, 6, 6, 6, 6, 8, 10, 12, 14, 16, 18, 22, 26, 32, 38, 46, 52, 64, 70, 76, 36 },
        { 0, 12, 24, 36, 54, 78, 108, 144, 186, 240, 312, 408, 540, 576 },
        { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 32, 44, 12 }
    },
    {
        { 0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
        { 6, 6, 6, 6, 6, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 38, 46, 52, 60, 68, 58, 54 },
        { 0, 12, 24, 36, 54, 78, 108, 144, 186, 240, 312, 402, 522, 576 },
        { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 }
    },
    {
        { 0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
        { 6, 6, 6, 6, 6, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 38, 46, 52, 60, 68, 58, 54 },
        { 0, 12, 24, 36, 54, 78, 108, 144, 186, 240, 312, 402, 522, 576 },
        { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 }
    },
    {
        { 0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576 },
        { 6, 6, 6, 6, 6, 6, 8, 10, 12, 14, 16, 20, 24, 28, 32, 38, 46, 52, 60, 68, 58, 54 },
        { 0, 12, 24, 36, 54, 78, 108, 144, 186, 240, 312, 402, 522, 576 },
        { 4, 4, 4, 6, 8, 10, 12, 14, 18, 24, 30, 40, 18 }
    },
    {
        { 0, 12, 24, 36, 48, 60, 72, 88, 108, 132, 160, 192, 232, 280, 336, 400, 476, 566, 568, 570, 572, 574, 576 },
        { 12, 12, 12, 12, 12, 12, 16, 20, 24, 28, 32, 40, 48, 56, 64, 76, 90, 2, 2, 2, 2, 2 },
        { 0, 24, 48, 72, 108, 156, 216, 288, 372, 480, 486, 492, 498, 576 },
        { 8, 8, 8, 12, 16, 20, 24, 28, 36, 2, 2, 2, 26 }
    }
};

int CodecMPEG::gMapBuf0[9][152];
int CodecMPEG::gMapBuf1[9][156];
int CodecMPEG::gMapBuf2[9][44];
int * CodecMPEG::gMap[9][3];
int * CodecMPEG::gMapEnd[9][3];
unsigned int CodecMPEG::gN_SLen2[512];
unsigned int CodecMPEG::gI_SLen2[256];
float CodecMPEG::gTan1_1[16];
float CodecMPEG::gTan2_1[16];
float CodecMPEG::gTan1_2[16];
float CodecMPEG::gTan2_2[16];
float CodecMPEG::gPow1_1[2][16];
float CodecMPEG::gPow2_1[2][16];
float CodecMPEG::gPow1_2[2][16];
float CodecMPEG::gPow2_2[2][16];

// 4.06 reference global; no G2MEAB consumer found (layer3 .bss ends at 0x80753368).
int scale[76];

static short tab0[1] = { 0 };
static short tab1[7] =
{
    -5, -3, -1, 17, 1, 16, 0
};

static short tab2[17] =
{
    -15, -11, -9, -5, -3, -1, 34, 2, 18, -1, 33, 32, 17, -1, 1, 16,
    0
};

static short tab3[17] =
{
    -13, -11, -9, -5, -3, -1, 34, 2, 18, -1, 33, 32, 16, 17, -1, 1,
    0
};

static short tab5[31] =
{
    -29, -25, -23, -15, -7, -5, -3, -1, 51, 35, 50, 49, -3, -1, 19, 3,
    -1, 48, 34, -3, -1, 18, 33, -1, 2, 32, 17, -1, 1, 16, 0
};

static short tab6[31] =
{
    -25, -19, -13, -9, -5, -3, -1, 51, 3, 35, -1, 50, 48, -1, 19, 49,
    -3, -1, 34, 2, 18, -3, -1, 33, 32, 1, -1, 17, -1, 16, 0
};

static short tab7[71] =
{
    -69, -65, -57, -39, -29, -17, -11, -7, -3, -1, 85, 69, -1, 84, 83, -1,
    53, 68, -3, -1, 37, 82, 21, -5, -1, 81, -1, 5, 52, -1, 80, -1,
    67, 51, -5, -3, -1, 36, 66, 20, -1, 65, 64, -11, -7, -3, -1, 4,
    35, -1, 50, 3, -1, 19, 49, -3, -1, 48, 34, 18, -5, -1, 33, -1,
    2, 32, 17, -1, 1, 16, 0
};

static short tab8[71] =
{
    -65, -63, -59, -45, -31, -19, -13, -7, -5, -3, -1, 85, 84, 69, 83, -3,
    -1, 53, 68, 37, -3, -1, 82, 5, 21, -5, -1, 81, -1, 52, 67, -3,
    -1, 80, 51, 36, -5, -3, -1, 66, 20, 65, -3, -1, 4, 64, -1, 35,
    50, -9, -7, -3, -1, 19, 49, -1, 3, 48, 34, -1, 2, 32, -1, 18,
    33, 17, -3, -1, 1, 16, 0
};

static short tab9[71] =
{
    -63, -53, -41, -29, -19, -11, -5, -3, -1, 85, 69, 53, -1, 83, -1, 84,
    5, -3, -1, 68, 37, -1, 82, 21, -3, -1, 81, 52, -1, 67, -1, 80,
    4, -7, -3, -1, 36, 66, -1, 51, 64, -1, 20, 65, -5, -3, -1, 35,
    50, 19, -1, 49, -1, 3, 48, -5, -3, -1, 34, 2, 18, -1, 33, 32,
    -3, -1, 17, 1, -1, 16, 0
};

static short tab10[127] =
{
    -125, -121, -111, -83, -55, -35, -21, -13, -7, -3, -1, 119, 103, -1, 118, 87,
    -3, -1, 117, 102, 71, -3, -1, 116, 86, -1, 101, 55, -9, -3, -1, 115,
    70, -3, -1, 85, 84, 99, -1, 39, 114, -11, -5, -3, -1, 100, 7, 112,
    -1, 98, -1, 69, 53, -5, -1, 6, -1, 83, 68, 23, -17, -5, -1, 113,
    -1, 54, 38, -5, -3, -1, 37, 82, 21, -1, 81, -1, 52, 67, -3, -1,
    22, 97, -1, 96, -1, 5, 80, -19, -11, -7, -3, -1, 36, 66, -1, 51,
    4, -1, 20, 65, -3, -1, 64, 35, -1, 50, 3, -3, -1, 19, 49, -1,
    48, 34, -7, -3, -1, 18, 33, -1, 2, 32, 17, -1, 1, 16, 0
};

static short tab11[127] =
{
    -121, -113, -89, -59, -43, -27, -17, -7, -3, -1, 119, 103, -1, 118, 117, -3,
    -1, 102, 71, -1, 116, -1, 87, 85, -5, -3, -1, 86, 101, 55, -1, 115,
    70, -9, -7, -3, -1, 69, 84, -1, 53, 83, 39, -1, 114, -1, 100, 7,
    -5, -1, 113, -1, 23, 112, -3, -1, 54, 99, -1, 96, -1, 68, 37, -13,
    -7, -5, -3, -1, 82, 5, 21, 98, -3, -1, 38, 6, 22, -5, -1, 97,
    -1, 81, 52, -5, -1, 80, -1, 67, 51, -1, 36, 66, -15, -11, -7, -3,
    -1, 20, 65, -1, 4, 64, -1, 35, 50, -1, 19, 49, -5, -3, -1, 3,
    48, 34, 33, -5, -1, 18, -1, 2, 32, 17, -3, -1, 1, 16, 0
};

static short tab12[127] =
{
    -115, -99, -73, -45, -27, -17, -9, -5, -3, -1, 119, 103, 118, -1, 87, 117,
    -3, -1, 102, 71, -1, 116, 101, -3, -1, 86, 55, -3, -1, 115, 85, 39,
    -7, -3, -1, 114, 70, -1, 100, 23, -5, -1, 113, -1, 7, 112, -1, 54,
    99, -13, -9, -3, -1, 69, 84, -1, 68, -1, 6, 5, -1, 38, 98, -5,
    -1, 97, -1, 22, 96, -3, -1, 53, 83, -1, 37, 82, -17, -7, -3, -1,
    21, 81, -1, 52, 67, -5, -3, -1, 80, 4, 36, -1, 66, 20, -3, -1,
    51, 65, -1, 35, 50, -11, -7, -5, -3, -1, 64, 3, 48, 19, -1, 49,
    34, -1, 18, 33, -7, -5, -3, -1, 2, 32, 0, 17, -1, 1, 16
};

static short tab13[511] =
{
    -509, -503, -475, -405, -333, -265, -205, -153, -115, -83, -53, -35, -21, -13, -9, -7,
    -5, -3, -1, 254, 252, 253, 237, 255, -1, 239, 223, -3, -1, 238, 207, -1,
    222, 191, -9, -3, -1, 251, 206, -1, 220, -1, 175, 233, -1, 236, 221, -9,
    -5, -3, -1, 250, 205, 190, -1, 235, 159, -3, -1, 249, 234, -1, 189, 219,
    -17, -9, -3, -1, 143, 248, -1, 204, -1, 174, 158, -5, -1, 142, -1, 127,
    126, 247, -5, -1, 218, -1, 173, 188, -3, -1, 203, 246, 111, -15, -7, -3,
    -1, 232, 95, -1, 157, 217, -3, -1, 245, 231, -1, 172, 187, -9, -3, -1,
    79, 244, -3, -1, 202, 230, 243, -1, 63, -1, 141, 216, -21, -9, -3, -1,
    47, 242, -3, -1, 110, 156, 15, -5, -3, -1, 201, 94, 171, -3, -1, 125,
    215, 78, -11, -5, -3, -1, 200, 214, 62, -1, 185, -1, 155, 170, -1, 31,
    241, -23, -13, -5, -1, 240, -1, 186, 229, -3, -1, 228, 140, -1, 109, 227,
    -5, -1, 226, -1, 46, 14, -1, 30, 225, -15, -7, -3, -1, 224, 93, -1,
    213, 124, -3, -1, 199, 77, -1, 139, 184, -7, -3, -1, 212, 154, -1, 169,
    108, -1, 198, 61, -37, -21, -9, -5, -3, -1, 211, 123, 45, -1, 210, 29,
    -5, -1, 183, -1, 92, 197, -3, -1, 153, 122, 195, -7, -5, -3, -1, 167,
    151, 75, 209, -3, -1, 13, 208, -1, 138, 168, -11, -7, -3, -1, 76, 196,
    -1, 107, 182, -1, 60, 44, -3, -1, 194, 91, -3, -1, 181, 137, 28, -43,
    -23, -11, -5, -1, 193, -1, 152, 12, -1, 192, -1, 180, 106, -5, -3, -1,
    166, 121, 59, -1, 179, -1, 136, 90, -11, -5, -1, 43, -1, 165, 105, -1,
    164, -1, 120, 135, -5, -1, 148, -1, 119, 118, 178, -11, -3, -1, 27, 177,
    -3, -1, 11, 176, -1, 150, 74, -7, -3, -1, 58, 163, -1, 89, 149, -1,
    42, 162, -47, -23, -9, -3, -1, 26, 161, -3, -1, 10, 104, 160, -5, -3,
    -1, 134, 73, 147, -3, -1, 57, 88, -1, 133, 103, -9, -3, -1, 41, 146,
    -3, -1, 87, 117, 56, -5, -1, 131, -1, 102, 71, -3, -1, 116, 86, -1,
    101, 115, -11, -3, -1, 25, 145, -3, -1, 9, 144, -1, 72, 132, -7, -5,
    -1, 114, -1, 70, 100, 40, -1, 130, 24, -41, -27, -11, -5, -3, -1, 55,
    39, 23, -1, 113, -1, 85, 7, -7, -3, -1, 112, 54, -1, 99, 69, -3,
    -1, 84, 38, -1, 98, 53, -5, -1, 129, -1, 8, 128, -3, -1, 22, 97,
    -1, 6, 96, -13, -9, -5, -3, -1, 83, 68, 37, -1, 82, 5, -1, 21,
    81, -7, -3, -1, 52, 67, -1, 80, 36, -3, -1, 66, 51, 20, -19, -11,
    -5, -1, 65, -1, 4, 64, -3, -1, 35, 50, 19, -3, -1, 49, 3, -1,
    48, 34, -3, -1, 18, 33, -1, 2, 32, -3, -1, 17, 1, 16, 0
};

static short tab15[511] =
{
    -495, -445, -355, -263, -183, -115, -77, -43, -27, -13, -7, -3, -1, 255, 239, -1,
    254, 223, -1, 238, -1, 253, 207, -7, -3, -1, 252, 222, -1, 237, 191, -1,
    251, -1, 206, 236, -7, -3, -1, 221, 175, -1, 250, 190, -3, -1, 235, 205,
    -1, 220, 159, -15, -7, -3, -1, 249, 234, -1, 189, 219, -3, -1, 143, 248,
    -1, 204, 158, -7, -3, -1, 233, 127, -1, 247, 173, -3, -1, 218, 188, -1,
    111, -1, 174, 15, -19, -11, -3, -1, 203, 246, -3, -1, 142, 232, -1, 95,
    157, -3, -1, 245, 126, -1, 231, 172, -9, -3, -1, 202, 187, -3, -1, 217,
    141, 79, -3, -1, 244, 63, -1, 243, 216, -33, -17, -9, -3, -1, 230, 47,
    -1, 242, -1, 110, 240, -3, -1, 31, 241, -1, 156, 201, -7, -3, -1, 94,
    171, -1, 186, 229, -3, -1, 125, 215, -1, 78, 228, -15, -7, -3, -1, 140,
    200, -1, 62, 109, -3, -1, 214, 227, -1, 155, 185, -7, -3, -1, 46, 170,
    -1, 226, 30, -5, -1, 225, -1, 14, 224, -1, 93, 213, -45, -25, -13, -7,
    -3, -1, 124, 199, -1, 77, 139, -1, 212, -1, 184, 154, -7, -3, -1, 169,
    108, -1, 198, 61, -1, 211, 210, -9, -5, -3, -1, 45, 13, 29, -1, 123,
    183, -5, -1, 209, -1, 92, 208, -1, 197, 138, -17, -7, -3, -1, 168, 76,
    -1, 196, 107, -5, -1, 182, -1, 153, 12, -1, 60, 195, -9, -3, -1, 122,
    167, -1, 166, -1, 192, 11, -1, 194, -1, 44, 91, -55, -29, -15, -7, -3,
    -1, 181, 28, -1, 137, 152, -3, -1, 193, 75, -1, 180, 106, -5, -3, -1,
    59, 121, 179, -3, -1, 151, 136, -1, 43, 90, -11, -5, -1, 178, -1, 165,
    27, -1, 177, -1, 176, 105, -7, -3, -1, 150, 74, -1, 164, 120, -3, -1,
    135, 58, 163, -17, -7, -3, -1, 89, 149, -1, 42, 162, -3, -1, 26, 161,
    -3, -1, 10, 160, 104, -7, -3, -1, 134, 73, -1, 148, 57, -5, -1, 147,
    -1, 119, 9, -1, 88, 133, -53, -29, -13, -7, -3, -1, 41, 103, -1, 118,
    146, -1, 145, -1, 25, 144, -7, -3, -1, 72, 132, -1, 87, 117, -3, -1,
    56, 131, -1, 102, 71, -7, -3, -1, 40, 130, -1, 24, 129, -7, -3, -1,
    116, 8, -1, 128, 86, -3, -1, 101, 55, -1, 115, 70, -17, -7, -3, -1,
    39, 114, -1, 100, 23, -3, -1, 85, 113, -3, -1, 7, 112, 54, -7, -3,
    -1, 99, 69, -1, 84, 38, -3, -1, 98, 22, -3, -1, 6, 96, 53, -33,
    -19, -9, -5, -1, 97, -1, 83, 68, -1, 37, 82, -3, -1, 21, 81, -3,
    -1, 5, 80, 52, -7, -3, -1, 67, 36, -1, 66, 51, -1, 65, -1, 20,
    4, -9, -3, -1, 35, 50, -3, -1, 64, 3, 19, -3, -1, 49, 48, 34,
    -9, -7, -3, -1, 18, 33, -1, 2, 32, 17, -3, -1, 1, 16, 0
};

static short tab16[511] =
{
    -509, -503, -461, -323, -103, -37, -27, -15, -7, -3, -1, 239, 254, -1, 223, 253,
    -3, -1, 207, 252, -1, 191, 251, -5, -1, 175, -1, 250, 159, -3, -1, 249,
    248, 143, -7, -3, -1, 127, 247, -1, 111, 246, 255, -9, -5, -3, -1, 95,
    245, 79, -1, 244, 243, -53, -1, 240, -1, 63, -29, -19, -13, -7, -5, -1,
    206, -1, 236, 221, 222, -1, 233, -1, 234, 217, -1, 238, -1, 237, 235, -3,
    -1, 190, 205, -3, -1, 220, 219, 174, -11, -5, -1, 204, -1, 173, 218, -3,
    -1, 126, 172, 202, -5, -3, -1, 201, 125, 94, 189, 242, -93, -5, -3, -1,
    47, 15, 31, -1, 241, -49, -25, -13, -5, -1, 158, -1, 188, 203, -3, -1,
    142, 232, -1, 157, 231, -7, -3, -1, 187, 141, -1, 216, 110, -1, 230, 156,
    -13, -7, -3, -1, 171, 186, -1, 229, 215, -1, 78, -1, 228, 140, -3, -1,
    200, 62, -1, 109, -1, 214, 155, -19, -11, -5, -3, -1, 185, 170, 225, -1,
    212, -1, 184, 169, -5, -1, 123, -1, 183, 208, 227, -7, -3, -1, 14, 224,
    -1, 93, 213, -3, -1, 124, 199, -1, 77, 139, -75, -45, -27, -13, -7, -3,
    -1, 154, 108, -1, 198, 61, -3, -1, 92, 197, 13, -7, -3, -1, 138, 168,
    -1, 153, 76, -3, -1, 182, 122, 60, -11, -5, -3, -1, 91, 137, 28, -1,
    192, -1, 152, 121, -1, 226, -1, 46, 30, -15, -7, -3, -1, 211, 45, -1,
    210, 209, -5, -1, 59, -1, 151, 136, 29, -7, -3, -1, 196, 107, -1, 195,
    167, -1, 44, -1, 194, 181, -23, -13, -7, -3, -1, 193, 12, -1, 75, 180,
    -3, -1, 106, 166, 179, -5, -3, -1, 90, 165, 43, -1, 178, 27, -13, -5,
    -1, 177, -1, 11, 176, -3, -1, 105, 150, -1, 74, 164, -5, -3, -1, 120,
    135, 163, -3, -1, 58, 89, 42, -97, -57, -33, -19, -11, -5, -3, -1, 149,
    104, 161, -3, -1, 134, 119, 148, -5, -3, -1, 73, 87, 103, 162, -5, -1,
    26, -1, 10, 160, -3, -1, 57, 147, -1, 88, 133, -9, -3, -1, 41, 146,
    -3, -1, 118, 9, 25, -5, -1, 145, -1, 144, 72, -3, -1, 132, 117, -1,
    56, 131, -21, -11, -5, -3, -1, 102, 40, 130, -3, -1, 71, 116, 24, -3,
    -1, 129, 128, -3, -1, 8, 86, 55, -9, -5, -1, 115, -1, 101, 70, -1,
    39, 114, -5, -3, -1, 100, 85, 7, 23, -23, -13, -5, -1, 113, -1, 112,
    54, -3, -1, 99, 69, -1, 84, 38, -3, -1, 98, 22, -1, 97, -1, 6,
    96, -9, -5, -1, 83, -1, 53, 68, -1, 37, 82, -1, 81, -1, 21, 5,
    -33, -23, -13, -7, -3, -1, 52, 67, -1, 80, 36, -3, -1, 66, 51, 20,
    -5, -1, 65, -1, 4, 64, -1, 35, 50, -3, -1, 19, 49, -3, -1, 3,
    48, 34, -3, -1, 18, 33, -1, 2, 32, -3, -1, 17, 1, 16, 0
};

static short tab24[511] =
{
    -451, -117, -43, -25, -15, -7, -3, -1, 239, 254, -1, 223, 253, -3, -1, 207,
    252, -1, 191, 251, -5, -1, 250, -1, 175, 159, -1, 249, 248, -9, -5, -3,
    -1, 143, 127, 247, -1, 111, 246, -3, -1, 95, 245, -1, 79, 244, -71, -7,
    -3, -1, 63, 243, -1, 47, 242, -5, -1, 241, -1, 31, 240, -25, -9, -1,
    15, -3, -1, 238, 222, -1, 237, 206, -7, -3, -1, 236, 221, -1, 190, 235,
    -3, -1, 205, 220, -1, 174, 234, -15, -7, -3, -1, 189, 219, -1, 204, 158,
    -3, -1, 233, 173, -1, 218, 188, -7, -3, -1, 203, 142, -1, 232, 157, -3,
    -1, 217, 126, -1, 231, 172, 255, -235, -143, -77, -45, -25, -15, -7, -3, -1,
    202, 187, -1, 141, 216, -5, -3, -1, 14, 224, 13, 230, -5, -3, -1, 110,
    156, 201, -1, 94, 186, -9, -5, -1, 229, -1, 171, 125, -1, 215, 228, -3,
    -1, 140, 200, -3, -1, 78, 46, 62, -15, -7, -3, -1, 109, 214, -1, 227,
    155, -3, -1, 185, 170, -1, 226, 30, -7, -3, -1, 225, 93, -1, 213, 124,
    -3, -1, 199, 77, -1, 139, 184, -31, -15, -7, -3, -1, 212, 154, -1, 169,
    108, -3, -1, 198, 61, -1, 211, 45, -7, -3, -1, 210, 29, -1, 123, 183,
    -3, -1, 209, 92, -1, 197, 138, -17, -7, -3, -1, 168, 153, -1, 76, 196,
    -3, -1, 107, 182, -3, -1, 208, 12, 60, -7, -3, -1, 195, 122, -1, 167,
    44, -3, -1, 194, 91, -1, 181, 28, -57, -35, -19, -7, -3, -1, 137, 152,
    -1, 193, 75, -5, -3, -1, 192, 11, 59, -3, -1, 176, 10, 26, -5, -1,
    180, -1, 106, 166, -3, -1, 121, 151, -3, -1, 160, 9, 144, -9, -3, -1,
    179, 136, -3, -1, 43, 90, 178, -7, -3, -1, 165, 27, -1, 177, 105, -1,
    150, 164, -17, -9, -5, -3, -1, 74, 120, 135, -1, 58, 163, -3, -1, 89,
    149, -1, 42, 162, -7, -3, -1, 161, 104, -1, 134, 119, -3, -1, 73, 148,
    -1, 57, 147, -63, -31, -15, -7, -3, -1, 88, 133, -1, 41, 103, -3, -1,
    118, 146, -1, 25, 145, -7, -3, -1, 72, 132, -1, 87, 117, -3, -1, 56,
    131, -1, 102, 40, -17, -7, -3, -1, 130, 24, -1, 71, 116, -5, -1, 129,
    -1, 8, 128, -1, 86, 101, -7, -5, -1, 23, -1, 7, 112, 115, -3, -1,
    55, 39, 114, -15, -7, -3, -1, 70, 100, -1, 85, 113, -3, -1, 54, 99,
    -1, 69, 84, -7, -3, -1, 38, 98, -1, 22, 97, -5, -3, -1, 6, 96,
    53, -1, 83, 68, -51, -37, -23, -15, -9, -3, -1, 37, 82, -1, 21, -1,
    5, 80, -1, 81, -1, 52, 67, -3, -1, 36, 66, -1, 51, 20, -9, -5,
    -1, 65, -1, 4, 64, -1, 35, 50, -1, 19, 49, -7, -5, -3, -1, 3,
    48, 34, 18, -1, 33, -1, 2, 32, -3, -1, 17, 1, -1, 16, 0
};

static short tab_c0[31] =
{
    -29, -21, -13, -7, -3, -1, 11, 15, -1, 13, 14, -3, -1, 7, 5, 9,
    -3, -1, 6, 3, -1, 10, 12, -3, -1, 2, 1, -1, 4, 8, 0
};

static short tab_c1[31] =
{
    -15, -7, -3, -1, 15, 14, -1, 13, 12, -3, -1, 11, 10, -1, 9, 8,
    -7, -3, -1, 7, 6, -1, 5, 4, -3, -1, 3, 2, -1, 1, 0
};

static newhuff ht[32] =
{
    { 0, tab0 }, { 0, tab1 }, { 0, tab2 }, { 0, tab3 },
    { 0, tab0 }, { 0, tab5 }, { 0, tab6 }, { 0, tab7 },
    { 0, tab8 }, { 0, tab9 }, { 0, tab10 }, { 0, tab11 },
    { 0, tab12 }, { 0, tab13 }, { 0, tab0 }, { 0, tab15 },
    { 1, tab16 }, { 2, tab16 }, { 3, tab16 }, { 4, tab16 },
    { 6, tab16 }, { 8, tab16 }, { 10, tab16 }, { 13, tab16 },
    { 4, tab24 }, { 5, tab24 }, { 6, tab24 }, { 7, tab24 },
    { 8, tab24 }, { 9, tab24 }, { 11, tab24 }, { 13, tab24 }
};

static newhuff htc[2] =
{
    { 0, tab_c0 }, { 0, tab_c1 }
};

FMOD_RESULT CodecMPEG::initLayer3(int down_sample_sblimit)
{
    int i, j, k, l;

    for (i = -256; i < 118 + 4; i++)
    {
        gGainPow2[i + 256] = (float)pow(2.0, -0.25f * (float)(i + 210));
    }

    for (i = 0; i < 8207; i++)
    {
        gIsPowTable[i] = (float)pow((float)i, 4.0f / 3.0f);
    }

    for (i = 0; i < 8; i++)
    {
        static float Ci[8] = { -0.6f, -0.535f, -0.33f, -0.185f, -0.095f, -0.041f, -0.0142f, -0.0037f };
        float sq = sqrtf(1.0f + Ci[i] * Ci[i]);

        gAaCs[i] = 1.0f / sq;
        gAaCa[i] = Ci[i] / sq;
    }

    for (i = 0; i < 18; i++)
    {
        gWin[0][i] = gWin[1][i] = 0.5f * (float)sin(FMOD_PI / 72.0f * (float)(2 * (i + 0) + 1)) / (float)cos(FMOD_PI * (float)(2 * (i + 0) + 19) / 72.0f);
        gWin[0][i + 18] = gWin[3][i + 18] = 0.5f * (float)sin(FMOD_PI / 72.0f * (float)(2 * (i + 18) + 1)) / (float)cos(FMOD_PI * (float)(2 * (i + 18) + 19) / 72.0f);
    }
    for (i = 0; i < 6; i++)
    {
        gWin[1][i + 18] = 0.5f / (float)cos(FMOD_PI * (float)(2 * (i + 18) + 19) / 72.0f);
        gWin[3][i + 12] = 0.5f / (float)cos(FMOD_PI * (float)(2 * (i + 12) + 19) / 72.0f);
        gWin[1][i + 24] = 0.5f * (float)sin(FMOD_PI / 24.0f * (float)(2 * i + 13)) / (float)cos(FMOD_PI * (float)(2 * (i + 24) + 19) / 72.0f);
        gWin[1][i + 30] = gWin[3][i] = 0.0f;
        gWin[3][i + 6] = 0.5f * (float)sin(FMOD_PI / 24.0f * (float)(2 * i + 1)) / (float)cos(FMOD_PI * (float)(2 * (i + 6) + 19) / 72.0f);
    }

    for (i = 0; i < 9; i++)
    {
        gTfCos36[i] = 0.5f / (float)cos(FMOD_PI * (float)(i * 2 + 1) / 36.0f);
    }
    for (i = 0; i < 3; i++)
    {
        gTfCos12[i] = 0.5f / (float)cos(FMOD_PI * (float)(i * 2 + 1) / 12.0f);
    }

    gCos6_1 = (float)cos(FMOD_PI / 6.0f * 1.0f);
    gCos6_2 = (float)cos(FMOD_PI / 6.0f * 2.0f);

    gCos9[0] = (float)cos(1.0f * FMOD_PI / 9.0f);
    gCos9[1] = (float)cos(5.0f * FMOD_PI / 9.0f);
    gCos9[2] = (float)cos(7.0f * FMOD_PI / 9.0f);
    gCos18[0] = (float)cos(1.0f * FMOD_PI / 18.0f);
    gCos18[1] = (float)cos(11.0f * FMOD_PI / 18.0f);
    gCos18[2] = (float)cos(13.0f * FMOD_PI / 18.0f);

    for (i = 0; i < 12; i++)
    {
        gWin[2][i] = 0.5f * (float)sin(FMOD_PI / 24.0f * (float)(2 * i + 1)) / (float)cos(FMOD_PI * (float)(2 * i + 7) / 24.0f);
    }

    for (j = 0; j < 4; j++)
    {
        static int len[4] = { 36, 36, 12, 36 };

        for (i = 0; i < len[j]; i += 2)
        {
            gWin1[j][i] = +gWin[j][i];
        }
        for (i = 1; i < len[j]; i += 2)
        {
            gWin1[j][i] = -gWin[j][i];
        }
    }

    for (i = 0; i < 16; i++)
    {
        float t = (float)tan(FMOD_PI * (float)i / 12.0f);

        gTan1_1[i] = t / (1.0f + t);
        gTan2_1[i] = 1.0f / (1.0f + t);
        gTan1_2[i] = FMOD_SQRT2 * t / (1.0f + t);
        gTan2_2[i] = FMOD_SQRT2 / (1.0f + t);

        for (j = 0; j < 2; j++)
        {
            float base = (float)pow(2.0, -0.25f * ((float)j + 1.0f));
            float p1 = 1.0f, p2 = 1.0f;

            if (i > 0)
            {
                if (i & 1)
                {
                    p1 = (float)pow(base, ((float)i + 1.0f) * 0.5f);
                }
                else
                {
                    p2 = (float)pow(base, (float)i * 0.5f);
                }
            }
            gPow1_1[j][i] = p1;
            gPow2_1[j][i] = p2;
            gPow1_2[j][i] = FMOD_SQRT2 * p1;
            gPow2_2[j][i] = FMOD_SQRT2 * p2;
        }
    }

    for (j = 0; j < 9; j++)
    {
        bandInfoStruct * bi = &gBandInfo[j];
        int * mp;
        int cb, lwin;
        int * bdf;

        mp = gMap[j][0] = gMapBuf0[j];
        bdf = bi->longDiff;
        for (i = 0, cb = 0; cb < 8; cb++, i += *bdf++)
        {
            *mp++ = (*bdf) >> 1;
            *mp++ = i;
            *mp++ = 3;
            *mp++ = cb;
        }
        bdf = bi->shortDiff + 3;
        for (cb = 3; cb < 13; cb++)
        {
            int l = (*bdf++) >> 1;

            for (lwin = 0; lwin < 3; lwin++)
            {
                *mp++ = l;
                *mp++ = i + lwin;
                *mp++ = lwin;
                *mp++ = cb;
            }
            i += 6 * l;
        }
        gMapEnd[j][0] = mp;

        mp = gMap[j][1] = gMapBuf1[j];
        bdf = bi->shortDiff + 0;
        for (i = 0, cb = 0; cb < 13; cb++)
        {
            int l = (*bdf++) >> 1;

            for (lwin = 0; lwin < 3; lwin++)
            {
                *mp++ = l;
                *mp++ = i + lwin;
                *mp++ = lwin;
                *mp++ = cb;
            }
            i += 6 * l;
        }
        gMapEnd[j][1] = mp;

        mp = gMap[j][2] = gMapBuf2[j];
        bdf = bi->longDiff;
        for (cb = 0; cb < 22; cb++)
        {
            *mp++ = (*bdf++) >> 1;
            *mp++ = cb;
        }
        gMapEnd[j][2] = mp;
    }

    for (i = 0; i < 9; i++)
    {
        for (j = 0; j < 23; j++)
        {
            gLongLimit[i][j] = (gBandInfo[i].longIdx[j] - 1 + 8) / 18 + 1;
            if (gLongLimit[i][j] > down_sample_sblimit)
            {
                gLongLimit[i][j] = down_sample_sblimit;
            }
        }
        for (j = 0; j < 14; j++)
        {
            gShortLimit[i][j] = (gBandInfo[i].shortIdx[j] - 1) / 18 + 1;
            if (gShortLimit[i][j] > down_sample_sblimit)
            {
                gShortLimit[i][j] = down_sample_sblimit;
            }
        }
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 6; j++)
        {
            for (k = 0; k < 6; k++)
            {
                int n = k + j * 6 + i * 36;

                gI_SLen2[n] = i | (j << 3) | (k << 6) | (3 << 12);
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 4; j++)
        {
            for (k = 0; k < 4; k++)
            {
                int n = k + j * 4 + i * 16;

                gI_SLen2[n + 180] = i | (j << 3) | (k << 6) | (4 << 12);
            }
        }
    }
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 3; j++)
        {
            int n = j + i * 3;

            gI_SLen2[n + 244] = i | (j << 3) | (5 << 12);
            gN_SLen2[n + 500] = i | (j << 3) | (2 << 12) | (1 << 15);
        }
    }

    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 4; k++)
            {
                for (l = 0; l < 4; l++)
                {
                    int n = l + k * 4 + j * 16 + i * 80;

                    gN_SLen2[n] = i | (j << 3) | (k << 6) | (l << 9) | (0 << 12);
                }
            }
        }
    }
    for (i = 0; i < 5; i++)
    {
        for (j = 0; j < 5; j++)
        {
            for (k = 0; k < 4; k++)
            {
                int n = k + j * 4 + i * 20;

                gN_SLen2[n + 400] = i | (j << 3) | (k << 6) | (1 << 12);
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_get_side_info_1(III_sideinfo * si, int stereo, int ms_stereo, int sfreq)
{
    int ch, gr;

    si->main_data_begin = getBits(9);
    if (stereo == 1)
    {
        si->private_bits = getBitsFast(5);
    }
    else
    {
        si->private_bits = getBitsFast(3);
    }

    for (ch = 0; ch < stereo; ch++)
    {
        si->ch[ch].gr[0].scfsi = -1;
        si->ch[ch].gr[1].scfsi = getBitsFast(4);
    }

    for (gr = 0; gr < 2; gr++)
    {
        for (ch = 0; ch < stereo; ch++)
        {
            gr_info_s * gr_info = &(si->ch[ch].gr[gr]);

            gr_info->part2_3_length = getBits(12);
            gr_info->big_values = getBitsFast(9);
            if (gr_info->big_values > 288)
            {
                return FMOD_ERR_FILE_BAD;
            }
            gr_info->pow2gain = gGainPow2 + 256 - getBitsFast(8);
            if (ms_stereo)
            {
                gr_info->pow2gain += 2;
            }
            gr_info->scalefac_compress = getBitsFast(4);

            if (get1Bit())
            {
                gr_info->block_type = getBitsFast(2);
                gr_info->mixed_block_flag = get1Bit();
                gr_info->table_select[0] = getBitsFast(5);
                gr_info->table_select[1] = getBitsFast(5);
                gr_info->table_select[2] = 0;
                gr_info->full_gain[0] = gr_info->pow2gain + (getBitsFast(3) << 3);
                gr_info->full_gain[1] = gr_info->pow2gain + (getBitsFast(3) << 3);
                gr_info->full_gain[2] = gr_info->pow2gain + (getBitsFast(3) << 3);

                if (gr_info->block_type == 0)
                {
                    return FMOD_ERR_FILE_BAD;
                }

                gr_info->region1start = 36 >> 1;
                gr_info->region2start = 576 >> 1;
            }
            else
            {
                int r0c, r1c;

                gr_info->table_select[0] = getBitsFast(5);
                gr_info->table_select[1] = getBitsFast(5);
                gr_info->table_select[2] = getBitsFast(5);
                r0c = getBitsFast(4);
                r1c = getBitsFast(3);
                gr_info->region1start = gBandInfo[sfreq].longIdx[r0c + 1] >> 1;
                gr_info->region2start = gBandInfo[sfreq].longIdx[r0c + 1 + r1c + 1] >> 1;
                gr_info->block_type = 0;
                gr_info->mixed_block_flag = 0;
            }
            gr_info->preflag = get1Bit();
            gr_info->scalefac_scale = get1Bit();
            gr_info->count1table_select = get1Bit();
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_get_side_info_2(III_sideinfo * si, int stereo, int ms_stereo, int sfreq)
{
    int ch;

    si->main_data_begin = getBits(8);
    if (stereo == 1)
    {
        si->private_bits = get1Bit();
    }
    else
    {
        si->private_bits = getBitsFast(2);
    }

    for (ch = 0; ch < stereo; ch++)
    {
        gr_info_s * gr_info = &(si->ch[ch].gr[0]);

        gr_info->part2_3_length = getBits(12);
        gr_info->big_values = getBitsFast(9);
        if (gr_info->big_values > 288)
        {
            return FMOD_ERR_FILE_BAD;
        }
        gr_info->pow2gain = gGainPow2 + 256 - getBitsFast(8);
        if (ms_stereo)
        {
            gr_info->pow2gain += 2;
        }
        gr_info->scalefac_compress = getBits(9);

        if (get1Bit())
        {
            int i;

            gr_info->block_type = getBitsFast(2);
            gr_info->mixed_block_flag = get1Bit();
            gr_info->table_select[0] = getBitsFast(5);
            gr_info->table_select[1] = getBitsFast(5);
            gr_info->table_select[2] = 0;
            for (i = 0; i < 3; i++)
            {
                gr_info->full_gain[i] = gr_info->pow2gain + (getBitsFast(3) << 3);
            }

            if (gr_info->block_type == 0)
            {
                return FMOD_ERR_FILE_BAD;
            }

            if (gr_info->block_type == 2)
            {
                gr_info->region1start = 36 >> 1;
            }
            else if (sfreq == 8)
            {
                gr_info->region1start = 108 >> 1;
            }
            else
            {
                gr_info->region1start = 54 >> 1;
            }
            gr_info->region2start = 576 >> 1;
        }
        else
        {
            int r0c, r1c;

            gr_info->table_select[0] = getBitsFast(5);
            gr_info->table_select[1] = getBitsFast(5);
            gr_info->table_select[2] = getBitsFast(5);
            r0c = getBitsFast(4);
            r1c = getBitsFast(3);
            gr_info->region1start = gBandInfo[sfreq].longIdx[r0c + 1] >> 1;
            gr_info->region2start = gBandInfo[sfreq].longIdx[r0c + 1 + r1c + 1] >> 1;
            gr_info->block_type = 0;
            gr_info->mixed_block_flag = 0;
        }
        gr_info->scalefac_scale = get1Bit();
        gr_info->count1table_select = get1Bit();
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_get_scale_factors_1(int * scf, gr_info_s * gr_info, int * numbits)
{
    static unsigned char slen[2][16] =
    {
        { 0, 0, 0, 0, 3, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4 },
        { 0, 1, 2, 3, 0, 1, 2, 3, 1, 2, 3, 1, 2, 3, 2, 3 }
    };
    int num0 = slen[0][gr_info->scalefac_compress];
    int num1 = slen[1][gr_info->scalefac_compress];

    *numbits = 0;

    if (gr_info->block_type == 2)
    {
        int i = 18;

        *numbits = (num0 + num1) * 18;

        if (gr_info->mixed_block_flag)
        {
            for (i = 8; i; i--)
            {
                *scf++ = getBitsFast(num0);
            }
            i = 9;
            *numbits -= num0;
        }

        for (; i; i--)
        {
            *scf++ = getBitsFast(num0);
        }
        for (i = 18; i; i--)
        {
            *scf++ = getBitsFast(num1);
        }
        *scf++ = 0;
        *scf++ = 0;
        *scf++ = 0;
    }
    else
    {
        int i;
        int scfsi = gr_info->scfsi;

        if (scfsi < 0)
        {
            for (i = 11; i; i--)
            {
                *scf++ = getBitsFast(num0);
            }
            for (i = 10; i; i--)
            {
                *scf++ = getBitsFast(num1);
            }
            *numbits = (num0 + num1) * 10 + num0;
        }
        else
        {
            *numbits = 0;
            if (!(scfsi & 0x8))
            {
                for (i = 6; i; i--)
                {
                    *scf++ = getBitsFast(num0);
                }
                *numbits += num0 * 6;
            }
            else
            {
                scf += 6;
            }

            if (!(scfsi & 0x4))
            {
                for (i = 5; i; i--)
                {
                    *scf++ = getBitsFast(num0);
                }
                *numbits += num0 * 5;
            }
            else
            {
                scf += 5;
            }

            if (!(scfsi & 0x2))
            {
                for (i = 5; i; i--)
                {
                    *scf++ = getBitsFast(num1);
                }
                *numbits += num1 * 5;
            }
            else
            {
                scf += 5;
            }

            if (!(scfsi & 0x1))
            {
                for (i = 5; i; i--)
                {
                    *scf++ = getBitsFast(num1);
                }
                *numbits += num1 * 5;
            }
            else
            {
                scf += 5;
            }
        }

        *scf++ = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_get_scale_factors_2(int * scf, gr_info_s * gr_info, int i_stereo, int * numbits)
{
    unsigned char * pnt;
    int i, j;
    unsigned int slen;
    int n = 0;
    static unsigned char stab[3][6][4] =
    {
        {
            { 6, 5, 5, 5 }, { 6, 5, 7, 3 }, { 11, 10, 0, 0 },
            { 7, 7, 7, 0 }, { 6, 6, 6, 3 }, { 8, 8, 5, 0 }
        },
        {
            { 9, 9, 9, 9 }, { 9, 9, 12, 6 }, { 18, 18, 0, 0 },
            { 12, 12, 12, 0 }, { 12, 9, 9, 6 }, { 15, 12, 9, 0 }
        },
        {
            { 6, 9, 9, 9 }, { 6, 9, 12, 6 }, { 15, 18, 0, 0 },
            { 6, 15, 12, 0 }, { 6, 12, 9, 6 }, { 6, 18, 9, 0 }
        }
    };

    *numbits = 0;

    if (i_stereo)
    {
        slen = gI_SLen2[gr_info->scalefac_compress >> 1];
    }
    else
    {
        slen = gN_SLen2[gr_info->scalefac_compress];
    }

    gr_info->preflag = (slen >> 15) & 0x1;

    n = 0;
    if (gr_info->block_type == 2)
    {
        n++;
        if (gr_info->mixed_block_flag)
        {
            n++;
        }
    }

    pnt = stab[n][(slen >> 12) & 0x7];

    for (i = 0; i < 4; i++)
    {
        int num = slen & 0x7;

        slen >>= 3;
        if (num)
        {
            for (j = 0; j < (int)(pnt[i]); j++)
            {
                *scf++ = getBitsFast(num);
            }
            *numbits += pnt[i] * num;
        }
        else
        {
            for (j = 0; j < (int)(pnt[i]); j++)
            {
                *scf++ = 0;
            }
        }
    }

    n = (n << 1) + 1;
    for (i = 0; i < n; i++)
    {
        *scf++ = 0;
    }

    return FMOD_OK;
}

static int pretab1[22] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0
};

static int pretab2[22] =
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

FMOD_RESULT CodecMPEG::III_dequantize_sample(float (* xr)[18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits)
{
    int shift = 1 + gr_info->scalefac_scale;
    float * xrpnt = (float *)xr;
    int l[3], l3;
    int part2remain = gr_info->part2_3_length - part2bits;
    int * me;

    {
        int bv = gr_info->big_values;
        int region1 = gr_info->region1start;
        int region2 = gr_info->region2start;

        l3 = ((576 >> 1) - bv) >> 1;

        if (bv <= region1)
        {
            l[0] = bv;
            l[1] = 0;
            l[2] = 0;
        }
        else
        {
            l[0] = region1;
            if (bv <= region2)
            {
                l[1] = bv - l[0];
                l[2] = 0;
            }
            else
            {
                l[1] = region2 - l[0];
                l[2] = bv - region2;
            }
        }
    }

    if (l[0] < 0)
    {
        l[0] = 0;
    }
    if (l[1] < 0)
    {
        l[1] = 0;
    }
    if (l[2] < 0)
    {
        l[2] = 0;
    }

    if (gr_info->block_type == 2)
    {
        int i, max[4];
        int step = 0, lwin = 0, cb = 0;
        float v = 0.0f;
        int * m, mc;

        if (gr_info->mixed_block_flag)
        {
            max[3] = -1;
            max[0] = max[1] = max[2] = 2;
            m = gMap[sfreq][0];
            me = gMapEnd[sfreq][0];
        }
        else
        {
            max[0] = max[1] = max[2] = max[3] = -1;
            m = gMap[sfreq][1];
            me = gMapEnd[sfreq][1];
        }

        mc = 0;
        for (i = 0; i < 2; i++)
        {
            int lp = l[i];
            newhuff * h = ht + gr_info->table_select[i];

            for (; lp; lp--, mc--)
            {
                int x, y;

                if (!mc)
                {
                    mc = *m++;
                    xrpnt = ((float *)xr) + (*m++);
                    lwin = *m++;
                    cb = *m++;
                    if (lwin == 3)
                    {
                        v = gr_info->pow2gain[(*scf++) << shift];
                        step = 1;
                    }
                    else
                    {
                        v = gr_info->full_gain[lwin][(*scf++) << shift];
                        step = 3;
                    }
                }
                {
                    short * val = h->table;

                    while ((y = *val++) < 0)
                    {
                        if (get1Bit())
                        {
                            val -= y;
                        }
                        part2remain--;
                    }
                    x = y >> 4;
                    y &= 0xF;
                }
                if (x == 15)
                {
                    max[lwin] = cb;
                    part2remain -= h->linbits + 1;
                    x += getBits(h->linbits);
                    if (get1Bit())
                    {
                        *xrpnt = -gIsPowTable[x] * v;
                    }
                    else
                    {
                        *xrpnt = gIsPowTable[x] * v;
                    }
                }
                else if (x)
                {
                    max[lwin] = cb;
                    if (get1Bit())
                    {
                        *xrpnt = -gIsPowTable[x] * v;
                    }
                    else
                    {
                        *xrpnt = gIsPowTable[x] * v;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt = 0.0f;
                }
                xrpnt += step;
                if (y == 15)
                {
                    max[lwin] = cb;
                    part2remain -= h->linbits + 1;
                    y += getBits(h->linbits);
                    if (get1Bit())
                    {
                        *xrpnt = -gIsPowTable[y] * v;
                    }
                    else
                    {
                        *xrpnt = gIsPowTable[y] * v;
                    }
                }
                else if (y)
                {
                    max[lwin] = cb;
                    if (get1Bit())
                    {
                        *xrpnt = -gIsPowTable[y] * v;
                    }
                    else
                    {
                        *xrpnt = gIsPowTable[y] * v;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt = 0.0f;
                }
                xrpnt += step;
            }
        }

        for (; l3 && (part2remain > 0); l3--)
        {
            newhuff * h = htc + gr_info->count1table_select;
            short * val = h->table, a;

            while ((a = *val++) < 0)
            {
                part2remain--;
                if (part2remain < 0)
                {
                    part2remain++;
                    a = 0;
                    break;
                }
                if (get1Bit())
                {
                    val -= a;
                }
            }

            for (i = 0; i < 4; i++)
            {
                if (!(i & 1))
                {
                    if (!mc)
                    {
                        mc = *m++;
                        xrpnt = ((float *)xr) + (*m++);
                        lwin = *m++;
                        cb = *m++;
                        if (lwin == 3)
                        {
                            v = gr_info->pow2gain[(*scf++) << shift];
                            step = 1;
                        }
                        else
                        {
                            v = gr_info->full_gain[lwin][(*scf++) << shift];
                            step = 3;
                        }
                    }
                    mc--;
                }
                if ((a & (0x8 >> i)))
                {
                    max[lwin] = cb;
                    part2remain--;
                    if (part2remain < 0)
                    {
                        part2remain++;
                        break;
                    }
                    if (get1Bit())
                    {
                        *xrpnt = -v;
                    }
                    else
                    {
                        *xrpnt = v;
                    }
                }
                else
                {
                    *xrpnt = 0.0f;
                }
                xrpnt += step;
            }
        }

        while (m < me)
        {
            if (!mc)
            {
                mc = *m++;
                xrpnt = ((float *)xr) + *m++;
                if ((*m++) == 3)
                {
                    step = 1;
                }
                else
                {
                    step = 3;
                }
                m++;
            }
            mc--;
            *xrpnt = 0.0f;
            xrpnt += step;
            *xrpnt = 0.0f;
            xrpnt += step;
        }

        gr_info->maxband[0] = max[0] + 1;
        gr_info->maxband[1] = max[1] + 1;
        gr_info->maxband[2] = max[2] + 1;
        gr_info->maxbandl = max[3] + 1;

        {
            int rmax = max[0] > max[1] ? max[0] : max[1];

            rmax = (rmax > max[2] ? rmax : max[2]) + 1;
            gr_info->maxb = rmax ? gShortLimit[sfreq][rmax] : gLongLimit[sfreq][max[3] + 1];
        }
    }
    else
    {
        int * pretab = gr_info->preflag ? pretab1 : pretab2;
        int i, max = -1;
        int cb = 0;
        int * m = gMap[sfreq][2];
        float v = 0.0f;
        int mc = 0;

        for (i = 0; i < 3; i++)
        {
            int lp = l[i];
            newhuff * h = ht + gr_info->table_select[i];

            for (; lp; lp--, mc--)
            {
                int x, y;

                if (!mc)
                {
                    mc = *m++;
                    v = gr_info->pow2gain[((*scf++) + (*pretab++)) << shift];
                    cb = *m++;
                }
                {
                    short * val = h->table;

                    while ((y = *val++) < 0)
                    {
                        if (get1Bit())
                        {
                            val -= y;
                        }
                        part2remain--;
                    }
                    x = y >> 4;
                    y &= 0xF;
                }
                if (x == 15)
                {
                    max = cb;
                    part2remain -= h->linbits + 1;
                    x += getBits(h->linbits);
                    if (get1Bit())
                    {
                        *xrpnt++ = -gIsPowTable[x] * v;
                    }
                    else
                    {
                        *xrpnt++ = gIsPowTable[x] * v;
                    }
                }
                else if (x)
                {
                    max = cb;
                    if (get1Bit())
                    {
                        *xrpnt++ = -gIsPowTable[x] * v;
                    }
                    else
                    {
                        *xrpnt++ = gIsPowTable[x] * v;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt++ = 0.0f;
                }
                if (y == 15)
                {
                    max = cb;
                    part2remain -= h->linbits + 1;
                    y += getBits(h->linbits);
                    if (get1Bit())
                    {
                        *xrpnt++ = -gIsPowTable[y] * v;
                    }
                    else
                    {
                        *xrpnt++ = gIsPowTable[y] * v;
                    }
                }
                else if (y)
                {
                    max = cb;
                    if (get1Bit())
                    {
                        *xrpnt++ = -gIsPowTable[y] * v;
                    }
                    else
                    {
                        *xrpnt++ = gIsPowTable[y] * v;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt++ = 0.0f;
                }
            }
        }

        for (; l3 && (part2remain > 0); l3--)
        {
            newhuff * h = htc + gr_info->count1table_select;
            short * val = h->table, a;

            while ((a = *val++) < 0)
            {
                part2remain--;
                if (part2remain < 0)
                {
                    part2remain++;
                    a = 0;
                    break;
                }
                if (get1Bit())
                {
                    val -= a;
                }
            }

            for (i = 0; i < 4; i++)
            {
                if (!(i & 1))
                {
                    if (!mc)
                    {
                        mc = *m++;
                        cb = *m++;
                        v = gr_info->pow2gain[((*scf++) + (*pretab++)) << shift];
                    }
                    mc--;
                }
                if ((a & (0x8 >> i)))
                {
                    max = cb;
                    part2remain--;
                    if (part2remain < 0)
                    {
                        part2remain++;
                        break;
                    }
                    if (get1Bit())
                    {
                        *xrpnt++ = -v;
                    }
                    else
                    {
                        *xrpnt++ = v;
                    }
                }
                else
                {
                    *xrpnt++ = 0.0f;
                }
            }
        }

        for (i = (&xr[32][0] - xrpnt) >> 1; i; i--)
        {
            *xrpnt++ = 0.0f;
            *xrpnt++ = 0.0f;
        }

        gr_info->maxbandl = max + 1;
        gr_info->maxb = gLongLimit[sfreq][gr_info->maxbandl];
    }

    while (part2remain > 16)
    {
        getBits(16);
        part2remain -= 16;
    }
    if (part2remain > 0)
    {
        getBits(part2remain);
    }
    else if (part2remain < 0)
    {
        return FMOD_ERR_FILE_BAD;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_dequantize_sample_ms(float (* xr)[32][18], int * scf, gr_info_s * gr_info, int sfreq, int part2bits)
{
    int shift = 1 + gr_info->scalefac_scale;
    float * xrpnt = (float *)xr[1];
    float * xr0pnt = (float *)xr[0];
    int l[3], l3;
    int part2remain = gr_info->part2_3_length - part2bits;
    int * me;

    {
        int bv = gr_info->big_values;
        int region1 = gr_info->region1start;
        int region2 = gr_info->region2start;

        l3 = ((576 >> 1) - bv) >> 1;

        if (bv <= region1)
        {
            l[0] = bv;
            l[1] = 0;
            l[2] = 0;
        }
        else
        {
            l[0] = region1;
            if (bv <= region2)
            {
                l[1] = bv - l[0];
                l[2] = 0;
            }
            else
            {
                l[1] = region2 - l[0];
                l[2] = bv - region2;
            }
        }
    }

    {
        int i;

        for (i = 0; i < 3; i++)
        {
            if (l[i] < 0)
            {
                l[i] = 0;
            }
        }
    }

    if (gr_info->block_type == 2)
    {
        int i, max[4];
        int step = 0, lwin = 0, cb = 0;
        float v = 0.0f;
        int * m, mc = 0;

        if (gr_info->mixed_block_flag)
        {
            max[3] = -1;
            max[0] = max[1] = max[2] = 2;
            m = gMap[sfreq][0];
            me = gMapEnd[sfreq][0];
        }
        else
        {
            max[0] = max[1] = max[2] = max[3] = -1;
            m = gMap[sfreq][1];
            me = gMapEnd[sfreq][1];
        }

        for (i = 0; i < 2; i++)
        {
            int lp = l[i];
            newhuff * h = ht + gr_info->table_select[i];

            for (; lp; lp--, mc--)
            {
                int x, y;

                if (!mc)
                {
                    mc = *m++;
                    xrpnt = ((float *)xr[1]) + *m;
                    xr0pnt = ((float *)xr[0]) + *m++;
                    lwin = *m++;
                    cb = *m++;
                    if (lwin == 3)
                    {
                        v = gr_info->pow2gain[(*scf++) << shift];
                        step = 1;
                    }
                    else
                    {
                        v = gr_info->full_gain[lwin][(*scf++) << shift];
                        step = 3;
                    }
                }
                {
                    short * val = h->table;

                    while ((y = *val++) < 0)
                    {
                        if (get1Bit())
                        {
                            val -= y;
                        }
                        part2remain--;
                    }
                    x = y >> 4;
                    y &= 0xF;
                }
                if (x == 15)
                {
                    max[lwin] = cb;
                    part2remain -= h->linbits + 1;
                    x += getBits(h->linbits);
                    if (get1Bit())
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt = *xr0pnt + a;
                        *xr0pnt -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt = *xr0pnt - a;
                        *xr0pnt += a;
                    }
                }
                else if (x)
                {
                    max[lwin] = cb;
                    if (get1Bit())
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt = *xr0pnt + a;
                        *xr0pnt -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt = *xr0pnt - a;
                        *xr0pnt += a;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt = *xr0pnt;
                }
                xrpnt += step;
                xr0pnt += step;
                if (y == 15)
                {
                    max[lwin] = cb;
                    part2remain -= h->linbits + 1;
                    y += getBits(h->linbits);
                    if (get1Bit())
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt = *xr0pnt + a;
                        *xr0pnt -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt = *xr0pnt - a;
                        *xr0pnt += a;
                    }
                }
                else if (y)
                {
                    max[lwin] = cb;
                    if (get1Bit())
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt = *xr0pnt + a;
                        *xr0pnt -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt = *xr0pnt - a;
                        *xr0pnt += a;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt = *xr0pnt;
                }
                xrpnt += step;
                xr0pnt += step;
            }
        }

        for (; l3 && (part2remain > 0); l3--)
        {
            newhuff * h = htc + gr_info->count1table_select;
            short * val = h->table, a;

            while ((a = *val++) < 0)
            {
                part2remain--;
                if (part2remain < 0)
                {
                    part2remain++;
                    a = 0;
                    break;
                }
                if (get1Bit())
                {
                    val -= a;
                }
            }

            for (i = 0; i < 4; i++)
            {
                if (!(i & 1))
                {
                    if (!mc)
                    {
                        mc = *m++;
                        xrpnt = ((float *)xr[1]) + *m;
                        xr0pnt = ((float *)xr[0]) + *m++;
                        lwin = *m++;
                        cb = *m++;
                        if (lwin == 3)
                        {
                            v = gr_info->pow2gain[(*scf++) << shift];
                            step = 1;
                        }
                        else
                        {
                            v = gr_info->full_gain[lwin][(*scf++) << shift];
                            step = 3;
                        }
                    }
                    mc--;
                }
                if ((a & (0x8 >> i)))
                {
                    max[lwin] = cb;
                    part2remain--;
                    if (part2remain < 0)
                    {
                        part2remain++;
                        break;
                    }
                    if (get1Bit())
                    {
                        *xrpnt = *xr0pnt + v;
                        *xr0pnt -= v;
                    }
                    else
                    {
                        *xrpnt = *xr0pnt - v;
                        *xr0pnt += v;
                    }
                }
                else
                {
                    *xrpnt = *xr0pnt;
                }
                xrpnt += step;
                xr0pnt += step;
            }
        }

        while (m < me)
        {
            if (!mc)
            {
                mc = *m++;
                xrpnt = ((float *)xr[1]) + *m;
                xr0pnt = ((float *)xr[0]) + *m++;
                if ((*m++) == 3)
                {
                    step = 1;
                }
                else
                {
                    step = 3;
                }
                m++;
            }
            mc--;
            *xrpnt = *xr0pnt;
            xrpnt += step;
            xr0pnt += step;
            *xrpnt = *xr0pnt;
            xrpnt += step;
            xr0pnt += step;
        }

        gr_info->maxband[0] = max[0] + 1;
        gr_info->maxband[1] = max[1] + 1;
        gr_info->maxband[2] = max[2] + 1;
        gr_info->maxbandl = max[3] + 1;

        {
            int rmax = max[0] > max[1] ? max[0] : max[1];

            rmax = (rmax > max[2] ? rmax : max[2]) + 1;
            gr_info->maxb = rmax ? gShortLimit[sfreq][rmax] : gLongLimit[sfreq][max[3] + 1];
        }
    }
    else
    {
        int * pretab = gr_info->preflag ? pretab1 : pretab2;
        int i, max = -1;
        int cb = 0;
        int mc = 0;
        int * m = gMap[sfreq][2];
        float v = 0.0f;

        for (i = 0; i < 3; i++)
        {
            int lp = l[i];
            newhuff * h = ht + gr_info->table_select[i];

            for (; lp; lp--, mc--)
            {
                int x, y;

                if (!mc)
                {
                    mc = *m++;
                    cb = *m++;
                    v = gr_info->pow2gain[((*scf++) + (*pretab++)) << shift];
                }
                {
                    short * val = h->table;

                    while ((y = *val++) < 0)
                    {
                        if (get1Bit())
                        {
                            val -= y;
                        }
                        part2remain--;
                    }
                    x = y >> 4;
                    y &= 0xF;
                }
                if (x == 15)
                {
                    max = cb;
                    part2remain -= h->linbits + 1;
                    x += getBits(h->linbits);
                    if (get1Bit())
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt++ = *xr0pnt + a;
                        *xr0pnt++ -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt++ = *xr0pnt - a;
                        *xr0pnt++ += a;
                    }
                }
                else if (x)
                {
                    max = cb;
                    if (get1Bit())
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt++ = *xr0pnt + a;
                        *xr0pnt++ -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[x] * v;

                        *xrpnt++ = *xr0pnt - a;
                        *xr0pnt++ += a;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt++ = *xr0pnt++;
                }
                if (y == 15)
                {
                    max = cb;
                    part2remain -= h->linbits + 1;
                    y += getBits(h->linbits);
                    if (get1Bit())
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt++ = *xr0pnt + a;
                        *xr0pnt++ -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt++ = *xr0pnt - a;
                        *xr0pnt++ += a;
                    }
                }
                else if (y)
                {
                    max = cb;
                    if (get1Bit())
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt++ = *xr0pnt + a;
                        *xr0pnt++ -= a;
                    }
                    else
                    {
                        float a = gIsPowTable[y] * v;

                        *xrpnt++ = *xr0pnt - a;
                        *xr0pnt++ += a;
                    }
                    part2remain--;
                }
                else
                {
                    *xrpnt++ = *xr0pnt++;
                }
            }
        }

        for (; l3 && (part2remain > 0); l3--)
        {
            newhuff * h = htc + gr_info->count1table_select;
            short * val = h->table, a;

            while ((a = *val++) < 0)
            {
                part2remain--;
                if (part2remain < 0)
                {
                    part2remain++;
                    a = 0;
                    break;
                }
                if (get1Bit())
                {
                    val -= a;
                }
            }

            for (i = 0; i < 4; i++)
            {
                if (!(i & 1))
                {
                    if (!mc)
                    {
                        mc = *m++;
                        cb = *m++;
                        v = gr_info->pow2gain[((*scf++) + (*pretab++)) << shift];
                    }
                    mc--;
                }
                if ((a & (0x8 >> i)))
                {
                    max = cb;
                    part2remain--;
                    if (part2remain <= 0)
                    {
                        part2remain++;
                        break;
                    }
                    if (get1Bit())
                    {
                        *xrpnt++ = *xr0pnt + v;
                        *xr0pnt++ -= v;
                    }
                    else
                    {
                        *xrpnt++ = *xr0pnt - v;
                        *xr0pnt++ += v;
                    }
                }
                else
                {
                    *xrpnt++ = *xr0pnt++;
                }
            }
        }

        for (i = (&xr[1][32][0] - xrpnt) >> 1; i; i--)
        {
            *xrpnt++ = *xr0pnt++;
            *xrpnt++ = *xr0pnt++;
        }

        gr_info->maxbandl = max + 1;
        gr_info->maxb = gLongLimit[sfreq][gr_info->maxbandl];
    }

    while (part2remain > 16)
    {
        getBits(16);
        part2remain -= 16;
    }
    if (part2remain > 0)
    {
        getBits(part2remain);
    }
    else if (part2remain < 0)
    {
        return FMOD_ERR_FILE_BAD;
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_i_stereo(float (* xr_buf)[32][18], int * scalefac, gr_info_s * gr_info, int sfreq, int ms_stereo, int lsf)
{
    float (* xr)[32 * 18] = (float (*)[32 * 18])xr_buf;
    bandInfoStruct * bi = &gBandInfo[sfreq];
    float * tab1, * tab2;

    if (lsf)
    {
        int p = gr_info->scalefac_compress & 0x1;

        if (ms_stereo)
        {
            tab1 = gPow1_2[p];
            tab2 = gPow2_2[p];
        }
        else
        {
            tab1 = gPow1_1[p];
            tab2 = gPow2_1[p];
        }
    }
    else
    {
        if (ms_stereo)
        {
            tab1 = gTan1_2;
            tab2 = gTan2_2;
        }
        else
        {
            tab1 = gTan1_1;
            tab2 = gTan2_1;
        }
    }

    if (gr_info->block_type == 2)
    {
        int lwin, do_l = 0;

        if (gr_info->mixed_block_flag)
        {
            do_l = 1;
        }

        for (lwin = 0; lwin < 3; lwin++)
        {
            int is_p, sb, idx, sfb = gr_info->maxband[lwin];

            if (sfb > 3)
            {
                do_l = 0;
            }

            for (; sfb < 12; sfb++)
            {
                is_p = scalefac[sfb * 3 + lwin - gr_info->mixed_block_flag];
                if (is_p != 7)
                {
                    float t1, t2;

                    sb = bi->shortDiff[sfb];
                    idx = bi->shortIdx[sfb] + lwin;
                    t1 = tab1[is_p];
                    t2 = tab2[is_p];
                    for (; sb > 0; sb--, idx += 3)
                    {
                        float v = xr[0][idx];

                        xr[0][idx] = v * t1;
                        xr[1][idx] = v * t2;
                    }
                }
            }

            is_p = scalefac[11 * 3 + lwin - gr_info->mixed_block_flag];
            sb = bi->shortDiff[12];
            idx = bi->shortIdx[12] + lwin;
            if (is_p != 7)
            {
                float t1, t2;

                t1 = tab1[is_p];
                t2 = tab2[is_p];
                for (; sb > 0; sb--, idx += 3)
                {
                    float v = xr[0][idx];

                    xr[0][idx] = v * t1;
                    xr[1][idx] = v * t2;
                }
            }
        }

        if (do_l)
        {
            int sfb = gr_info->maxbandl;
            int idx = bi->longIdx[sfb];

            for (; sfb < 8; sfb++)
            {
                int sb = bi->longDiff[sfb];
                int is_p = scalefac[sfb];

                if (is_p != 7)
                {
                    float t1, t2;

                    t1 = tab1[is_p];
                    t2 = tab2[is_p];
                    for (; sb > 0; sb--, idx++)
                    {
                        float v = xr[0][idx];

                        xr[0][idx] = v * t1;
                        xr[1][idx] = v * t2;
                    }
                }
                else
                {
                    idx += sb;
                }
            }
        }
    }
    else
    {
        int sfb = gr_info->maxbandl;
        int is_p, idx = bi->longIdx[sfb];

        for (; sfb < 21; sfb++)
        {
            int sb = bi->longDiff[sfb];

            is_p = scalefac[sfb];
            if (is_p != 7)
            {
                float t1, t2;

                t1 = tab1[is_p];
                t2 = tab2[is_p];
                for (; sb > 0; sb--, idx++)
                {
                    float v = xr[0][idx];

                    xr[0][idx] = v * t1;
                    xr[1][idx] = v * t2;
                }
            }
            else
            {
                idx += sb;
            }
        }

        is_p = scalefac[20];
        if (is_p != 7)
        {
            int sb;
            float t1 = tab1[is_p], t2 = tab2[is_p];

            for (sb = bi->longDiff[21]; sb > 0; sb--, idx++)
            {
                float v = xr[0][idx];

                xr[0][idx] = v * t1;
                xr[1][idx] = v * t2;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::III_antialias(float (* xr)[18], gr_info_s * gr_info)
{
    int sblim;

    if (gr_info->block_type == 2)
    {
        if (!gr_info->mixed_block_flag)
        {
            return FMOD_ERR_FILE_BAD;
        }
        sblim = 1;
    }
    else
    {
        sblim = gr_info->maxb - 1;
    }

    {
        int sb;
        float * xr1 = (float *)xr[1];

        for (sb = sblim; sb; sb--, xr1 += 10)
        {
            int ss;
            float * cs = gAaCs, * ca = gAaCa;
            float * xr2 = xr1;

            for (ss = 7; ss >= 0; ss--)
            {
                float bu = *--xr2, bd = *xr1;

                *xr2 = (bu * (*cs)) - (bd * (*ca));
                *xr1++ = (bd * (*cs++)) + (bu * (*ca++));
            }
        }
    }

    return FMOD_OK;
}

void CodecMPEG::dct36(float * inbuf, float * o1, float * o2, float * wintab, float * tsbuf)
{
    float tmp[18];

    {
        float * in = inbuf;

        in[17] += in[16]; in[16] += in[15]; in[15] += in[14];
        in[14] += in[13]; in[13] += in[12]; in[12] += in[11];
        in[11] += in[10]; in[10] += in[9];  in[9]  += in[8];
        in[8]  += in[7];  in[7]  += in[6];  in[6]  += in[5];
        in[5]  += in[4];  in[4]  += in[3];  in[3]  += in[2];
        in[2]  += in[1];  in[1]  += in[0];

        in[17] += in[15]; in[15] += in[13]; in[13] += in[11]; in[11] += in[9];
        in[9]  += in[7];  in[7]  += in[5];  in[5]  += in[3];  in[3]  += in[1];

        {
            float t0, t1, t2, t3, t4, t5, t6, t7;

            t1 = gCos6_2 * in[12];
            t2 = gCos6_2 * (in[8] + in[16] - in[4]);

            t3 = in[0] + t1;
            t4 = in[0] - t1 - t1;
            t5 = t4 - t2;

            t0 = gCos9[0] * (in[4] + in[8]);
            t1 = gCos9[1] * (in[8] - in[16]);

            tmp[4] = t4 + t2 + t2;
            t2 = gCos9[2] * (in[4] + in[16]);

            t6 = t3 - t0 - t2;
            t0 += t3 + t1;
            t3 += t2 - t1;

            t2 = gCos18[0] * (in[2] + in[10]);
            t4 = gCos18[1] * (in[10] - in[14]);
            t7 = gCos6_1 * in[6];

            t1 = t2 + t4 + t7;
            tmp[0] = t0 + t1;
            tmp[8] = t0 - t1;
            t1 = gCos18[2] * (in[2] + in[14]);
            t2 += t1 - t7;

            tmp[3] = t3 + t2;
            t0 = gCos6_1 * (in[10] + in[14] - in[2]);
            tmp[5] = t3 - t2;

            t4 -= t1 + t7;

            tmp[1] = t5 - t0;
            tmp[7] = t5 + t0;
            tmp[2] = t6 + t4;
            tmp[6] = t6 - t4;
        }

        {
            float t0, t1, t2, t3, t4, t5, t6, t7;

            t1 = gCos6_2 * in[13];
            t2 = gCos6_2 * (in[9] + in[17] - in[5]);

            t3 = in[1] + t1;
            t4 = in[1] - t1 - t1;
            t5 = t4 - t2;

            t0 = gCos9[0] * (in[5] + in[9]);
            t1 = gCos9[1] * (in[9] - in[17]);

            tmp[13] = (t4 + t2 + t2) * gTfCos36[17 - 13];
            t2 = gCos9[2] * (in[5] + in[17]);

            t6 = t3 - t0 - t2;
            t0 += t3 + t1;
            t3 += t2 - t1;

            t2 = gCos18[0] * (in[3] + in[11]);
            t4 = gCos18[1] * (in[11] - in[15]);
            t7 = gCos6_1 * in[7];

            t1 = t2 + t4 + t7;
            tmp[17] = (t0 + t1) * gTfCos36[17 - 17];
            tmp[9] = (t0 - t1) * gTfCos36[17 - 9];
            t1 = gCos18[2] * (in[3] + in[15]);
            t2 += t1 - t7;

            tmp[14] = (t3 + t2) * gTfCos36[17 - 14];
            t0 = gCos6_1 * (in[11] + in[15] - in[3]);
            tmp[12] = (t3 - t2) * gTfCos36[17 - 12];

            t4 -= t1 + t7;

            tmp[16] = (t5 - t0) * gTfCos36[17 - 16];
            tmp[10] = (t5 + t0) * gTfCos36[17 - 10];
            tmp[15] = (t6 + t4) * gTfCos36[17 - 15];
            tmp[11] = (t6 - t4) * gTfCos36[17 - 11];
        }
    }

    {
        float * out2 = o2;
        float * w = wintab;
        float * out1 = o1;
        float * ts = tsbuf;

        {
            float tmpval;
            float sum0 = tmp[0];
            float sum1 = tmp[17];

            out2[9] = (tmpval = sum0 + sum1) * w[27];
            out2[8] = tmpval * w[26];
            sum0 -= sum1;
            ts[32 * 8] = out1[8] + sum0 * w[8];
            ts[32 * 9] = out1[9] + sum0 * w[9];
        }
        {
            float tmpval;
            float sum0 = tmp[1];
            float sum1 = tmp[16];

            out2[10] = (tmpval = sum0 + sum1) * w[28];
            out2[7] = tmpval * w[25];
            sum0 -= sum1;
            ts[32 * 7] = out1[7] + sum0 * w[7];
            ts[32 * 10] = out1[10] + sum0 * w[10];
        }
        {
            float tmpval;
            float sum0 = tmp[2];
            float sum1 = tmp[15];

            out2[11] = (tmpval = sum0 + sum1) * w[29];
            out2[6] = tmpval * w[24];
            sum0 -= sum1;
            ts[32 * 6] = out1[6] + sum0 * w[6];
            ts[32 * 11] = out1[11] + sum0 * w[11];
        }
        {
            float tmpval;
            float sum0 = tmp[3];
            float sum1 = tmp[14];

            out2[12] = (tmpval = sum0 + sum1) * w[30];
            out2[5] = tmpval * w[23];
            sum0 -= sum1;
            ts[32 * 5] = out1[5] + sum0 * w[5];
            ts[32 * 12] = out1[12] + sum0 * w[12];
        }
        {
            float tmpval;
            float sum0 = tmp[4];
            float sum1 = tmp[13];

            out2[13] = (tmpval = sum0 + sum1) * w[31];
            out2[4] = tmpval * w[22];
            sum0 -= sum1;
            ts[32 * 4] = out1[4] + sum0 * w[4];
            ts[32 * 13] = out1[13] + sum0 * w[13];
        }
        {
            float tmpval;
            float sum0 = tmp[5];
            float sum1 = tmp[12];

            out2[14] = (tmpval = sum0 + sum1) * w[32];
            out2[3] = tmpval * w[21];
            sum0 -= sum1;
            ts[32 * 3] = out1[3] + sum0 * w[3];
            ts[32 * 14] = out1[14] + sum0 * w[14];
        }
        {
            float tmpval;
            float sum0 = tmp[6];
            float sum1 = tmp[11];

            out2[15] = (tmpval = sum0 + sum1) * w[33];
            out2[2] = tmpval * w[20];
            sum0 -= sum1;
            ts[32 * 2] = out1[2] + sum0 * w[2];
            ts[32 * 15] = out1[15] + sum0 * w[15];
        }
        {
            float tmpval;
            float sum0 = tmp[7];
            float sum1 = tmp[10];

            out2[16] = (tmpval = sum0 + sum1) * w[34];
            out2[1] = tmpval * w[19];
            sum0 -= sum1;
            ts[32 * 1] = out1[1] + sum0 * w[1];
            ts[32 * 16] = out1[16] + sum0 * w[16];
        }
        {
            float tmpval;
            float sum0 = tmp[8];
            float sum1 = tmp[9];

            out2[17] = (tmpval = sum0 + sum1) * w[35];
            out2[0] = tmpval * w[18];
            sum0 -= sum1;
            ts[32 * 0] = out1[0] + sum0 * w[0];
            ts[32 * 17] = out1[17] + sum0 * w[17];
        }
    }
}

void CodecMPEG::dct12(float * in, float * rawout1, float * rawout2, float * wi, float * ts)
{
    {
        float in0, in1, in2, in3, in4, in5;
        float * out1 = rawout1;

        ts[32 * 0] = out1[0]; ts[32 * 1] = out1[1]; ts[32 * 2] = out1[2];
        ts[32 * 15] = out1[15]; ts[32 * 16] = out1[16]; ts[32 * 17] = out1[17];

        in5 = in[5 * 3];
        in5 += (in4 = in[4 * 3]);
        in4 += (in3 = in[3 * 3]);
        in3 += (in2 = in[2 * 3]);
        in2 += (in1 = in[1 * 3]);
        in1 += (in0 = in[0 * 3]);

        in5 += in3; in3 += in1;

        in2 *= gCos6_1;
        in3 *= gCos6_1;

        {
            float tmp0, tmp1 = (in0 - in4);

            {
                float tmp2 = (in1 - in5) * gTfCos12[1];

                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            ts[(17 - 1) * 32] = out1[17 - 1] + tmp0 * wi[11 - 1];
            ts[(12 + 1) * 32] = out1[12 + 1] + tmp0 * wi[6 + 1];
            ts[(6 + 1) * 32] = out1[6 + 1] + tmp1 * wi[1];
            ts[(11 - 1) * 32] = out1[11 - 1] + tmp1 * wi[5 - 1];
        }

        in0 += in4 * gCos6_2;

        in4 = in0 + in2;
        in0 -= in2;

        in1 += in5 * gCos6_2;

        in5 = (in1 + in3) * gTfCos12[0];
        in1 = (in1 - in3) * gTfCos12[2];

        in3 = in4 + in5;
        in4 -= in5;

        in2 = in0 + in1;
        in0 -= in1;

        ts[(17 - 0) * 32] = out1[17 - 0] + in2 * wi[11 - 0];
        ts[(12 + 0) * 32] = out1[12 + 0] + in2 * wi[6 + 0];
        ts[(12 + 2) * 32] = out1[12 + 2] + in3 * wi[6 + 2];
        ts[(17 - 2) * 32] = out1[17 - 2] + in3 * wi[11 - 2];

        ts[(6 + 0) * 32] = out1[6 + 0] + in0 * wi[0];
        ts[(11 - 0) * 32] = out1[11 - 0] + in0 * wi[5 - 0];
        ts[(6 + 2) * 32] = out1[6 + 2] + in4 * wi[2];
        ts[(11 - 2) * 32] = out1[11 - 2] + in4 * wi[5 - 2];
    }

    in++;

    {
        float in0, in1, in2, in3, in4, in5;
        float * out2 = rawout2;

        in5 = in[5 * 3];
        in5 += (in4 = in[4 * 3]);
        in4 += (in3 = in[3 * 3]);
        in3 += (in2 = in[2 * 3]);
        in2 += (in1 = in[1 * 3]);
        in1 += (in0 = in[0 * 3]);

        in5 += in3; in3 += in1;

        in2 *= gCos6_1;
        in3 *= gCos6_1;

        {
            float tmp0, tmp1 = (in0 - in4);

            {
                float tmp2 = (in1 - in5) * gTfCos12[1];

                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            out2[5 - 1] = tmp0 * wi[11 - 1];
            out2[0 + 1] = tmp0 * wi[6 + 1];
            ts[(12 + 1) * 32] += tmp1 * wi[1];
            ts[(17 - 1) * 32] += tmp1 * wi[5 - 1];
        }

        in0 += in4 * gCos6_2;

        in4 = in0 + in2;
        in0 -= in2;

        in1 += in5 * gCos6_2;

        in5 = (in1 + in3) * gTfCos12[0];
        in1 = (in1 - in3) * gTfCos12[2];

        in3 = in4 + in5;
        in4 -= in5;

        in2 = in0 + in1;
        in0 -= in1;

        out2[5 - 0] = in2 * wi[11 - 0];
        out2[0 + 0] = in2 * wi[6 + 0];
        out2[0 + 2] = in3 * wi[6 + 2];
        out2[5 - 2] = in3 * wi[11 - 2];

        ts[(12 + 0) * 32] += in0 * wi[0];
        ts[(17 - 0) * 32] += in0 * wi[5 - 0];
        ts[(12 + 2) * 32] += in4 * wi[2];
        ts[(17 - 2) * 32] += in4 * wi[5 - 2];
    }

    in++;

    {
        float in0, in1, in2, in3, in4, in5;
        float * out2 = rawout2;

        out2[12] = out2[13] = out2[14] = out2[15] = out2[16] = out2[17] = 0.0f;

        in5 = in[5 * 3];
        in5 += (in4 = in[4 * 3]);
        in4 += (in3 = in[3 * 3]);
        in3 += (in2 = in[2 * 3]);
        in2 += (in1 = in[1 * 3]);
        in1 += (in0 = in[0 * 3]);

        in5 += in3; in3 += in1;

        in2 *= gCos6_1;
        in3 *= gCos6_1;

        {
            float tmp0, tmp1 = (in0 - in4);

            {
                float tmp2 = (in1 - in5) * gTfCos12[1];

                tmp0 = tmp1 + tmp2;
                tmp1 -= tmp2;
            }
            out2[11 - 1] = tmp0 * wi[11 - 1];
            out2[6 + 1] = tmp0 * wi[6 + 1];
            out2[0 + 1] += tmp1 * wi[1];
            out2[5 - 1] += tmp1 * wi[5 - 1];
        }

        in0 += in4 * gCos6_2;

        in4 = in0 + in2;
        in0 -= in2;

        in1 += in5 * gCos6_2;

        in5 = (in1 + in3) * gTfCos12[0];
        in1 = (in1 - in3) * gTfCos12[2];

        in3 = in4 + in5;
        in4 -= in5;

        in2 = in0 + in1;
        in0 -= in1;

        out2[11 - 0] = in2 * wi[11 - 0];
        out2[6 + 0] = in2 * wi[6 + 0];
        out2[6 + 2] = in3 * wi[6 + 2];
        out2[11 - 2] = in3 * wi[11 - 2];

        out2[0 + 0] += in0 * wi[0];
        out2[5 - 0] += in0 * wi[5 - 0];
        out2[0 + 2] += in4 * wi[2];
        out2[5 - 2] += in4 * wi[5 - 2];
    }
}

FMOD_RESULT CodecMPEG::III_hybrid(float (* fsIn)[18], float (* tsOut)[32], int ch, gr_info_s * gr_info)
{
    float * tspnt = (float *)tsOut;
    float * rawout1, * rawout2;
    int bt;
    int sb = 0;

    {
        int b = mBlc[ch];

        rawout1 = mBlock[b][ch];
        b = -b + 1;
        rawout2 = mBlock[b][ch];
        mBlc[ch] = b;
    }

    if (gr_info->mixed_block_flag)
    {
        sb = 2;
        dct36(fsIn[0], rawout1, rawout2, gWin[0], tspnt);
        dct36(fsIn[1], rawout1 + 18, rawout2 + 18, gWin1[0], tspnt + 1);
        rawout1 += 36;
        rawout2 += 36;
        tspnt += 2;
    }

    bt = gr_info->block_type;
    if (bt == 2)
    {
        for (; sb < (int)gr_info->maxb; sb += 2, tspnt += 2, rawout1 += 36, rawout2 += 36)
        {
            dct12(fsIn[sb], rawout1, rawout2, gWin[2], tspnt);
            dct12(fsIn[sb + 1], rawout1 + 18, rawout2 + 18, gWin1[2], tspnt + 1);
        }
    }
    else
    {
        for (; sb < (int)gr_info->maxb; sb += 2, tspnt += 2, rawout1 += 36, rawout2 += 36)
        {
            dct36(fsIn[sb], rawout1, rawout2, gWin[bt], tspnt);
            dct36(fsIn[sb + 1], rawout1 + 18, rawout2 + 18, gWin1[bt], tspnt + 1);
        }
    }

    for (; sb < 32; sb++, tspnt++)
    {
        int i;

        for (i = 0; i < 18; i++)
        {
            tspnt[i * 32] = *rawout1++;
            *rawout2++ = 0.0f;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT CodecMPEG::decodeLayer3(void * pcm_sample, unsigned int * outlen)
{
    FMOD_RESULT result;
    int gr, ch, ss;
    int scalefacs[2][39];
    III_sideinfo sideinfo;
    int channels = mFrame.stereo;
    int sfreq = mFrame.sampling_frequency;
    int ms_stereo, i_stereo;
    int granules;
    int inc = channels * 32;
    float hybridout[2][18][32];
    float hybridin[2][32][18] = { 0 };

    if (mWaveFormat.format == FMOD_SOUND_FORMAT_PCMFLOAT || mWaveFormat.format == FMOD_SOUND_FORMAT_MPEG)
    {
        inc *= sizeof(float);
    }
    else
    {
        inc *= sizeof(short);
    }

    *outlen = 0;

    memset(&sideinfo, 0, sizeof(III_sideinfo));

    if (mFrame.mode == 1)
    {
        ms_stereo = mFrame.mode_ext & 0x2;
        i_stereo = mFrame.mode_ext & 0x1;
    }
    else
    {
        ms_stereo = i_stereo = 0;
    }

    if (mFrame.lsf)
    {
        granules = 1;
        result = III_get_side_info_2(&sideinfo, channels, ms_stereo, sfreq);
    }
    else
    {
        granules = 2;
        result = III_get_side_info_1(&sideinfo, channels, ms_stereo, sfreq);
    }
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mFrameSizeOld >= 0 || !sideinfo.main_data_begin)
    {
        unsigned char * bsbufold = mBSSpace[mBSNum] + 512;

        mWordPointer -= sideinfo.main_data_begin;
        if (sideinfo.main_data_begin)
        {
            memcpy(mWordPointer, bsbufold + mFrameSizeOld - sideinfo.main_data_begin, sideinfo.main_data_begin);
        }
        mBitIndex = 0;
    }

    for (gr = 0; gr < granules; gr++)
    {
        {
            gr_info_s * gr_info = &(sideinfo.ch[0].gr[gr]);
            int part2bits;

            if (mFrame.lsf)
            {
                III_get_scale_factors_2(scalefacs[0], gr_info, 0, &part2bits);
            }
            else
            {
                III_get_scale_factors_1(scalefacs[0], gr_info, &part2bits);
            }

            result = III_dequantize_sample(hybridin[0], scalefacs[0], gr_info, sfreq, part2bits);
            if (result != FMOD_OK)
            {
                break;
            }
        }

        if (channels == 2)
        {
            gr_info_s * gr_info = &(sideinfo.ch[1].gr[gr]);
            int part2bits;

            if (mFrame.lsf)
            {
                III_get_scale_factors_2(scalefacs[1], gr_info, i_stereo, &part2bits);
            }
            else
            {
                III_get_scale_factors_1(scalefacs[1], gr_info, &part2bits);
            }

            if (ms_stereo)
            {
                III_dequantize_sample_ms(hybridin, scalefacs[1], gr_info, sfreq, part2bits);
            }
            else
            {
                III_dequantize_sample(hybridin[1], scalefacs[1], gr_info, sfreq, part2bits);
            }

            if (i_stereo)
            {
                III_i_stereo(hybridin, scalefacs[1], gr_info, sfreq, ms_stereo, mFrame.lsf);
            }

            if (ms_stereo || i_stereo)
            {
                if (gr_info->maxb > sideinfo.ch[0].gr[gr].maxb)
                {
                    sideinfo.ch[0].gr[gr].maxb = gr_info->maxb;
                }
                else
                {
                    gr_info->maxb = sideinfo.ch[0].gr[gr].maxb;
                }
            }
        }

        for (ch = 0; ch < channels; ch++)
        {
            gr_info_s * gr_info = &(sideinfo.ch[ch].gr[gr]);

            III_antialias(hybridin[ch], gr_info);
            III_hybrid(hybridin[ch], hybridout[ch], ch, gr_info);
        }

        for (ss = 0; ss < 18; ss++)
        {
            synth(pcm_sample, hybridout[0][ss], channels);
            pcm_sample = (char *)pcm_sample + inc;
        }

        *outlen += inc * 18;
    }

    *outlen = granules * inc * 18;

    return FMOD_OK;
}

} // namespace FMOD
