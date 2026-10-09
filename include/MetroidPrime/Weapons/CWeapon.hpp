#ifndef _CWEAPON
#define _CWEAPON

#include "types.h"

#include "MetroidPrime/CRenderActor.hpp"

// Minimal declaration. Unlike Echoes' CWeapon (a CActor), the prototype's derives from
// CRenderActor: its constructor (0x800E0CF0) builds a CRenderActor, installs the vtable at
// 0x806B3B60 and stores the projectile attributes at 0x170, right after the base. TypesMatch id 9.
class CWeapon : public CRenderActor {
public:
  // Echoes' names; only the two bomb bits are checked against the prototype
  // (CStateManager::TestBombHittingWater).
  enum EProjectileAttrib {
    kPA_TriggerBomb = 1 << 8,
    kPA_PowerBombs = 1 << 9,
  };

  int GetAttribField() const { return mAttribField; } // Echoes' name

private:
  int mAttribField; // Echoes' name
};

#endif // _CWEAPON
