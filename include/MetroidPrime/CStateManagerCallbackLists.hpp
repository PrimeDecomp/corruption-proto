#ifndef _CSTATEMANAGERCALLBACKLISTS
#define _CSTATEMANAGERCALLBACKLISTS

#include "types.h"

#include "Kyoto/TSignal1.hpp"
#include "Kyoto/TSignal2.hpp"

class CEntity;
class CRenderManager;
class CStateManager;

// Guessed name, after CStateManagerCallbackLists.cpp. CStateManager allocates it (0x5E8 bytes)
// and keeps it at 0x0. It is 63 separate signals (0x18 bytes each): the constructor (0x8030008C)
// is the inlined list constructors and the destructor (0x802FFB1C) destroys them one by one in
// reverse order. Five signal types, by the destructor instance each one gets:
//
// - 0x0..0x438: 46 update-phase signals. CStateManager's update (0x80292E9C) emits each of them
//   once, in turn, with the state manager and the frame time (instance at 0x80294444).
// - 0x450..0x498 and 0x4B0..0x558: two groups of render-phase signals that CRenderManager.cpp
//   emits with the render manager itself (instances at 0x802A6A20 and 0x802A65B8). CFluidPlane
//   and CScriptFalsePerspective connect to the first group; CMorphBall and CScriptSubtitles to
//   the second.
// - 0x570..0x5A0: the entity notifications that CStateManagerObject fires. CGameArea listens to
//   the added and removed signals, CSortedLists to the removed one and CStateManagerCollision's
//   constructor (0x8029CECC) to the added and active-changed ones; Echoes' AddObject and
//   RemoveObject updated those structures directly.
// - 0x5B8 and 0x5D0: two TSignal1<CStateManager&> (the destructor instance is emitted in
//   main.cpp; CStateManager.cpp emits them).
//
// Echoes and Prime have no equivalent; their state manager calls each subsystem directly.
class CStateManagerCallbackLists {
public:
  typedef TSignal2< CStateManager&, float > TUpdateSignal;
  // Guessed argument types. Both groups pass the render manager and their code is the same, so
  // they differ only in the type; which group takes the const reference is not known.
  typedef TSignal1< CRenderManager& > TRenderSignal;
  typedef TSignal1< const CRenderManager& > TConstRenderSignal;
  typedef TSignal2< CStateManager&, CEntity& > TEntitySignal;
  typedef TSignal1< CStateManager& > TStateManagerSignal;

  CStateManagerCallbackLists();
  ~CStateManagerCallbackLists();

  // Guessed names. AddObject (0x80299E60) fires the first after kSM_Create, RemoveObject
  // (0x80299B18) the second before unlisting, and the message pump (0x80298594) the third when
  // a message flips the target's active flag.
  TEntitySignal& ObjectAdded() { return mObjectAdded; }
  TEntitySignal& ObjectRemoved() { return mObjectRemoved; }
  TEntitySignal& ActiveChanged() { return mActiveChanged; }
  // Guessed names. CStateManager's destructor emits the first before anything else and the
  // second once the objects are gone, before its members are destroyed.
  TStateManagerSignal& StateManagerDestroying() { return x5b8_; }
  TStateManagerSignal& StateManagerDestroyed() { return x5d0_; }

