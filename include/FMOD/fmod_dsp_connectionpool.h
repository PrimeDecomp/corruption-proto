// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_DSP_CONNECTIONPOOL_H
#define _FMOD_DSP_CONNECTIONPOOL_H

#include "fmod.h"
#include "fmod_dsp_connection.h"

namespace FMOD {
    class DSPConnection;
    struct SystemI;
}

namespace FMOD {

class DSPConnectionPool
{
    SystemI * mSystem; // offset 0x0
    DSPConnection * mConnection[32]; // offset 0x4
    DSPConnection * mConnectionMemory[32]; // offset 0x84
    int mNumInputLevels; // offset 0x104
    int mNumOutputLevels; // offset 0x108
    int mNumConnections; // offset 0x10C
    DSPConnection mUsedListHead; // offset 0x110
    DSPConnection mFreeListHead; // offset 0x460
    float * mLevelData[32]; // offset 0x7B0
    float * mLevelDataMemory[32]; // offset 0x830
public:
    FMOD_RESULT init(SystemI * system, int numconnections, int numoutputlevels, int numinputlevels);
    FMOD_RESULT close();
    FMOD_RESULT alloc(DSPConnection * * connection);
    FMOD_RESULT free(DSPConnection * connection);
};

} // namespace FMOD

#endif
