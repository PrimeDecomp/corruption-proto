#ifndef _CMAPWORLD
#define _CMAPWORLD

#include "types.h"

class CMapWorldInfo;
class IWorld;

// Minimal: only what CStateManagerObject uses.
class CMapWorld {
public:
  // Echoes' name and signature (0x800999DC).
  void RecalculateWorldSphere(const CMapWorldInfo& info, const IWorld& world) const;
};

#endif // _CMAPWORLD
