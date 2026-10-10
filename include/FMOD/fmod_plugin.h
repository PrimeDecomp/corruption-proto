// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_PLUGIN_H
#define _FMOD_PLUGIN_H

#include "fmod.h"
#include "fmod_linkedlist.h"

namespace FMOD {
    struct Global;
    struct Plugin;
    struct SystemI;
}

namespace FMOD {

struct Plugin : public LinkedListNode
{
    SystemI * mSystem; // offset 0x10
    Global * mGlobal; // offset 0x14
    Plugin();
    FMOD_RESULT init();
    virtual FMOD_RESULT release();
};

} // namespace FMOD

#endif
