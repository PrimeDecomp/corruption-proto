// Freeverb denormal guard (public domain, Jezar at Dreampoint). G2MEAB revmodel::processreplace 0x80625D84
// tests the exponent bits (rlwinm 0x7F800000) of each comb/allpass sample and stores 0.0f when they are clear.

#ifndef _DENORMALS_H
#define _DENORMALS_H

#define undenormalise(sample) if (((*(unsigned int *)&sample) & 0x7f800000) == 0) sample = 0.0f

#endif