  // Guessed names. The update emits these 46 in this order; each is named after the work it
  // precedes (Pre) or follows (Post). Several are emitted back to back or only under the
  // same condition as the work they surround.
  TUpdateSignal& FrameStart() { return x0_; }
  TUpdateSignal& PreMapWorldSphere() { return x18_; }
  TUpdateSignal& PrePlayTime() { return x30_; }
  TUpdateSignal& PreRenderClock() { return x48_; }
  TUpdateSignal& PrePowerUps() { return x60_; }
  TUpdateSignal& PrePreThink() { return x78_; }
  TUpdateSignal& PreFluidPlanes() { return x90_; }
  TUpdateSignal& PreSortedLists() { return xa8_; }
  TUpdateSignal& PostSortedLists() { return xc0_; }
  TUpdateSignal& PreMovePlatforms() { return xd8_; }
  TUpdateSignal& PostMovePlatforms() { return xf0_; }
  TUpdateSignal& PreMoveActors() { return x108_; }
  TUpdateSignal& PostMoveActors() { return x120_; }
  TUpdateSignal& PrePlayerInput() { return x138_; }
  TUpdateSignal& PostPlayerInput() { return x150_; }
  TUpdateSignal& PreMovePlayer() { return x168_; }
  TUpdateSignal& PostMovePlayer() { return x180_; }
  TUpdateSignal& PreSortedLists2() { return x198_; }
  TUpdateSignal& PostSortedLists2() { return x1b0_; }
  TUpdateSignal& PreTouch() { return x1c8_; }
  TUpdateSignal& PostTouch() { return x1e0_; }
  TUpdateSignal& PreThink() { return x1f8_; }
  TUpdateSignal& PostThink() { return x210_; }
  TUpdateSignal& PreGameplayChecks() { return x228_; }
  TUpdateSignal& PreWorldUpdate() { return x240_; }
  TUpdateSignal& PostWorldUpdate() { return x258_; }
  TUpdateSignal& PreAreaSounds() { return x270_; }
  TUpdateSignal& PostAreaSounds() { return x288_; }
  TUpdateSignal& PreDocks() { return x2a0_; }
  TUpdateSignal& PostDocks() { return x2b8_; }
  TUpdateSignal& PreGameMode() { return x2d0_; }
  TUpdateSignal& PostGameMode() { return x2e8_; }
  TUpdateSignal& PreDispatch() { return x300_; }
  TUpdateSignal& PostDispatch() { return x318_; }
  TUpdateSignal& PreCameras() { return x330_; }
  TUpdateSignal& PostCameras() { return x348_; }
  TUpdateSignal& PreDispatch2() { return x360_; }
  TUpdateSignal& PostDispatch2() { return x378_; }
  TUpdateSignal& PrePostUpdatePlayer() { return x390_; }
  TUpdateSignal& PostPostUpdatePlayer() { return x3a8_; }
  TUpdateSignal& PreWorldState() { return x3c0_; }
  TUpdateSignal& PostWorldState() { return x3d8_; }
  TUpdateSignal& PreTravel() { return x3f0_; }
  TUpdateSignal& PostTravel() { return x408_; }
  TUpdateSignal& PreGraveyard() { return x420_; }
  TUpdateSignal& PostGraveyard() { return x438_; }

private:
  TUpdateSignal x0_;
  TUpdateSignal x18_;
  TUpdateSignal x30_;
  TUpdateSignal x48_;
  TUpdateSignal x60_;
  TUpdateSignal x78_;
  TUpdateSignal x90_;
  TUpdateSignal xa8_;
  TUpdateSignal xc0_;
  TUpdateSignal xd8_;
  TUpdateSignal xf0_;
  TUpdateSignal x108_;
  TUpdateSignal x120_;
  TUpdateSignal x138_;
  TUpdateSignal x150_;
  TUpdateSignal x168_;
  TUpdateSignal x180_;
  TUpdateSignal x198_;
  TUpdateSignal x1b0_;
  TUpdateSignal x1c8_;
  TUpdateSignal x1e0_;
  TUpdateSignal x1f8_;
  TUpdateSignal x210_;
  TUpdateSignal x228_;
  TUpdateSignal x240_;
  TUpdateSignal x258_;
  TUpdateSignal x270_;
  TUpdateSignal x288_;
  TUpdateSignal x2a0_;
  TUpdateSignal x2b8_;
  TUpdateSignal x2d0_;
  TUpdateSignal x2e8_;
  TUpdateSignal x300_;
  TUpdateSignal x318_;
  TUpdateSignal x330_;
  TUpdateSignal x348_;
  TUpdateSignal x360_;
  TUpdateSignal x378_;
  TUpdateSignal x390_;
  TUpdateSignal x3a8_;
  TUpdateSignal x3c0_;
  TUpdateSignal x3d8_;
  TUpdateSignal x3f0_;
  TUpdateSignal x408_;
  TUpdateSignal x420_;
  TUpdateSignal x438_;
  TRenderSignal x450_;
  TRenderSignal x468_;
  TRenderSignal x480_;
  TRenderSignal x498_;
  TConstRenderSignal x4b0_;
  TConstRenderSignal x4c8_;
  TConstRenderSignal x4e0_;
  TConstRenderSignal x4f8_;
  TConstRenderSignal x510_;
  TConstRenderSignal x528_;
  TConstRenderSignal x540_;
  TConstRenderSignal x558_;
  TEntitySignal mObjectAdded;
  TEntitySignal mObjectRemoved;
  TEntitySignal mActiveChanged;
  TStateManagerSignal x5b8_;
  TStateManagerSignal x5d0_;
};
CHECK_SIZEOF(CStateManagerCallbackLists, 0x5E8)

#endif // _CSTATEMANAGERCALLBACKLISTS
