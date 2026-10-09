#ifndef _CPLAYER
#define _CPLAYER

#include "types.h"

#include "MetroidPrime/CPhysicsActor.hpp"

// Minimal declaration; Echoes' hierarchy. The layout past CActor is not modelled except for the
// death time, which CStateManager's update and Think read.
class CPlayer : public CPhysicsActor {
public:
  // Echoes' name. Positive once the player died; the state manager then skips most of the
  // update and only lets the player think.
  float GetDeathTime() const { return mDeathTime; }
  // Echoes' name and signature; 0x800139F0. CStateManager::PostUpdatePlayer calls it.
  void PostUpdate(float dt, CStateManager& mgr);
  // Echoes' names and bodies (0x80013F48, 0x80013EC4): the player thinks, then the object whose
  // id it keeps at 0x3058 (Echoes' death effect). The state manager calls them once the player
  // is dead.
  void DoThink(float dt, CStateManager& mgr);
  void DoPreThink(float dt, CStateManager& mgr);

private:
  uchar xF8_[0x304C - 0xF8];
  float mDeathTime; // Echoes' name
};

#endif // _CPLAYER
