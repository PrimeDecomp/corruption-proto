// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _ALLPASS_H
#define _ALLPASS_H

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

#endif
