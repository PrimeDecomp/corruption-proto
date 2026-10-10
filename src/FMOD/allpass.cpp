// G2MEAB prototype translation unit; complete reconstruction (Freeverb allpass, public domain, Jezar at Dreampoint).
// .text: 0x8062591C..0x8062596C (4 native functions; getfeedback is dead-stripped).
// Original basename from the 4.06 reference library object (lib/freeverb/allpass.cpp).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; G2MEAB accesses confirm the
// 4.06 layout (setbuffer 0x80625928 stores +0x4/+0x8, constructor clears +0xC, setfeedback stores +0x0).

#include "allpass.h"

allpass::allpass()
{
    bufidx = 0;
}

void allpass::setbuffer(float * buf, int size)
{
    buffer = buf;
    bufsize = size;
}

void allpass::mute()
{
    for (int i = 0; i < bufsize; i++)
    {
        buffer[i] = 0;
    }
}

void allpass::setfeedback(float val)
{
    feedback = val;
}

float allpass::getfeedback()
{
}
