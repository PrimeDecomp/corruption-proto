#ifndef _CCAMERAMANAGER
#define _CCAMERAMANAGER

#include "types.h"

class CCinematicCamera;
class CGameCamera;
class CStateManager;

// Minimal view of Echoes' camera manager (Cameras/CCameraManager.cpp); CDisplayManager owns it.
// Only what CStateManager reads is placed.
class CCameraManager {
public:
  // Guessed name. FrameBegin reads it inline; CDisplayManager's GetCinematicCamera (0x802A36C8)
  // returns it too.
  CCinematicCamera* GetCinematicCamera() const { return mCinematicCamera; }

  // Echoes' name and signature (0x801E82A4): deactivates the cinematic camera, clears its id and
  // lets the player update its cinematic state. CStateManager::SpecialSkipCinematic calls it.
  void StopCinematics(CStateManager& mgr);
  // Guessed name. 0x801E8914 starts the cinematic camera whose id waits at 0x1C (through
  // 0x801E85BC, which looks up the CScriptCinematicCamera) unless it is already the one at 0x20,
  // then clears the id. CStateManager::PreThinkObjects calls it first.
  void StartPendingCinematic(CStateManager& mgr);

  // Guessed name. CDisplayManager's debug camera activation (0x802A3310) makes this camera the
  // current one and its deactivation (0x802A32A0) switches back; the console commands save and
  // restore its transform.
  CGameCamera* GetDebugCamera() const { return mDebugCamera; }

private:
  uchar x0_[0x2C];
  CGameCamera* mDebugCamera;
  uchar x30_[0x34 - 0x30];
  CCinematicCamera* mCinematicCamera;
};

#endif // _CCAMERAMANAGER
