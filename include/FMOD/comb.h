// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information (Freeverb). The 4.06 layout is confirmed by the G2MEAB accesses (see the .cpp).

#ifndef _COMB_H
#define _COMB_H

#include "denormals.h"

class comb;

class comb
{
public:
    comb();
    void setbuffer(float * buf, int size);
    float process(float input);
    void mute();
    void setdamp(float val);
    float getdamp();
    void setfeedback(float val);
    float getfeedback();
private:
    float feedback; // offset 0x0
    float filterstore; // offset 0x4
    float damp1; // offset 0x8
    float damp2; // offset 0xC
    float * buffer; // offset 0x10
    int bufsize; // offset 0x14
    int bufidx; // offset 0x18
};

// Big to inline - but crucial for speed (Freeverb); inlined into revmodel::processreplace 0x80625D84.
inline float comb::process(float input)
{
    float output;

    output = buffer[bufidx];
    undenormalise(output);

    filterstore = (output * damp2) + (filterstore * damp1);
    undenormalise(filterstore);

    buffer[bufidx] = input + (filterstore * feedback);

    if (++bufidx >= bufsize)
    {
        bufidx = 0;
    }

    return output;
}

#endif
