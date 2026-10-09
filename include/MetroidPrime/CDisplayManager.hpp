#ifndef _CDISPLAYMANAGER
#define _CDISPLAYMANAGER

#include "types.h"

class CCameraManager;
class CCinematicCamera;
class CStateManager;

// Minimal declaration. Guessed name, after CDisplayManager.cpp, which holds its destructor
// (0x802A422C). CStateManager owns it at 0x14 and deletes it in its destructor.
class CDisplayManager {
public:
  ~CDisplayManager();

  // Guessed names. 0x802A34D0 asserts ("Fatal error, no Player Camera Manager available!")
  // and returns the camera manager at 0x38.
  CCameraManager* PlayerCameraManager();
  // The camera manager's cinematic test (Echoes' name, 0x802A3700) and a stricter one that also
  // checks the cinematic camera's 0x2 flag (0x802A3750, guessed name). CStateManager scales the
  // frame time and counts FrameBegin's glitch frames with the second; play time, hints and the
  // escape timer use the first.
  bool IsInCinematicCamera();
  bool IsCinematicActive();
  CCinematicCamera* GetCinematicCamera(); // Echoes' name; 0x802A36C8
  // Guessed name. Updates every camera manager and the viewports (0x802A388C).
  void Update(float dt, CStateManager& mgr);
};

#endif // _CDISPLAYMANAGER
