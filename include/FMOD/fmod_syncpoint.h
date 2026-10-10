// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. SyncPoint is the G2MEAB layout.

#ifndef _FMOD_SYNCPOINT_H
#define _FMOD_SYNCPOINT_H

#include "fmod_linkedlist.h"

namespace FMOD {

// G2MEAB: 0x11C bytes, vtable 0x806EE000 (destructor 0x8060B4A8). SoundI embeds one as its list head
// (+0x1A8); SoundI::getSyncPointInfo reads mOffset at +0x14 and copies mName from +0x18.
struct SyncPoint : public LinkedListNode
{
    unsigned int mOffset; // offset 0x14
    char mName[256]; // offset 0x18
    int mRiffID; // offset 0x118
};

} // namespace FMOD

#endif
