#ifndef _CANIMDATA
#define _CANIMDATA

#include "types.h"

// Minimal: only what main.cpp uses.
class CAnimData {
public:
  // Echoes names; both run over the same static cache (0x80795168) in CAnimData.cpp and are
  // called from CMain::InitializeSubsystems/ShutdownSubsystems like in Echoes.
  static void InitializeCache();
  static void FreeCache();
};

#endif // _CANIMDATA
