#ifndef _CTWEAKPLAYERCONTROLS
#define _CTWEAKPLAYERCONTROLS

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrPlayerControls;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakPlayerControls {
public:
  explicit CTweakPlayerControls(const SLdrPlayerControls& data) : mData(&data) {}

private:
  const SLdrPlayerControls* mData;
};
CHECK_SIZEOF(CTweakPlayerControls, 0x4)

extern rstl::single_ptr< CTweakPlayerControls > gpTweakPlayerControls;

#endif // _CTWEAKPLAYERCONTROLS
