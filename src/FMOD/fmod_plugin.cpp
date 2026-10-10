// Complete reconstruction of the G2MEAB unit (.text 0x80610930..0x80610968). The constructor and init
// are inline in fmod_plugin.h; release is the only retained body (vtable 0x806EE990 slot after the
// shared destructor 0x805C706C).

#include "fmod_plugin.h"
#include "fmod_memory.h"

namespace FMOD {

FMOD_RESULT Plugin::release()
{
    FMOD_Memory_Free(this);
    return FMOD_OK;
}

} // namespace FMOD
