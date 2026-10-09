#ifndef _CACTORLIGHTS
#define _CACTORLIGHTS

#include "types.h"

// Minimal view of Echoes' actor lights (CActorLights.cpp); only the dirty flag at 0x2C8, which the
// console's RELOADAREALIGHTS sets on every render actor so it picks its lights again.
class CActorLights {
public:
  void SetDirty() { mIsDirty = true; } // Echoes' name

private:
  uchar x0_[0x2C8];
  bool mIsDirty : 1;
};

#endif // _CACTORLIGHTS
