#ifndef _CSTATEMANAGER
#define _CSTATEMANAGER

#include "types.h"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/CRandom16.hpp"

class CStateManagerCallbackLists;
class CStateManagerCollision;
class CStateManagerObject;

// Only the fields with evidence are laid out. The size comes from CMFGameLoader, which allocates
// the manager through TOneStatic<CStateManager>'s operator new (0x8021A380) and asserts a
// 0x220-byte limit there.
class CStateManager {
public:
  CStateManagerObject& ObjectManager() { return *mObjectManager; } // Guessed name
  // CScriptLUA's RandomRange (0x802B5C24) inlines this warning before using the generator.
  CRandom16* Random() {
    if (!mRandomAvailable) {
      gpfnWarningPrintf("BUG THIS! Random() called when not deterministic!\n");
      rs_debugger_printf("BUG THIS! Random() called when not deterministic!\n");
    }
    return &mRandom;
  }
  bool IsRandomAvailable() const { return mRandomAvailable; }

private:
  // The constructor news each of these (0x5E8, 0x1138 and 0x1C038 bytes); their constructors live
  // in CStateManagerCallbackLists.cpp, CStateManagerObject.cpp and CStateManagerCollision.cpp, and
  // the class names are guessed from those files.
  CStateManagerCallbackLists* mCallbackLists; // Guessed name
  CStateManagerObject* mObjectManager;        // Guessed name
  CStateManagerCollision* mCollision;         // Guessed name
  uchar xc_[0x16C - 0xC];
  // Echoes' names. As in Echoes, the constructor builds the "DefaultShadow" token at 0x160, then
  // seeds this with 0 and clears the flag.
  CRandom16 mRandom;
  bool mRandomAvailable : 1;
  uchar x171_[0x220 - 0x171];
};
CHECK_SIZEOF(CStateManager, 0x220)

#endif // _CSTATEMANAGER
