#ifndef _CTWEAKPLAYERGUN
#define _CTWEAKPLAYERGUN

#include "types.h"

#include "rstl/reserved_vector.hpp"
#include "rstl/single_ptr.hpp"

struct SLdrTweakPlayerGun;

// Cached normal and charged damage per beam (Echoes CTweakPlayerGun.hpp).
struct SWeaponInfo {
  float mCoolDown;
  uchar mDamage[0x38];
};
CHECK_SIZEOF(SWeaponInfo, 0x3c)

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakPlayerGun {
public:
  explicit CTweakPlayerGun(const SLdrTweakPlayerGun& data) : mData(&data) { BuildCache(); }

private:
  void BuildCache(); // TweaksAccessors.cpp 0x80247CB8

  const SLdrTweakPlayerGun* mData;
  rstl::reserved_vector< SWeaponInfo, 8 > mBeamInfo;
};
CHECK_SIZEOF(CTweakPlayerGun, 0x1e8)

extern rstl::single_ptr< CTweakPlayerGun > gpTweakPlayerGun;

#endif // _CTWEAKPLAYERGUN
