// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information (Freeverb). The 4.06 layout is confirmed by the G2MEAB accesses (see the .cpp).

#ifndef _REVMODEL_H
#define _REVMODEL_H

#include "tuning.h"
#include "allpass.h"
#include "comb.h"

class revmodel;

class revmodel
{
public:
    revmodel();
    void mute();
    void processmix(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip);
    void processreplace(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip);
    void setroomsize(float value);
    float getroomsize();
    void setdamp(float value);
    float getdamp();
    void setwet(float value);
    float getwet();
    void setdry(float value);
    float getdry();
    void setwidth(float value);
    float getwidth();
    void setmode(float value);
    float getmode();
    void update();
private:
    float gain; // offset 0x0
    float roomsize; // offset 0x4
    float roomsize1; // offset 0x8
    float damp; // offset 0xC
    float damp1; // offset 0x10
    float wet; // offset 0x14
    float wet1; // offset 0x18
    float wet2; // offset 0x1C
    float dry; // offset 0x20
    float width; // offset 0x24
    float mode; // offset 0x28
    comb combL[numcombs]; // offset 0x2C
    comb combR[numcombs]; // offset 0x10C
    allpass allpassL[numallpasses]; // offset 0x1EC
    allpass allpassR[numallpasses]; // offset 0x22C
    float bufcombL1[combtuningL1]; // offset 0x26C
    float bufcombR1[combtuningR1]; // offset 0x13DC
    float bufcombL2[combtuningL2]; // offset 0x25A8
    float bufcombR2[combtuningR2]; // offset 0x3838
    float bufcombL3[combtuningL3]; // offset 0x4B24
    float bufcombR3[combtuningR3]; // offset 0x5F18
    float bufcombL4[combtuningL4]; // offset 0x7368
    float bufcombR4[combtuningR4]; // offset 0x8898
    float bufcombL5[combtuningL5]; // offset 0x9E24
    float bufcombR5[combtuningR5]; // offset 0xB45C
    float bufcombL6[combtuningL6]; // offset 0xCAF0
    float bufcombR6[combtuningR6]; // offset 0xE23C
    float bufcombL7[combtuningL7]; // offset 0xF9E4
    float bufcombR7[combtuningR7]; // offset 0x11238
    float bufcombL8[combtuningL8]; // offset 0x12AE8
    float bufcombR8[combtuningR8]; // offset 0x1442C
    float bufallpassL1[allpasstuningL1]; // offset 0x15DCC
    float bufallpassR1[allpasstuningR1]; // offset 0x1667C
    float bufallpassL2[allpasstuningL2]; // offset 0x16F88
    float bufallpassR2[allpasstuningR2]; // offset 0x1766C
    float bufallpassL3[allpasstuningL3]; // offset 0x17DAC
    float bufallpassR3[allpasstuningR3]; // offset 0x18300
    float bufallpassL4[allpasstuningL4]; // offset 0x188B0
    float bufallpassR4[allpasstuningR4]; // offset 0x18C34
};

#endif
