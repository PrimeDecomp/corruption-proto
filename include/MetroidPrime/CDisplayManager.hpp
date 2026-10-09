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

  // Guessed names. Both assert ("Fatal error, no Player Camera Manager available!") and return
  // the camera manager at 0x38; the const one sits at line 351 (0x802A34D0), the other at line
  // 365 (0x802A343C). Callers that change the camera manager (StopCinematics, the pending
  // cinematic, setting the current camera) use the second; the readers (the current-camera
  // getter on a const camera manager, which uses the const object lookup) the first.
  const CCameraManager* PlayerCameraManager() const;
  CCameraManager* PlayerCameraManager();
  // The camera manager's cinematic test (Echoes' name, 0x802A3700) and a stricter one that also
  // checks the cinematic camera's 0x2 flag (0x802A3750, guessed name). CStateManager scales the
  // frame time and counts FrameBegin's glitch frames with the second; play time, hints and the
  // escape timer use the first.
  bool IsInCinematicCamera();
  bool IsCinematicActive();
  CCinematicCamera* GetCinematicCamera(); // Echoes' name; 0x802A36C8
  // Guessed name. The flag at 0x124 (0x802A3188), which the debug camera's activation (0x802A3310)
  // sets and its deactivation (0x802A32A0) clears.
  bool IsDebugCameraActive() const;
  // Guessed name. Updates every camera manager and the viewports (0x802A388C).
  void Update(float dt, CStateManager& mgr);
};

#endif // _CDISPLAYMANAGER
