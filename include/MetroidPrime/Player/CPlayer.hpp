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

private:
  uchar xF8_[0x304C - 0xF8];
  float mDeathTime; // Echoes' name
};

#endif // _CPLAYER
