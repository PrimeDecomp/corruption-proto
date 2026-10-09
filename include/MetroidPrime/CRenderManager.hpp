#ifndef _CRENDERMANAGER
#define _CRENDERMANAGER

#include "types.h"

#include "Kyoto/TSignal1.hpp"

class CStateManager;

// Minimal declaration. The class name comes from the "CRenderManager.cpp(2108) : " allocation
// string. CStateManager owns it (0x780 bytes, polymorphic) and the constructor (0x802AA5F4) keeps
// the state manager at +4.
class CRenderManager {
public:
  // Guessed names. A signal at +0x604 that the world render pass emits with the state manager
  // after the opaque geometry; CActor draws its collision boxes from it. The argument type is
  // inferred: CActor's bridge for it is a different instantiation than its CStateManager& one.
  typedef TSignal1< const CStateManager& > DebugDrawSignal;
  rstl::auto_ptr< IConnection > ConnectDebugDraw(DebugDrawSignal::Functor functor); // 0x802A48E4
};

#endif // _CRENDERMANAGER
