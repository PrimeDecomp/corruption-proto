#ifndef _CGAMEAREA
#define _CGAMEAREA

#include "types.h"

#include "MetroidPrime/IGameArea.hpp"

// Minimal declaration: only the interface is used so far.
class CStateManager;

class CGameArea : public IGameArea {
public:
  ~CGameArea();

  // Echoes' name; CStateManager's update calls it on the next area (0x8004E53C).
  void UpdateDocks(CStateManager& mgr);
  // Guessed name. 0x800516BC prints the area's script layers; CStateManager's update calls it
  // once when "Scripting Layers Verbose" is set to 2.
  void DumpScriptLayers(CStateManager& mgr);
};

#endif // _CGAMEAREA
