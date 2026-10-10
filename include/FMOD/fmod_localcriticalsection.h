// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_LOCALCRITICALSECTION_H
#define _FMOD_LOCALCRITICALSECTION_H

#include "fmod_os_misc.h"

namespace FMOD {
    class LocalCriticalSection;
}

namespace FMOD {

class LocalCriticalSection
{
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x0
    bool mEntered; // offset 0x4
public:
    LocalCriticalSection();
    LocalCriticalSection(FMOD_OS_CRITICALSECTION * crit, bool enternow) {}
    ~LocalCriticalSection() {}
    void enter();
    void leave();
};

} // namespace FMOD

#endif
