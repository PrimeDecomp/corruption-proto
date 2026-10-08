#ifndef _CTWEAKGUICOLORS
#define _CTWEAKGUICOLORS

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakGuiColors;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakGuiColors {
public:
  explicit CTweakGuiColors(const SLdrTweakGuiColors& data) : mData(&data) {}

private:
  const SLdrTweakGuiColors* mData;
};
CHECK_SIZEOF(CTweakGuiColors, 0x4)

extern rstl::single_ptr< CTweakGuiColors > gpTweakGuiColors;

#endif // _CTWEAKGUICOLORS
