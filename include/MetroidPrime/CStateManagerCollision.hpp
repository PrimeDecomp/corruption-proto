#ifndef _CSTATEMANAGERCOLLISION
#define _CSTATEMANAGERCOLLISION

#include "types.h"

class CStateManager;
class CStateManagerObject;

// Minimal declaration. Guessed name, after CStateManagerCollision.cpp, which holds its
// constructor (0x8029CECC) and destructor (0x8029CDCC). CStateManager allocates it (0x1C038
// bytes) and keeps it at 0x8.
class CStateManagerCollision {
public:
  // CStateManager passes itself and its entity database, which is built first.
  CStateManagerCollision(CStateManager& mgr, CStateManagerObject& objects);
  ~CStateManagerCollision();

private:
  uchar x0_[0x1C038];
};
CHECK_SIZEOF(CStateManagerCollision, 0x1C038)

#endif // _CSTATEMANAGERCOLLISION
