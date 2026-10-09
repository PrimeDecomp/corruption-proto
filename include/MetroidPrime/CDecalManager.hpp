#ifndef _CDECALMANAGER
#define _CDECALMANAGER

#include "types.h"

class CStateManager;

// Minimal: only what main.cpp and CStateManager use.
class CDecalManager {
public:
  // Echoes names, matched by their call sites in CMain::InitializeSubsystems/ShutdownSubsystems.
  static void Initialize();
  static void ShutDown();
  // Echoes' name; CStateManager's update calls it (0x800EB650).
  static void Update(float dt, CStateManager& mgr);
};

#endif // _CDECALMANAGER
