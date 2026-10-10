// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. G2MEAB layout (crit 0x0, entered 0x4) from the
// inlined stack objects in DSPI::addInput 0x806073E0 (sp+0xC crit, sp+0x10 flag).

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
    // G2MEAB: everything is inlined (DSPI 0x806073E0/0x806076D0: ctor stores mEntered=false then mCrit,
    // enter/leave call FMOD_OS_CriticalSection_Enter 0x80623564 / _Leave 0x80623598).
    LocalCriticalSection(FMOD_OS_CRITICALSECTION * crit, bool enternow = false)
    {
        mEntered = false;
        mCrit = crit;
        if (enternow)
        {
            enter();
        }
    }
    ~LocalCriticalSection()
    {
        if (mEntered)
        {
            leave();
        }
    }
    void enter()
    {
        FMOD_OS_CriticalSection_Enter(mCrit);
        mEntered = true;
    }
    void leave()
    {
        FMOD_OS_CriticalSection_Leave(mCrit);
        mEntered = false;
    }
};

} // namespace FMOD

#endif
