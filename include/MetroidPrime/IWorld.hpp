#ifndef _IWORLD
#define _IWORLD

#include "types.h"

// Echoes' world interface. Only the virtual destructor is declared: CStateManagerObject deletes
// its world through vtable slot 0x8. CMapWorld takes the world through this interface.
class IWorld {
public:
  virtual ~IWorld();
};

#endif // _IWORLD
