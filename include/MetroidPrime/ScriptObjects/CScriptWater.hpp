#ifndef _CSCRIPTWATER
#define _CSCRIPTWATER

#include "types.h"

#include "MetroidPrime/CActor.hpp"

#include "Kyoto/Math/CPlane.hpp"

// Minimal declaration. TypesMatch id 0x5A; the vtable (lbl_806B3AC8) starts with a function in
// CScriptWater.cpp. Echoes derives it from CScriptTrigger; the layout past CActor is not modelled.
class CScriptWater : public CActor {
public:
  // Echoes' name. The world-space surface plane, facing up at the height stored at 0x188 (Echoes
  // reads the top of the trigger bounds). CActor.cpp emits it (0x80036F50).
  // Guessed name. Echoes' TestBombHittingWater reads the top of the trigger bounds instead.
  float GetSurfaceHeight() const { return x188_; }
  CPlane GetWRSurfacePlane() const {
    return CPlane(x188_, CUnitVector3f(0.f, 0.f, 1.f, CUnitVector3f::kN_Yes));
  }

private:
  uchar xf8_[0x188 - 0xF8];
  float x188_;
};

#endif // _CSCRIPTWATER
