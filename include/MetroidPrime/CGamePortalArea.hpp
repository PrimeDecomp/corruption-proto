#ifndef _CGAMEPORTALAREA
#define _CGAMEPORTALAREA

#include "types.h"

#include "MetroidPrime/TGameTypes.hpp"

// Minimal declaration, after CGamePortalArea.cpp; the prototype's counterpart of Echoes'
// CPortalArea. A loaded area's post-constructed data owns one at 0x134.
class CGamePortalArea {
public:
  // Echoes' name (0x8021287C). Unlike Echoes, it does not take the state manager.
  bool RemoveActor(const TUniqueId& uid);
};

#endif // _CGAMEPORTALAREA
