#ifndef _CREDUNDANTHINTMANAGER
#define _CREDUNDANTHINTMANAGER

#include "types.h"

#include "rstl/vector.hpp"

class CStateManager;

// Minimal view. The class name comes from CRedundantHintManager.cpp; it takes the place of
// Echoes' CHintOptions in CGameState (at 0xC4) and has its layout: a vector of 0xC-byte hint
// states and the index of the displayed hint (-1 for none). The method names are Echoes'.
class CRedundantHintManager {
public:
  struct SHintState {
    // 0x801A5DA8. Compares the state's time (0x4) against the hint's.
    bool CanContinue() const;

    int x0_;
    float mTime; // Echoes' name
    int x8_;
  };

  // 0x801A5104. Null while hints are disabled in the options or no hint is displayed.
  const SHintState* GetCurrentDisplayedHint() const;
  void DismissDisplayedHint();               // 0x801A4FC4
  void Update(float dt, CStateManager& mgr); // 0x801A53F4
  // 0x801A50E4. The displayed hint's index, or -1 while hints are disabled in the options.
  int GetNextHintIdx();

private:
  rstl::vector< SHintState > mHintStates;
  int mCurrentHint;
};
CHECK_SIZEOF(CRedundantHintManager, 0x14)

#endif // _CREDUNDANTHINTMANAGER
