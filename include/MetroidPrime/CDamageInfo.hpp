#ifndef _CDAMAGEINFO
#define _CDAMAGEINFO

#include "types.h"

// Minimal declaration: only the weapon type that CActor::SendWeaponDamageState switches on.
// It is read as the first halfword, like the 16-bit type at the start of Echoes' CWeaponMode,
// which Echoes' CDamageInfo stores first. The rest of the layout is not modelled yet.
class CDamageInfo {
public:
  int GetWeaponType() const { return mWeaponType; } // Guessed name

private:
  ushort mWeaponType;
};

#endif // _CDAMAGEINFO
