#ifndef _CDAMAGEINFO
#define _CDAMAGEINFO

#include "types.h"

#include "Kyoto/CAssetId.hpp"

class CWeaponMode;

// Minimal declaration: only the weapon type that CActor::SendWeaponDamageState switches on.
// It is read as the first halfword, like the 16-bit type at the start of Echoes' CWeaponMode,
// which Echoes' CDamageInfo stores first. The rest of the layout is not modelled yet.
class CDamageInfo {
public:
  // 0x80014360 (emitted at the end of CPlayer.cpp's range). Stores the mode, the damage twice
  // (Echoes' damage and radius damage), two more floats, three asset ids at 0x18..0x28 and two
  // flags at 0x30. Which float and which flag is which is not known; the argument order of the
  // floats relative to the others is inferred from Echoes' constructors.
  CDamageInfo(const CWeaponMode& mode, float damage, bool, bool, CAssetId, CAssetId, CAssetId,
              float, float);

  int GetWeaponType() const { return mWeaponType; } // Guessed name

private:
  ushort mWeaponType;
  uchar x2_[0x16];
  CAssetId x18_;
  CAssetId x20_;
  CAssetId x28_;
  uchar x30_[0x8];
};
CHECK_SIZEOF(CDamageInfo, 0x38)

#endif // _CDAMAGEINFO
