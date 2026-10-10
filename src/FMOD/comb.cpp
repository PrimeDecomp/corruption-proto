// G2MEAB prototype translation unit; complete reconstruction (Freeverb comb, public domain, Jezar at Dreampoint).
// .text: 0x8062596C..0x806259D8 (5 native functions; getdamp and getfeedback are dead-stripped).
// Original basename from the 4.06 reference library object (lib/freeverb/comb.cpp).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; G2MEAB accesses confirm the
// 4.06 layout (constructor 0x8062596C clears +0x4/+0x18, setbuffer +0x10/+0x14, setdamp +0x8/+0xC).

#include "comb.h"

comb::comb()
{
    filterstore = 0;
    bufidx = 0;
}

void comb::setbuffer(float * buf, int size)
{
    buffer = buf;
    bufsize = size;
}

void comb::mute()
{
    for (int i = 0; i < bufsize; i++)
    {
        buffer[i] = 0;
    }
}

void comb::setdamp(float val)
{
    damp1 = val;
    damp2 = 1 - val;
}

float comb::getdamp()
{
}

void comb::setfeedback(float val)
{
    feedback = val;
}

float comb::getfeedback()
{
}
