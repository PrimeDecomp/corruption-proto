#ifndef _CTWEAKTARGETING
#define _CTWEAKTARGETING

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakTargeting;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakTargeting {
public:
  explicit CTweakTargeting(const SLdrTweakTargeting& data) : mData(&data) {}

  // Guessed name, after the CGameDebug option it initializes; it returns the unnamed bool at
  // 0x274. Defined in TweaksAccessors.cpp.
  bool GetShowOrbitPoint() const;

private:
  const SLdrTweakTargeting* mData;
};
CHECK_SIZEOF(CTweakTargeting, 0x4)

extern rstl::single_ptr< CTweakTargeting > gpTweakTargeting;

#endif // _CTWEAKTARGETING
