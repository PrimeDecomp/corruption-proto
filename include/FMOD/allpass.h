// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information (Freeverb). The 4.06 layout is confirmed by the G2MEAB accesses (see the .cpp).

#ifndef _ALLPASS_H
#define _ALLPASS_H

#include "denormals.h"

struct allpass;

struct allpass
{
    allpass();
    void setbuffer(float * buf, int size);
    float process(float input);
    void mute();
    void setfeedback(float val);
    float getfeedback();
    float feedback; // offset 0x0
    float * buffer; // offset 0x4
    int bufsize; // offset 0x8
    int bufidx; // offset 0xC
};

// Big to inline - but crucial for speed (Freeverb); inlined into revmodel::processreplace 0x80625D84.
inline float allpass::process(float input)
{
    float output;
    float bufout;

    bufout = buffer[bufidx];
    undenormalise(bufout);

    output = -input + bufout;
    buffer[bufidx] = input + (bufout * feedback);

    if (++bufidx >= bufsize)
    {
        bufidx = 0;
    }

    return output;
}

#endif
