// G2MEAB Plugin base (layout below; vtable 0x806EE990).

#ifndef _FMOD_PLUGIN_H
#define _FMOD_PLUGIN_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_globals.h"

namespace FMOD {
    class MemPool;
    struct Plugin;
    struct SystemI;
}

namespace FMOD {

// G2MEAB: 0x20 bytes. The inlined constructor (PluginFactory::createCodec 0x806115A0) copies gSystemPool
// to +0x1C and gSystemHead to +0x18; the inlined init (CodecRaw::openInternal 0x805E3124) copies them
// back; createSoundInternal 0x8061C1C0 stores the SystemI at +0x14. Proposed by group F (owner A).
struct Plugin : public LinkedListNode
{
    SystemI * mSystem; // offset 0x14
    SystemI * mSystemHead; // offset 0x18, Guessed name (4.06 mGlobal)
    MemPool * mSystemPool; // offset 0x1C, Guessed name (4.06 mGlobal)
    Plugin()
    {
        mSystemPool = gSystemPool;
        mSystemHead = gSystemHead;
    }
    FMOD_RESULT init()
    {
        gSystemPool = mSystemPool;
        gSystemHead = mSystemHead;
        return FMOD_OK;
    }
    virtual FMOD_RESULT release();
};

} // namespace FMOD

#endif
