#ifndef _CCINEMATICCAMERA
#define _CCINEMATICCAMERA

#include "types.h"

#include "MetroidPrime/Cameras/CGameCamera.hpp"

// Minimal view of Echoes' cinematic camera (Cameras/CCinematicCamera.cpp); only what
// CStateManager reads is placed, the rest is padding.
class CCinematicCamera : public CGameCamera {
public:
  // Guessed name. The cinematic script object the camera plays ("Cinematic script object went
  // away! Have to quit right now"); FrameBegin prints its name.
  TUniqueId GetCinematicObjectId() const { return x208_; }
  // Echoes' names. Unlike Echoes' 0x200, the update tests 0x100 before applying the slow-motion
  // scale; the camera manager's 0x801E80E8 tests 0x2.
  uint GetFlags() const { return mFlags; }
  float GetSlowMotionScale() const { return mSlowMotionScale; }

private:
  uchar xF8_[0x208 - 0xF8];
  TUniqueId x208_;
  uchar x20c_[0x4];
  uint mFlags;
  float mSlowMotionScale;
};

#endif // _CCINEMATICCAMERA
