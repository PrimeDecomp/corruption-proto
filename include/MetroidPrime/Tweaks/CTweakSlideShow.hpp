#ifndef _CTWEAKSLIDESHOW
#define _CTWEAKSLIDESHOW

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakSlideShow;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakSlideShow {
public:
  explicit CTweakSlideShow(const SLdrTweakSlideShow& data) : mData(&data) {}

private:
  const SLdrTweakSlideShow* mData;
};
CHECK_SIZEOF(CTweakSlideShow, 0x4)

extern rstl::single_ptr< CTweakSlideShow > gpTweakSlideShow;

#endif // _CTWEAKSLIDESHOW
