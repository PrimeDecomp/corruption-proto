#ifndef _CAI
#define _CAI

#include "types.h"

#include "MetroidPrime/CPhysicsActor.hpp"

#include "rstl/single_ptr.hpp"

class CGenericFSM2State;
class CStateManager;

// Partial layout from the constructor (0x8009F754; vtable 0x806B337C, adds cast flag 0x10) and
// the destructor (0x8009F4D0). Only what CStateManager reads is named so far. The constructor
// also sets up a timer at 0x2E0 (CAi's think, 0x8009F40C, adds the frame time to it), an owned
// 0x1080-byte object at 0x2E4, a string at 0x2E8, seven owned connections at 0x2F8..0x330, two
// members copied from constructor arguments at 0x340 and 0x368, an optional 'FSM2' token at 0x390
// (Echoes' state machine token) and two floats at 0x39C.
class CAi : public CPhysicsActor {
public:
  ~CAi();

  // Guessed name, after Echoes' CStateManager::ShouldUpdatePatterned, which the prototype moved
  // here (0x8009E310): false during a cinematic pause, in an area occluded for more than five
  // seconds, or while the AI debug options pause or disable the AIs. CStateManager::Think and
  // CStateManagerCollision's actor movement skip the AIs it rejects.
  bool ShouldUpdate(CStateManager& mgr);

  // Guessed names.
  const CGenericFSM2State* GetStateMachineState() const { return mStateMachineState.get(); }
  bool IsAiActive() const { return x3a8_25_; }

private:
  uchar x2e0_[0x33C - 0x2E0];
  rstl::single_ptr< CGenericFSM2State > mStateMachineState; // Guessed name
  uchar x340_[0x3A8 - 0x340];
  // Both set by the constructor. CAi's think only runs the state machine while x3a8_25_ is set.
  bool x3a8_24_ : 1;
  bool x3a8_25_ : 1;
};

#endif // _CAI
