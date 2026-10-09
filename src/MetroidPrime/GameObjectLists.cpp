// NonMatching translation-unit scaffold.
// G2MEAB .text 0x801482C4..0x80148720 (end exclusive).
// Echoes' GameObjectLists.cpp with a render actor list added after the actor list, the trigger
// list moved up after the physics actor list, and the AI waypoint list also taking targeting
// points. Listed below are the functions not implemented yet.
// 0x80148484 +0x58: CListeningAiList::IsQualified (needs CPatterned's IsListening, vtable slot
//   0xD8, which no header models yet)
// 0x801486F0 +0x30: static initializer for TGameTypes.hpp's seven SDA constants

#include "MetroidPrime/GameObjectLists.hpp"

#include "MetroidPrime/CEntity.hpp"
#include "MetroidPrime/TCastTo.hpp"

class CActor;
class CGameLight;
class CPhysicsActor;
class CRenderActor;
class CScriptAIHint;
class CScriptAiJumpPoint;
class CScriptCoverPoint;
class CScriptPlatform;
class CScriptTargetingPoint;
class CScriptTrigger;

CActorList::CActorList() : CObjectList(kOL_Actor, false) {}

uchar CActorList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CActor >(entity) != nullptr;
}

CRenderActorList::CRenderActorList() : CObjectList(kOL_RenderActor, false) {}

uchar CRenderActorList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CRenderActor >(entity) != nullptr;
}

CPhysicsActorList::CPhysicsActorList() : CObjectList(kOL_PhysicsActor, false) {}

uchar CPhysicsActorList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CPhysicsActor >(entity) != nullptr;
}

CTriggerList::CTriggerList() : CObjectList(kOL_Trigger, false) {}

uchar CTriggerList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CScriptTrigger >(entity) != nullptr;
}

CListeningAiList::CListeningAiList() : CObjectList(kOL_ListeningAi, true) {}

CAiWaypointList::CAiWaypointList() : CObjectList(kOL_AiWaypoint, false) {}

// Unlike Echoes, targeting points (type id 0x53) count as AI waypoints too.
uchar CAiWaypointList::IsQualified(const CEntity& entity) {
  uchar ret = false;
  if (TCastToConstPtr< CScriptCoverPoint >(entity) != nullptr) {
    ret = true;
  } else if (TCastToConstPtr< CScriptAiJumpPoint >(entity) != nullptr) {
    ret = true;
  } else if (TCastToConstPtr< CScriptAIHint >(entity) != nullptr) {
    ret = true;
  } else if (TCastToConstPtr< CScriptTargetingPoint >(entity) != nullptr) {
    ret = true;
  }
  return ret;
}

CPlatformList::CPlatformList() : CObjectList(kOL_Platform, false) {}

uchar CPlatformList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CScriptPlatform >(entity) != nullptr;
}

CGameLightList::CGameLightList() : CObjectList(kOL_GameLight, false) {}

uchar CGameLightList::IsQualified(const CEntity& entity) {
  return TCastToConstPtr< CGameLight >(entity) != nullptr;
}
