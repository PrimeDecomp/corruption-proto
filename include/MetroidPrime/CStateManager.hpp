#ifndef _CSTATEMANAGER
#define _CSTATEMANAGER

#include "types.h"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/CRandom16.hpp"

class CRenderManager;
class CStateManagerCallbackLists;
class CStateManagerCollision;
class CStateManagerObject;

// Only the fields with evidence are laid out. The size comes from CMFGameLoader, which allocates
// the manager through TOneStatic<CStateManager>'s operator new (0x8021A380) and asserts a
// 0x220-byte limit there.
//
// Unlike Echoes' monolithic class (0x1C00+ bytes), Corruption's manager is a 0x220-byte hub
// whose state lives in separately allocated objects:
// - CStateManagerObject (0x4): the entity database. Object lists, unique ids, the script message
//   queue and its delivery, the world, the graveyard, the editor-id map, the mailbox, the
//   map-world info and the player. Callers across the DOL reach it through the pointer and its
//   out-of-line getters instead of inline CStateManager accessors.
// - CStateManagerCallbackLists (0x0): 63 signals. Instead of calling each subsystem in turn, the
//   update emits 46 per-phase signals with the frame time, CRenderManager emits two groups of
//   render signals, and CStateManagerObject fires the entity added/removed/active signals. The
//   destructor emits the 0x5B8 signal first.
// - CStateManagerCollision (0x8): the collision and near-list work.
// - CRenderManager (0x18): the drawing (Echoes' DrawWorld family).
// What stays here is the frame driver (FrameBegin, the update and its profiling), the damage and
// knock-back rules, area changes for actors, world setup and the memory callbacks.
class CStateManager {
public:
  CStateManagerObject& ObjectManager() { return *mObjectManager; } // Guessed name
  const CStateManagerObject& ObjectManager() const { return *mObjectManager; }
  CStateManagerCallbackLists& CallbackLists() { return *mCallbackLists; } // Guessed name
  CRenderManager* RenderManager() { return mRenderManager; }              // Guessed name
  // CScriptLUA's RandomRange (0x802B5C24) inlines this warning before using the generator.
  CRandom16* Random() {
    if (!mRandomAvailable) {
      gpfnWarningPrintf("BUG THIS! Random() called when not deterministic!\n");
      rs_debugger_printf("BUG THIS! Random() called when not deterministic!\n");
    }
    return &mRandom;
  }
  bool IsRandomAvailable() const { return mRandomAvailable; }
  // Prime's name. The update (0x80292E9C) seeds CDecal and CProjectileWeapon with it and bumps
  // it at the end; the script message logs print it.
  uint GetUpdateFrameIndex() const { return mUpdateFrameIdx; }

  // Echoes' names, in Echoes' order. The constructor installs the callback with CMemory; unlike
  // Echoes, it first reports the last and current areas.
  void SwapOutTexturesToARAM(int, uint);
  static const bool MemoryAllocatorAllocationFailedCallback(const void* context, uint size);
  bool SwapOutAllPossibleMemory();

private:
  // The constructor news each of these (0x5E8, 0x1138 and 0x1C038 bytes); their constructors live
  // in CStateManagerCallbackLists.cpp, CStateManagerObject.cpp and CStateManagerCollision.cpp, and
  // the class names are guessed from those files.
  CStateManagerCallbackLists* mCallbackLists; // Guessed name
  CStateManagerObject* mObjectManager;        // Guessed name
  CStateManagerCollision* mCollision;         // Guessed name
  uchar xc_[0x18 - 0xC];
  // Guessed name. CStateManager.cpp news this (0x780 bytes; the constructor 0x802AA5F4 is in
  // CRenderManager.cpp and keeps the state manager at +4).
  CRenderManager* mRenderManager;
  uchar x1c_[0x15C - 0x1C];
  uint mUpdateFrameIdx; // Prime's name
  uchar x160_[0x16C - 0x160];
  // Echoes' names. As in Echoes, the constructor builds the "DefaultShadow" token at 0x160, then
  // seeds this with 0 and clears the flag.
  CRandom16 mRandom;
  bool mRandomAvailable : 1;
  uchar x171_[0x220 - 0x171];
};
CHECK_SIZEOF(CStateManager, 0x220)

#endif // _CSTATEMANAGER
