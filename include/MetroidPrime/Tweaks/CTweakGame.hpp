#ifndef _CTWEAKGAME
#define _CTWEAKGAME

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakGame;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakGame {
public:
  explicit CTweakGame(const SLdrTweakGame& data) : mData(&data) {}

  // Named after the SLdrTweakGame fields they return. Defined in TweaksAccessors.cpp.
  bool GetMusicOnByDefault() const;
  float GetHardModeDamageMultiplier() const;
  float GetHardModeWeaponMultiplier() const;

private:
  const SLdrTweakGame* mData;
};
CHECK_SIZEOF(CTweakGame, 0x4)

extern rstl::single_ptr< CTweakGame > gpTweakGame;

#endif // _CTWEAKGAME
