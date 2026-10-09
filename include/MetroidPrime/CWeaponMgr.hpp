#ifndef _CWEAPONMGR
#define _CWEAPONMGR

#include "types.h"

#include "MetroidPrime/TGameTypes.hpp"
#include "MetroidPrime/Weapons/WeaponTypes.hpp"

#include "rstl/map.hpp"
#include "rstl/reserved_vector.hpp"

// Minimal view of the per-owner weapon counters (CWeaponMgr.cpp; CStateManager news one, 0x14
// bytes). The whole unit is Echoes' CWeaponMgr.cpp: IncrCount adds the owner
// when it has no counters yet, DecrCount removes it once every count is back to zero.
class CWeaponMgr {
public:
  // Echoes' type. The map's nodes are 0x68 bytes, so the prototype keeps 20 counters per owner
  // (Echoes has 21 weapon types).
  typedef rstl::reserved_vector< int, 20 > Vec;

  CWeaponMgr(); // 0x800B1538

  void Remove(TUniqueId uid);                              // 0x800B12A4
  void IncrCount(TUniqueId uid, EWeaponType type);         // 0x800B121C
  void DecrCount(TUniqueId uid, EWeaponType type);         // 0x800B1158
  int GetNumActive(TUniqueId uid, EWeaponType type) const; // 0x800B1104

  void Add(TUniqueId uid, EWeaponType type); // 0x800B140C
  Vec* GetIndex(TUniqueId uid) const;        // 0x800B0FE4

private:
  rstl::map< TUniqueId, Vec > mWeapons; // Echoes' name
};
CHECK_SIZEOF(CWeaponMgr, 0x14)

#endif // _CWEAPONMGR
