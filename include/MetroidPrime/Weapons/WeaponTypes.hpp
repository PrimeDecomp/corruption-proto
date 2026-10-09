#ifndef _WEAPONTYPES
#define _WEAPONTYPES

#include "types.h"

// Echoes' weapon type enum. Only the type is needed so far; the prototype's values are not
// mapped yet.
enum EWeaponType {
  kWT_None = -1,
  // Guessed name. CStateManager's "Kill Player" and "Kill All AIs" debug options deal their
  // 10000 damage with this type.
  kWT_DebugKill = 13,
};

// Echoes' class: the type in the high halfword, then the charged and comboed flags.
class CWeaponMode {
public:
  explicit CWeaponMode(EWeaponType type = kWT_None, const bool charged = false,
                       const bool comboed = false)
  : mWeaponType(uint(type)), mCharged(charged), mComboed(comboed) {}

private:
  uint mWeaponType : 16;
  uint mCharged : 1;
  uint mComboed : 1;
};

#endif // _WEAPONTYPES
