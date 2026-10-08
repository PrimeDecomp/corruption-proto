#ifndef _CTWEAKGUI
#define _CTWEAKGUI

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakGui;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakGui {
public:
  explicit CTweakGui(const SLdrTweakGui& data) : mData(&data) {}

private:
  const SLdrTweakGui* mData;
};
CHECK_SIZEOF(CTweakGui, 0x4)

extern rstl::single_ptr< CTweakGui > gpTweakGui;

#endif // _CTWEAKGUI
