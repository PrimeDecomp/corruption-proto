#ifndef _CTWEAKGUI
#define _CTWEAKGUI

#include "types.h"

#include "rstl/single_ptr.hpp"

struct SLdrTweakGui;

// Accessor over the loaded tweak record; created by TweaksLoader.cpp's CreateTweakGlobals.
class CTweakGui {
public:
  explicit CTweakGui(const SLdrTweakGui& data) : mData(&data) {}

  // Guessed names, after the CGameDebug options they initialize: the SLdrTweakGui::misc fields
  // they read have no known property names (record offset in the comment). Defined in
  // TweaksAccessors.cpp.
  int GetHudCameraFov() const;              // 0xB8
  int GetHudCameraY() const;                // 0xBC
  int GetHudCameraZ() const;                // 0xC0
  int GetRadarMode() const;                 // 0x108
  int GetEnableHud() const;                 // 0x10C
  int GetEnableAutoMapper() const;          // 0x110
  int GetEnableTargeting() const;           // 0x118
  int GetFaceReflectionWidth() const;       // 0x14C
  int GetFaceReflectionHeight() const;      // 0x150
  int GetFaceReflectionPositionY() const;   // 0x154
  int GetFaceReflectionPositionZ() const;   // 0x158
  int GetFaceReflectionAspectRatio() const; // 0x15C

private:
  const SLdrTweakGui* mData;
};
CHECK_SIZEOF(CTweakGui, 0x4)

extern rstl::single_ptr< CTweakGui > gpTweakGui;

#endif // _CTWEAKGUI
