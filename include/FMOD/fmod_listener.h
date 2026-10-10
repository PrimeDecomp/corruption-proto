// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_LISTENER_H
#define _FMOD_LISTENER_H

#include "fmod.h"

namespace FMOD {
    struct Listener;
}

namespace FMOD {

const int LISTENER_MAX = 4;
struct Listener
{
    FMOD_VECTOR mPosition; // offset 0x0
    FMOD_VECTOR mLastPosition; // offset 0xC
    FMOD_VECTOR mVelocity; // offset 0x18
    FMOD_VECTOR mLastVelocity; // offset 0x24
    FMOD_VECTOR mUp; // offset 0x30
    FMOD_VECTOR mLastUp; // offset 0x3C
    FMOD_VECTOR mFront; // offset 0x48
    FMOD_VECTOR mLastFront; // offset 0x54
    FMOD_VECTOR mRight; // offset 0x60
    bool mMoved; // offset 0x6C
    bool mRotated; // offset 0x6D
    Listener();
};

} // namespace FMOD

#endif
