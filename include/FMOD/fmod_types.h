// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_TYPES_H
#define _FMOD_TYPES_H

typedef long long FMOD_SINT64;
typedef unsigned long long FMOD_UINT64;
typedef unsigned int FMOD_UINT_NATIVE;
typedef float FMOD_UFLOAT;
struct FMOD_GUID
{
    unsigned int Data1; // offset 0x0
    unsigned short Data2; // offset 0x4
    unsigned short Data3; // offset 0x6
    unsigned char Data4[8]; // offset 0x8
};

union FMOD_UINT64P {
    FMOD_UINT64 mValue; // offset 0x0
    struct {
        unsigned int mHi; // offset 0x0
        unsigned int mLo; // offset 0x4
    }; // offset 0x0
};

union FMOD_SINT64P {
    FMOD_SINT64 mValue; // offset 0x0
    struct {
        int mHi; // offset 0x0
        unsigned int mLo; // offset 0x4
    }; // offset 0x0
};

struct FMOD_INT24
{
    unsigned char val[3]; // offset 0x0
};

#endif
