// Complete reconstruction of the G2MEAB unit (.text 0x8060B08C..0x8060B560).
// The static initializer constructs the memory pool 0x80754290 and the system list head 0x807542DC and
// registers the head's implicit destructor 0x8060B0D4; that destructor and the SyncPoint destructor
// 0x8060B4A8 are compiler-emitted copies of header code. The configured unit also covers
// Listener::Listener 0x8060B50C, which by alphabetical position is the 4.06 fmod_listener.cpp.

#include "fmod_globals.h"
#include "fmod_listener.h"
#include "fmod_memory.h"
#include "fmod_systemi.h"

namespace FMOD {

static MemPool gSystemPoolMem;
static SystemI gSystemHeadMem;

SystemI * gSystemHead = &gSystemHeadMem;
MemPool * gSystemPool = &gSystemPoolMem;

// The last-* vectors are left unset.
Listener::Listener()
{
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mLastPosition.x = 0.0f;
    mLastPosition.y = 0.0f;
    mLastPosition.z = 0.0f;
    mVelocity.x = 0.0f;
    mVelocity.y = 0.0f;
    mVelocity.z = 0.0f;
    mUp.x = 0.0f;
    mUp.y = 1.0f;
    mUp.z = 0.0f;
    mFront.x = 0.0f;
    mFront.y = 0.0f;
    mFront.z = 1.0f;
    mRight.x = 1.0f;
    mRight.y = 0.0f;
    mRight.z = 0.0f;
}

} // namespace FMOD
