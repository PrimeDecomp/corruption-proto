#ifndef _CCAMERAMANAGER
#define _CCAMERAMANAGER

#include "types.h"

class CCinematicCamera;

// Minimal view of Echoes' camera manager (Cameras/CCameraManager.cpp); CDisplayManager owns it.
// Only what CStateManager reads is placed.
class CCameraManager {
public:
  // Guessed name. FrameBegin reads it inline; CDisplayManager's GetCinematicCamera (0x802A36C8)
  // returns it too.
  CCinematicCamera* GetCinematicCamera() const { return mCinematicCamera; }

private:
  uchar x0_[0x34];
  CCinematicCamera* mCinematicCamera;
};

#endif // _CCAMERAMANAGER
