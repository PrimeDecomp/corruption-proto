#ifndef _CDECALMANAGER
#define _CDECALMANAGER

#include "types.h"

// Minimal: only what main.cpp uses.
class CDecalManager {
public:
  // Echoes names, matched by their call sites in CMain::InitializeSubsystems/ShutdownSubsystems.
  static void Initialize();
  static void ShutDown();
};

#endif // _CDECALMANAGER
