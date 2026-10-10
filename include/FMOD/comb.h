// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _COMB_H
#define _COMB_H

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

#endif
