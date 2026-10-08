#ifndef _CTWEAKPLAYER
#define _CTWEAKPLAYER

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakPlayer;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakPlayer {
public:
  explicit CTweakPlayer(const SLdrTweakPlayer& data) : mData(&data) {}

private:
  const SLdrTweakPlayer* mData;
};
CHECK_SIZEOF(CTweakPlayer, 0x4)

extern rstl::single_ptr< CTweakPlayer > gpTweakPlayer;

#endif // _CTWEAKPLAYER
