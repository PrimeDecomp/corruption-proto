#ifndef _CWORLD
#define _CWORLD

#include "MetroidPrime/IWorld.hpp"

class CMapWorld;

// Minimal: only what CStateManagerObject uses.
class CWorld : public IWorld {
public:
  virtual ~CWorld();

  // Echoes' name and signature; reads the map world through the resource at 0x38 (0x80039038).
  CMapWorld* GetMapWorld() const;
};

#endif // _CWORLD
