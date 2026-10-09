#ifndef _CTWEAKPLAYER
#define _CTWEAKPLAYER

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakPlayer;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakPlayer {
public:
  explicit CTweakPlayer(const SLdrTweakPlayer& data) : mData(&data) {}

  // Echoes names; out of line in TweaksAccessors.cpp (0x8024AE58 and 0x8024AE4C).
  float GetLeftAnalogMax() const;
  float GetRightAnalogMax() const;

private:
  const SLdrTweakPlayer* mData;
};
CHECK_SIZEOF(CTweakPlayer, 0x4)

extern rstl::single_ptr< CTweakPlayer > gpTweakPlayer;

#endif // _CTWEAKPLAYER
