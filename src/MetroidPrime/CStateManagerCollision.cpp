// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8029B708..0x8029D208 (23 native functions).
// Source identity: asserted target basename; inferred root placement.
// Listed below are the functions not implemented yet (see CStateManagerCollision.hpp).
// 0x8029B708 +0x538: owned native method/helper retained; exact source-level name unresolved
// 0x8029BC40 +0x334: owned native method/helper retained; exact source-level name unresolved
// 0x8029BF74 +0x1EC: owned native method/helper retained; exact source-level name unresolved
// 0x8029C160 +0x50: owned native method/helper retained; exact source-level name unresolved
// 0x8029C1B0 +0xE4: owned native method/helper retained; exact source-level name unresolved
// 0x8029C294 +0x37C: projectile collision response; original source374
// 0x8029C610 +0x24: owned native method/helper retained; exact source-level name unresolved
// 0x8029C634 +0x24: owned native method/helper retained; exact source-level name unresolved
// 0x8029C658 +0x3C: owned native method/helper retained; exact source-level name unresolved
// 0x8029C694 +0x24: owned native method/helper retained; exact source-level name unresolved
// 0x8029C6B8 +0x140: UpdateActorInSortedLists (Prime's body; needs CActor's sorted-list dirty flag
//   accessor and the sorted list manager's ActorInLists/Insert/Move/Remove)
// 0x8029C7F8 +0xFC: owned native method/helper retained; exact source-level name unresolved
// 0x8029C8F4 +0x194: Echoes' CalculateObjectBounds (returns an optional CAABox)
// 0x8029CA88 +0x10C: owned native method/helper retained; exact source-level name unresolved
// 0x8029CB94 +0xDC: owned native method/helper retained; exact source-level name unresolved
// 0x8029CC70 +0xD0: owned native method/helper retained; exact source-level name unresolved
// 0x8029CD40 +0x2C: owned native method/helper retained; exact source-level name unresolved
// 0x8029CD6C +0x30: owned native method/helper retained; exact source-level name unresolved
// 0x8029CD9C +0x30: owned native method/helper retained; exact source-level name unresolved
// 0x8029D1D8 +0x30: registered static initializer; .ctors8065B968,seven independent SDA constants

#include "MetroidPrime/CStateManagerCollision.hpp"

#include "MetroidPrime/CGameCollision.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerCallbackLists.hpp"

#include "Kyoto/TFunctor.hpp"

CStateManagerCollision::CStateManagerCollision(CStateManager& mgr, CStateManagerObject& objects)
: mStateMgr(&mgr), mObjects(&objects), mSortedListManager(mgr) {
  mActiveChangedConnection = mgr.CallbackLists().ActiveChanged().Connect(
      TFunctor2FromMethod< CStateManagerCollision, CStateManager&, CEntity& >::Make(
          *this, &CStateManagerCollision::UpdateActorInSortedLists));
  mObjectAddedConnection = mgr.CallbackLists().ObjectAdded().Connect(
      TFunctor2FromMethod< CStateManagerCollision, CStateManager&, CEntity& >::Make(
          *this, &CStateManagerCollision::UpdateActorInSortedLists));
  CGameCollision::InitCollision(false);
}

CStateManagerCollision::~CStateManagerCollision() { CGameCollision::UninitializeCollision(); }
