// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information; G2MEAB layout (group D).

#ifndef _FMOD_DSP_CONNECTIONPOOL_H
#define _FMOD_DSP_CONNECTIONPOOL_H

#include "fmod.h"
#include "fmod_dsp_connection.h"

namespace FMOD {
    class DSPConnection;
    struct SystemI;
}

namespace FMOD {

// G2MEAB layout, sizeof 0x100 (embedded in SystemI at 0x834, next member at 0x934).
// Evidence: init 0x805F3F7C, close 0x805F413C, alloc 0x805F41B8, free 0x805F43E0.
class DSPConnectionPool
{
public:
    DSPConnection * mConnection; // offset 0x0
    int mNumInputLevels; // offset 0x4
    int mNumConnections; // offset 0x8
    DSPConnection mUsedListHead; // offset 0xC
    DSPConnection mFreeListHead; // offset 0x84
    float * mLevelDataMemory; // offset 0xFC

    FMOD_RESULT init(int numconnections, int numinputlevels);
    FMOD_RESULT close();
    FMOD_RESULT alloc(DSPConnection * * connection);
    FMOD_RESULT free(DSPConnection * connection);
};

} // namespace FMOD

#endif
