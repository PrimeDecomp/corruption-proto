// The inline sqrtf of the MSL that G2MEAB FMOD was compiled against (pre-2.4.7 form): it returns NaN
// for negative and NaN input and classifies with FP_NAN == 1. Expanded natively in
// ChannelI::calcVolumeAndPitchFor3D 0x805BD588 and ChannelRealManual3D::set2DFreqVolumePanFor3D
// 0x805B7AB4. The project <math.h> carries the later form without those checks.

#ifndef _FMOD_MSL_MATH_H
#define _FMOD_MSL_MATH_H

#include <math.h>

static inline int MSL_fpclassifyf(float x)
{
    switch ((*(int *)&x) & 0x7F800000)
    {
        case 0x7F800000:
        {
            if ((*(int *)&x) & 0x007FFFFF)
            {
                return 1;
            }
            else
            {
                return 2;
            }
            break;
        }
        case 0:
        {
            if ((*(int *)&x) & 0x007FFFFF)
            {
                return 5;
            }
            else
            {
                return 3;
            }
            break;
        }
    }

    return 4;
}

static inline float MSL_sqrtf(float x)
{
    static const double _half = .5;
    static const double _three = 3.0;

    if (x > 0.0f)
    {
        double guess = __frsqrte((double)x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        guess = _half * guess * (_three - guess * guess * x);
        return (float)(x * guess);
    }
    else if (x < 0.0)
    {
        return NAN;
    }
    else if (MSL_fpclassifyf(x) == 1)
    {
        return NAN;
    }

    return x;
}

#endif
