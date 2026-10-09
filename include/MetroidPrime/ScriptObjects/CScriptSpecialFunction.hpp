#ifndef _CSCRIPTSPECIALFUNCTION
#define _CSCRIPTSPECIALFUNCTION

#include "types.h"

#include "MetroidPrime/CActor.hpp"
#include "MetroidPrime/Player/CPlayerState.hpp"

// Minimal declaration (CScriptSpecialFunction.cpp). CStateManager::DisplayAlertAboutOutOfAmmo
// casts every object with type id 0x4E (TypesMatch.cpp, 0x801D72CC) and, for function 0x33 at
// 0x170, calls 0x80110DB4 in this class's file, which checks the same function again. The cast's
// class is inferred from that and from the ids following the class names alphabetically.
class CScriptSpecialFunction : public CActor {
public:
  // Echoes' name for the enum.
  enum ESpecialFunction {
    kSF_ItemDepletion = 0x33, // Echoes' guessed name; Echoes uses the same value
  };

  ESpecialFunction GetFunction() const { return mFunction; } // Echoes' name

  // Guessed name, as in Echoes. Unlike Echoes there is no player index. When the depleted item is
  // the one at 0x25C and its amount is 0, the object sends its script messages.
  void OnItemDepleted(CStateManager& mgr, CPlayerState::EItemType item);

private:
  uchar xF8_[0x170 - 0xF8];
  ESpecialFunction mFunction; // Echoes' name
};

#endif // _CSCRIPTSPECIALFUNCTION
