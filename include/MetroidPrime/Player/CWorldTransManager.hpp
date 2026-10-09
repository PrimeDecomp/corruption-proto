#ifndef _CWORLDTRANSMANAGER
#define _CWORLDTRANSMANAGER

#include "types.h"

// Minimal view of the world-transition manager (CWorldTransManager.cpp). Names follow Echoes.
class CWorldTransManager {
public:
  // 0x80176FF4. CGameState.cpp emits the rc_ptr instance (0x8015EC2C) that CStateManager's
  // destructor calls to release its copy.
  ~CWorldTransManager();

  void TouchModels(); // 0x801769D4
  // 0x80175870. CMFGame calls it after its first tick.
  void EndTransition();
};

#endif // _CWORLDTRANSMANAGER
