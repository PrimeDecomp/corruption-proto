// G2MEAB prototype translation unit; complete reconstruction (Freeverb revmodel, public domain, Jezar at Dreampoint).
// .text: 0x806259D8..0x806262F4 (16 native functions; processmix is dead-stripped).
// Original basename from the 4.06 reference library object (lib/freeverb/revmodel.cpp).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; the G2MEAB constructor
// (0x806259D8) confirms the 4.06 layout: comb arrays at 0x2C/0x10C, allpass arrays at 0x1EC/0x22C, buffers from 0x26C.

#include "revmodel.h"

revmodel::revmodel()
{
    // Tie the components to their buffers
    combL[0].setbuffer(bufcombL1, combtuningL1);
    combR[0].setbuffer(bufcombR1, combtuningR1);
    combL[1].setbuffer(bufcombL2, combtuningL2);
    combR[1].setbuffer(bufcombR2, combtuningR2);
    combL[2].setbuffer(bufcombL3, combtuningL3);
    combR[2].setbuffer(bufcombR3, combtuningR3);
    combL[3].setbuffer(bufcombL4, combtuningL4);
    combR[3].setbuffer(bufcombR4, combtuningR4);
    combL[4].setbuffer(bufcombL5, combtuningL5);
    combR[4].setbuffer(bufcombR5, combtuningR5);
    combL[5].setbuffer(bufcombL6, combtuningL6);
    combR[5].setbuffer(bufcombR6, combtuningR6);
    combL[6].setbuffer(bufcombL7, combtuningL7);
    combR[6].setbuffer(bufcombR7, combtuningR7);
    combL[7].setbuffer(bufcombL8, combtuningL8);
    combR[7].setbuffer(bufcombR8, combtuningR8);
    allpassL[0].setbuffer(bufallpassL1, allpasstuningL1);
    allpassR[0].setbuffer(bufallpassR1, allpasstuningR1);
    allpassL[1].setbuffer(bufallpassL2, allpasstuningL2);
    allpassR[1].setbuffer(bufallpassR2, allpasstuningR2);
    allpassL[2].setbuffer(bufallpassL3, allpasstuningL3);
    allpassR[2].setbuffer(bufallpassR3, allpasstuningR3);
    allpassL[3].setbuffer(bufallpassL4, allpasstuningL4);
    allpassR[3].setbuffer(bufallpassR4, allpasstuningR4);

    // Set default values
    allpassL[0].setfeedback(0.5f);
    allpassR[0].setfeedback(0.5f);
    allpassL[1].setfeedback(0.5f);
    allpassR[1].setfeedback(0.5f);
    allpassL[2].setfeedback(0.5f);
    allpassR[2].setfeedback(0.5f);
    allpassL[3].setfeedback(0.5f);
    allpassR[3].setfeedback(0.5f);
    setwet(initialwet);
    setroomsize(initialroom);
    setdry(initialdry);
    setdamp(initialdamp);
    setwidth(initialwidth);
    setmode(initialmode);

    // Buffer will be full of rubbish - so we MUST mute them
    mute();
}

void revmodel::mute()
{
    if (getmode() >= freezemode)
    {
        return;
    }

    for (int i = 0; i < numcombs; i++)
    {
        combL[i].mute();
        combR[i].mute();
    }
    for (int i = 0; i < numallpasses; i++)
    {
        allpassL[i].mute();
        allpassR[i].mute();
    }
}

void revmodel::processreplace(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip)
{
    float outL, outR, input;

    while (numsamples-- > 0)
    {
        outL = outR = 0;
        input = (*inputL + *inputR) * gain;

        // Accumulate comb filters in parallel
        for (int i = 0; i < numcombs; i++)
        {
            outL += combL[i].process(input);
            outR += combR[i].process(input);
        }

        // Feed through allpasses in series
        for (int i = 0; i < numallpasses; i++)
        {
            outL = allpassL[i].process(outL);
            outR = allpassR[i].process(outR);
        }

        // Calculate output REPLACING anything already there
        *outputL = outL * wet1 + outR * wet2 + *inputL * dry;
        *outputR = outR * wet1 + outL * wet2 + *inputR * dry;

        // Increment sample pointers, allowing for interleave (if any)
        inputL += skip;
        inputR += skip;
        outputL += skip;
        outputR += skip;
    }
}

void revmodel::processmix(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip)
{
}

void revmodel::update()
{
    // Recalculate internal values after parameter change

    int i;

    wet1 = wet * (width / 2 + 0.5f);
    wet2 = wet * ((1 - width) / 2);

    if (mode >= freezemode)
    {
        roomsize1 = 1;
        damp1 = 0;
        gain = muted;
    }
    else
    {
        roomsize1 = roomsize;
        damp1 = damp;
        gain = fixedgain;
    }

    for (i = 0; i < numcombs; i++)
    {
        combL[i].setfeedback(roomsize1);
        combR[i].setfeedback(roomsize1);
    }

    for (i = 0; i < numcombs; i++)
    {
        combL[i].setdamp(damp1);
        combR[i].setdamp(damp1);
    }
}

// The following get/set functions are not inlined, because
// speed is never an issue when calling them, and also
// because as you develop the reverb model, you may
// wish to take dynamic action when they are called.

void revmodel::setroomsize(float value)
{
    roomsize = (value * scaleroom) + offsetroom;
    update();
}

float revmodel::getroomsize()
{
    return (roomsize - offsetroom) / scaleroom;
}

void revmodel::setdamp(float value)
{
    damp = value * scaledamp;
    update();
}

float revmodel::getdamp()
{
    return damp / scaledamp;
}

void revmodel::setwet(float value)
{
    wet = value * scalewet;
    update();
}

float revmodel::getwet()
{
    return wet / scalewet;
}

void revmodel::setdry(float value)
{
    dry = value * scaledry;
}

float revmodel::getdry()
{
    return dry / scaledry;
}

void revmodel::setwidth(float value)
{
    width = value;
    update();
}

float revmodel::getwidth()
{
    return width;
}

void revmodel::setmode(float value)
{
    mode = value;
    update();
}

float revmodel::getmode()
{
    if (mode >= freezemode)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
