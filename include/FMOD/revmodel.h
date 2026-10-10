// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _REVMODEL_H
#define _REVMODEL_H

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
    comb combL[8]; // offset 0x2C
    comb combR[8]; // offset 0x10C
    allpass allpassL[4]; // offset 0x1EC
    allpass allpassR[4]; // offset 0x22C
    float bufcombL1[1116]; // offset 0x26C
    float bufcombR1[1139]; // offset 0x13DC
    float bufcombL2[1188]; // offset 0x25A8
    float bufcombR2[1211]; // offset 0x3838
    float bufcombL3[1277]; // offset 0x4B24
    float bufcombR3[1300]; // offset 0x5F18
    float bufcombL4[1356]; // offset 0x7368
    float bufcombR4[1379]; // offset 0x8898
    float bufcombL5[1422]; // offset 0x9E24
    float bufcombR5[1445]; // offset 0xB45C
    float bufcombL6[1491]; // offset 0xCAF0
    float bufcombR6[1514]; // offset 0xE23C
    float bufcombL7[1557]; // offset 0xF9E4
    float bufcombR7[1580]; // offset 0x11238
    float bufcombL8[1617]; // offset 0x12AE8
    float bufcombR8[1640]; // offset 0x1442C
    float bufallpassL1[556]; // offset 0x15DCC
    float bufallpassR1[579]; // offset 0x1667C
    float bufallpassL2[441]; // offset 0x16F88
    float bufallpassR2[464]; // offset 0x1766C
    float bufallpassL3[341]; // offset 0x17DAC
    float bufallpassR3[364]; // offset 0x18300
    float bufallpassL4[225]; // offset 0x188B0
    float bufallpassR4[248]; // offset 0x18C34
};

#endif
