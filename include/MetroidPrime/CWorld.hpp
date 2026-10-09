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

  // Guessed name and owner. Clears the static list of locked CTokens (0x8077E108, constructed by
  // CWorld.cpp's static initializer) that 0x80037188 fills from a world's current area on the
  // console's network-asset reload. Called by CMain::ShutdownSubsystems and CGameArea. The
  // configured split places it at the end of CActor.cpp, after that unit's static initializer.
  static void ClearLockedTokens();
};

#endif // _CWORLD
