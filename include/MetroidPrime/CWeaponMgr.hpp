#ifndef _CWEAPONMGR
#define _CWEAPONMGR

#include "types.h"

#include "MetroidPrime/TGameTypes.hpp"

// Echoes' weapon type enum (WeaponTypes.hpp there). Only the type is needed so far; the
// prototype's values are not mapped yet.
enum EWeaponType {
  kWT_None = -1,
};

// Minimal view of the per-owner weapon counters (CWeaponMgr.cpp; CStateManager news one, 0x14
// bytes). Names and signatures follow Echoes, whose bodies these match: IncrCount adds the owner
// when it has no counters yet, DecrCount removes it once every count is back to zero.
class CWeaponMgr {
public:
  void DecrCount(TUniqueId uid, EWeaponType type); // 0x800B1158
  void IncrCount(TUniqueId uid, EWeaponType type); // 0x800B121C
};

#endif // _CWEAPONMGR
