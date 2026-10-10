// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_CODEC_SWVAG_H
#define _FMOD_CODEC_SWVAG_H

namespace FMOD {

struct FMOD_VAG_HDR
{
    unsigned char format[4]; // offset 0x0
    unsigned int ver; // offset 0x4
    unsigned int ssa; // offset 0x8
    unsigned int size; // offset 0xC
    unsigned int fs; // offset 0x10
    unsigned short volL; // offset 0x14
    unsigned short volR; // offset 0x16
    unsigned short pitch; // offset 0x18
    unsigned short ADSR1; // offset 0x1A
    unsigned short ADSR2; // offset 0x1C
    unsigned short reserved; // offset 0x1E
    char name[16]; // offset 0x20
};

} // namespace FMOD

#endif
