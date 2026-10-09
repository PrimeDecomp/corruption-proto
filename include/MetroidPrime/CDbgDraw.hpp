#ifndef _CDBGDRAW
#define _CDBGDRAW

#include "types.h"

class CStateManager;

// Minimal declaration. The class name comes from CDbgDraw.cpp. CGameGlobalObjects owns the
// instance (at 0xA2C8) and publishes it at 0x80797110; it keeps timed debug primitives in lists.
class CDbgDraw {
public:
  // Guessed name. 0x8002EF58 ages the timed primitives by the frame time and drops the expired
  // ones; CStateManager's update calls it while the game runs.
  void Update(float dt, CStateManager& mgr);
};

extern CDbgDraw* gpDbgDraw; // Guessed name

#endif // _CDBGDRAW
