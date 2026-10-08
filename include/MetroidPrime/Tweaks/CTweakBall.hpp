#ifndef _CTWEAKBALL
#define _CTWEAKBALL

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakBall;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakBall {
public:
  explicit CTweakBall(const SLdrTweakBall& data) : mData(&data) {}

private:
  const SLdrTweakBall* mData;
};
CHECK_SIZEOF(CTweakBall, 0x4)

extern rstl::single_ptr< CTweakBall > gpTweakBall;

#endif // _CTWEAKBALL
