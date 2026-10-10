// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_SYNCPOINT_H
#define _FMOD_SYNCPOINT_H

#include "fmod_linkedlist.h"

namespace FMOD {

struct SyncPoint : public SortedLinkedListNode
{
    unsigned int mOffset; // offset 0x10
    char mName[256]; // offset 0x14
    int mRiffID; // offset 0x114
};

} // namespace FMOD

#endif
