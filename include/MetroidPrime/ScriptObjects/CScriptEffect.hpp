#ifndef _CSCRIPTEFFECT
#define _CSCRIPTEFFECT

#include "types.h"

#include "MetroidPrime/CActor.hpp"

// Minimal declaration; Echoes' class (ScriptObjects/CScriptEffect.cpp).
class CScriptEffect : public CActor {
public:
  // Echoes' name. 0x8007E648 clears the two particle counters; CStateManager's update calls it
  // first thing every frame.
  static void ResetParticleCounts();
};

#endif // _CSCRIPTEFFECT
