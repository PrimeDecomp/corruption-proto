#ifndef _CGAMECOLLISION
#define _CGAMECOLLISION

#include "types.h"

// Minimal view (CGameCollision.cpp): what CStateManagerCollision calls.
class CGameCollision {
public:
  // Echoes' names: the collision primitive type and collider tables. CStateManagerCollision's
  // constructor and destructor call them, where Echoes' CStateManager did. Echoes passes the
  // state manager; here the argument is a flag (CStateManagerCollision passes false) that, when
  // set, also allocates a 0xC800-byte buffer with a CCallStack (0x8013546C).
  static void InitCollision(bool flag);
  static void UninitializeCollision(); // 0x80135238
};

#endif // _CGAMECOLLISION
