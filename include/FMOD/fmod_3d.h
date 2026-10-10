// 4.06 fmod_3d.h (FMOD_Cart2Angle at fmod_3d.h:58). The static helper is emitted by each unit that uses
// it: G2MEAB fn_805B7998 (fmod_channel_real) and 0x8061A6E0 (fmod_systemi).

#ifndef _FMOD_3D_H
#define _FMOD_3D_H

#include <stdlib.h>

static int FMOD_Cart2Angle(int y, int x)
{
    int coeff_1;
    int coeff_2;
    int abs_y;
    int r;
    int angle;

    if (x == 0 && y == 0)
    {
        return 0;
    }

    x <<= 10;
    y <<= 10;

    coeff_1 = 804;
    coeff_2 = 3 * coeff_1;
    abs_y = (abs)(y) + 1; // native calls abs 0x80642FB4; the parentheses bypass the __abs macro

    if (x >= 0)
    {
        r = (x - abs_y) / ((x + abs_y) >> 10);
        angle = coeff_1 - ((coeff_1 * r) >> 10);
    }
    else
    {
        r = (x + abs_y) / ((abs_y - x) >> 10);
        angle = coeff_2 - ((coeff_1 * r) >> 10);
    }

    if (y < 0)
    {
        angle = -angle;
    }

    angle *= 180;
    angle /= 3216;

    return angle < 0 ? angle + 360 : (angle > 359 ? angle - 360 : angle);
}

#endif
