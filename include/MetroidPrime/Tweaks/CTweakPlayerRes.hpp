#ifndef _CTWEAKPLAYERRES
#define _CTWEAKPLAYERRES

#include "types.h"

#include "Kyoto/Math/CMayaSpline.hpp"
#include "rstl/single_ptr.hpp"

struct SLdrTweakPlayerRes;

// Accessor over the loaded tweak record with cached resource IDs and splines.
class CTweakPlayerRes {
public:
  explicit CTweakPlayerRes(const SLdrTweakPlayerRes& data); // CTweakPlayerRes.cpp 0x802B05A8
  ~CTweakPlayerRes() {} // Emitted in TweakGlobals.cpp, 0x8002A4D4

private:
  uchar x0_[0x1f4]; // Cached resource IDs (CTweakPlayerRes.cpp 0x802AFADC)
  CMayaSpline x1f4_[5];
  uchar x348_[0x8];
};
CHECK_SIZEOF(CTweakPlayerRes, 0x350)

extern rstl::single_ptr< CTweakPlayerRes > gpTweakPlayerRes;

#endif // _CTWEAKPLAYERRES
