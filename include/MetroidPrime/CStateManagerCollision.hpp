#ifndef _CSTATEMANAGERCOLLISION
#define _CSTATEMANAGERCOLLISION

#include "types.h"

#include "MetroidPrime/CSortedLists.hpp"

#include "Kyoto/TSignal1.hpp"

#include "rstl/auto_ptr.hpp"

class CEntity;
class CStateManager;
class CStateManagerObject;

// Guessed name, after CStateManagerCollision.cpp, which holds its constructor (0x8029CECC) and
// destructor (0x8029CDCC). CStateManager allocates it (0x1C038 bytes) and keeps it at 0x8.
//
// What it owns: the sorted list manager (the actor bounds lists that near-list queries walk), and
// the two signal connections that keep an actor's entry up to date. Its other functions are the
// collision queries Echoes kept on CStateManager: thin forwarders to the sorted list manager's
// near-list builders (0x8029C610, 0x8029C634, 0x8029C658, 0x8029C694, called from actors and
// AI), wrappers over CGameCollision (0x8029CD6C, 0x8029CD9C), the object bounds calculation
// (0x8029C8F4) and the projectile collision response (0x8029C294).
//
// Differences from Echoes: Echoes' CStateManager kept the sorted list manager in its
// CStateManagerContainer and called UpdateActorInSortedLists itself from AddObject, from
// DispatchScriptMessages when a message changed an object's active state, and from
// UpdateSortedLists; RemoveObject removed the actor. Here the entity database
// (CStateManagerObject) knows nothing about the sorted lists: this object subscribes to the
// CStateManagerCallbackLists object-added and active-changed signals instead, and the sorted list
// manager subscribes to the object-removed signal itself. The collision primitive tables
// (CGameCollision::InitCollision and UninitializeCollision) are also set up and torn down here
// rather than by the state manager's own constructor and destructor.
class CStateManagerCollision {
public:
  // CStateManager passes itself and its entity database, which is built first.
  CStateManagerCollision(CStateManager& mgr, CStateManagerObject& objects);
  ~CStateManagerCollision();

  // Echoes' names for the passes CStateManager's update makes in this order (Echoes' update
  // calls UpdateSortedLists, MovePlatforms, MoveActors and the player's CGameCollision::Move);
  // MovePlayer is a guessed name.
  void UpdateSortedLists();     // 0x8029C7F8
  void MovePlatforms(float dt); // 0x8029C1B0
  void MoveActors(float dt);    // 0x8029BF74
  void MovePlayer(float dt);    // 0x8029C160

private:
  // 0x8029C6B8. Prime's UpdateActorInSortedLists body, run for every added entity and every
  // entity whose active state changes: a dirty actor that uses the sorted lists is inserted,
  // moved or removed according to its bounds and active state.
  void UpdateActorInSortedLists(CStateManager& mgr, CEntity& entity);

  CStateManager* mStateMgr;
  CStateManagerObject* mObjects;
  SL::CSortedListManager mSortedListManager; // Echoes' name
  // Guessed names.
  rstl::auto_ptr< IConnection > mActiveChangedConnection;
  rstl::auto_ptr< IConnection > mObjectAddedConnection;
};
CHECK_SIZEOF(CStateManagerCollision, 0x1C038)

#endif // _CSTATEMANAGERCOLLISION
