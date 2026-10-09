#ifndef _CWEAPONMGR
#define _CWEAPONMGR

#include "types.h"

#include "MetroidPrime/TGameTypes.hpp"
#include "MetroidPrime/Weapons/WeaponTypes.hpp"

#include "rstl/map.hpp"
#include "rstl/reserved_vector.hpp"

// Minimal view of the per-owner weapon counters (CWeaponMgr.cpp; CStateManager news one, 0x14
// bytes). Names and signatures follow Echoes, whose bodies these match: IncrCount adds the owner
// when it has no counters yet, DecrCount removes it once every count is back to zero.
class CWeaponMgr {
public:
  // Echoes' type. The map's nodes are 0x68 bytes, so the prototype keeps 20 counters per owner
  // (Echoes has 21 weapon types).
  typedef rstl::reserved_vector< int, 20 > Vec;

  CWeaponMgr(); // 0x800B1538

  void DecrCount(TUniqueId uid, EWeaponType type); // 0x800B1158
  void IncrCount(TUniqueId uid, EWeaponType type); // 0x800B121C

private:
  rstl::map< TUniqueId, Vec > mWeapons; // Echoes' name
};
CHECK_SIZEOF(CWeaponMgr, 0x14)

#endif // _CWEAPONMGR
