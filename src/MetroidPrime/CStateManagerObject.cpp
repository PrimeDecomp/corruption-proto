// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80298128..0x8029B708 (77 native functions). Listed below are the ones not yet
// implemented.
// Source identity: asserted target basename; inferred root placement.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80298130 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x80298150 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x8029B6D8 +0x30: registered static initializer; .ctors 0x8065B964; seven SDA constants

#include "MetroidPrime/CStateManagerObject.hpp"

#include "MetroidPrime/CEntity.hpp"
#include "MetroidPrime/CMapWorld.hpp"
#include "MetroidPrime/CMapWorldInfo.hpp"
#include "MetroidPrime/CObjectListSmall.hpp"
#include "MetroidPrime/CScriptMailbox.hpp"
#include "MetroidPrime/CScriptMsgQueue.hpp"
#include "MetroidPrime/CScriptObjectLoaderHelper.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerCallbackLists.hpp"
#include "MetroidPrime/CWorld.hpp"
#include "MetroidPrime/GameObjectLists.hpp"

#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/TCastTo.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Network/CBBASupport.hpp"

#include "rstl/algorithm.hpp"
#include "rstl/string.hpp"

class CActor;
class CGameCamera;
class CPatterned;
class CScriptCameraHint;
class CScriptControlHint;
class CScriptDock;
class CScriptDoor;
class CScriptEffect;
class CScriptLayerController;
class CScriptPlatform;
class CScriptPlayerHint;
class CScriptSound;
class CScriptTrigger;
class CScriptWaypoint;
class CWeapon;

// Echoes' CStateManager setup of the same members. The list ids are Echoes' indices; the small
// lists are built dock first, as Echoes builds its filtered lists. Only the dynamic lists go into
// the second views.
CStateManagerObject::CStateManagerObject(
    CStateManager& mgr, const rstl::ncrc_ptr< CStringPropertyManager >& stringProperties,
    const rstl::ncrc_ptr< CScriptMailbox >& mailbox,
    const rstl::ncrc_ptr< CMapWorldInfo >& mapWorldInfo)
: mStateMgr(&mgr)
, mLastUniqueId(0)
, mObjectIndexArray(0)
, mObjectLists(rstl::auto_ptr< CObjectList >())
, mObjectListsSmall(rstl::auto_ptr< CObjectListSmall >())
, mAllocatedObjectIndices(2048, false)
, mScriptMsgs(rs_new_line(61) CScriptMsgQueue())
, mWorld(nullptr)
, mScriptObjectLoaderHelper(rs_new_line(70) CScriptObjectLoaderHelper())
, mNextAreaId(0)
, mPreviousAreaId(kInvalidAreaId)
, mStringPropertyManager(stringProperties)
, mMailbox(mailbox)
, mMapWorldInfo(mapWorldInfo)
, mPlayer(nullptr)
, mDispatchingScriptMessages(false) {
  mObjectLists[kOL_All] = rs_new_line(75) CObjectList(kOL_All, false);
  mObjectLists[kOL_Actor] = rs_new_line(76) CActorList();
  mObjectLists[kOL_RenderActor] = rs_new_line(77) CRenderActorList();
  mObjectLists[kOL_PhysicsActor] = rs_new_line(78) CPhysicsActorList();
  mObjectLists[kOL_GameLight] = rs_new_line(79) CGameLightList();
  mObjectLists[kOL_ListeningAi] = rs_new_line(80) CListeningAiList();
  mObjectLists[kOL_AiWaypoint] = rs_new_line(81) CAiWaypointList();
  mObjectLists[kOL_Platform] = rs_new_line(82) CPlatformList();
  mObjectLists[kOL_Trigger] = rs_new_line(83) CTriggerList();

  mObjectListsSmall[kOLS_Dock] = rs_new_line(85) CDockListSmall();
  mObjectListsSmall[kOLS_Door] = rs_new_line(86) CDoorListSmall();
  mObjectListsSmall[kOLS_Type106] = rs_new_line(87) CType106ListSmall();
  mObjectListsSmall[kOLS_GameCamera] = rs_new_line(88) CGameCameraListSmall();
  mObjectListsSmall[kOLS_GrapplePoint] = rs_new_line(89) CGrapplePointListSmall();

  for (int i = 0; i < mObjectLists.size(); ++i) {
    CObjectList* list = mObjectLists[i].get();
    if (list->IsDynamic()) {
      mDynamicObjectLists.push_back(list);
    }
  }
  for (int i = 0; i < mObjectListsSmall.size(); ++i) {
    CObjectListSmall* list = mObjectListsSmall[i].get();
    if (list->IsDynamic()) {
      mDynamicObjectListsSmall.push_back(list);
    }
  }
}

CStateManagerObject::~CStateManagerObject() {}

void CStateManagerObject::SetWorld(rstl::auto_ptr< CWorld > world) { mWorld = world.release(); }

CWorld* CStateManagerObject::World() { return mWorld.get(); }

CWorld* CStateManagerObject::GetWorld() const { return mWorld.get(); }

bool CStateManagerObject::HasWorld() const { return mWorld.get() != nullptr; }

TAreaId CStateManagerObject::GetNextAreaId() const { return mNextAreaId; }

TAreaId CStateManagerObject::GetPreviousAreaId() const { return mPreviousAreaId; }

void CStateManagerObject::AreaLoaded(TAreaId area) { mMailbox->SendMsgs(area, *mStateMgr); }

void CStateManagerObject::PrepareAreaUnload(TAreaId area) {
  ScriptObjectLoaderHelper()->FreeScriptObjects(area, *mStateMgr);
}

void CStateManagerObject::AreaUnloaded(TAreaId area) {}

const CObjectList& CStateManagerObject::GetObjectListById(int id) const {
  return *mObjectLists[id];
}

CObjectList& CStateManagerObject::ObjectListById(int id) { return *mObjectLists[id]; }

const CObjectListSmall& CStateManagerObject::GetObjectListSmallById(int id) const {
  return *mObjectListsSmall[id];
}

CObjectListSmall& CStateManagerObject::ObjectListSmallById(int id) {
  return *mObjectListsSmall[id];
}

const CEntity* CStateManagerObject::GetObjectById(TUniqueId uid) const {
  return GetAllObjectList().GetObjectById(uid);
}

CEntity* CStateManagerObject::ObjectById(TUniqueId uid) {
  return mObjectLists[0]->GetObjectById(uid);
}

void CStateManagerObject::DeleteObjectRequest(TUniqueId uid) {
  SendScriptMsg(uid, kInvalidUniqueId, kSM_Delete, SScriptMsgOriginator(kInvalidUniqueId));
}

void CStateManagerObject::ClearGraveyard() {
  for (rstl::list< rstl::reserved_vector< CEntity*, 32 > >::iterator it = mGraveyard.begin();
       it != mGraveyard.end(); ++it) {
    rstl::reserved_vector< CEntity*, 32 >& batch = *it;
    for (rstl::reserved_vector< CEntity*, 32 >::iterator entity = batch.begin();
         entity != batch.end(); ++entity) {
      delete *entity;
    }
  }
  mGraveyard.clear();
}

// Unlike Echoes, there is no area-change bookkeeping beyond the visited flag and the map sphere.
void CStateManagerObject::SetCurrentAreaId(TAreaId area) {
  if (mNextAreaId != area) {
    mPreviousAreaId = mNextAreaId;
    mNextAreaId = area;
  }

  if (area != kInvalidAreaId && !MapWorldInfo()->IsAreaVisited(area)) {
    MapWorldInfo()->SetAreaVisited(area, true);
    const CMapWorld* mapWorld = mWorld->GetMapWorld();
    mapWorld->RecalculateWorldSphere(*GetMapWorldInfo(), *World());
  }
}

void CStateManagerObject::AddObject(CEntity* object) {
  RS_VERIFY_THROW(284, object != NULL, false, "Can't add a null object");
  if (object) {
    AddObject(*object);
  }
}

void CStateManagerObject::AddObject(CEntity& entity) {
  TEditorId editorId = entity.GetEditorId();
  TUniqueId uid = entity.GetUniqueId();
  if (editorId != kInvalidEditorId) {
    mScriptIdMap.insert(rstl::pair< TEditorId, TUniqueId >(editorId, uid));
  }

  for (rstl::reserved_vector< rstl::auto_ptr< CObjectList >, 9 >::iterator it =
           mObjectLists.begin();
       it != mObjectLists.end(); ++it) {
    (*it)->AddObject(entity);
  }
  for (rstl::reserved_vector< rstl::auto_ptr< CObjectListSmall >, 5 >::iterator it =
           mObjectListsSmall.begin();
       it != mObjectListsSmall.end(); ++it) {
    (*it)->AddObject(entity);
  }

  DeliverScriptMsg(CScriptMsg(kSM_Create, kInvalidUniqueId, entity.GetUniqueId(),
                              SScriptMsgOriginator(kInvalidUniqueId), kSS_InvalidState));
  mStateMgr->CallbackLists().ObjectAdded().Emit(*mStateMgr, entity);
}

void CStateManagerObject::RemoveObject(TUniqueId uid) {
  if (CEntity* entity = ObjectListById(0).GetObjectById(uid)) {
    TEditorId editorId = entity->GetEditorId();
    if (editorId != kInvalidEditorId) {
      rstl::pair< TIdList::iterator, TIdList::iterator > range = mScriptIdMap.equal_range(editorId);
      TIdList::iterator it = range.first;
      while (it != range.second) {
        if (it->second == uid) {
          it = mScriptIdMap.erase(it);
        } else {
          ++it;
        }
      }
    }
    mStateMgr->CallbackLists().ObjectRemoved().Emit(*mStateMgr, *entity);
  }

  for (int i = 0; i < mObjectLists.size(); ++i) {
    mObjectLists[i]->RemoveObject(uid);
  }
  for (int i = 0; i < mObjectListsSmall.size(); ++i) {
    mObjectListsSmall[i]->RemoveObject(uid);
  }
  mAllocatedObjectIndices[uid.value & 0xFFFF] = false;
}

const CScriptObjectLoaderHelper* CStateManagerObject::GetScriptObjectLoaderHelper() const {
  return mScriptObjectLoaderHelper.get();
}

CScriptObjectLoaderHelper* CStateManagerObject::ScriptObjectLoaderHelper() {
  return mScriptObjectLoaderHelper.get();
}

// Unlike Echoes, the generation is reset to zero rather than bumped, so a slot's ids repeat.
TUniqueId CStateManagerObject::AllocateUniqueId() {
  ushort startId = mLastUniqueId;
  ushort id;
  do {
    id = mLastUniqueId;
    mLastUniqueId = (id + 1) % 2048;
    RS_VERIFY_THROW(389, mLastUniqueId != startId, false, "Object list is full");
  } while (mAllocatedObjectIndices[id]);

  ushort& generation = mObjectIndexArray[id];
  generation = 0;
  mAllocatedObjectIndices[id] = true;
  return TUniqueId(id | (generation << 16));
}

// Echoes' body, over the unowned dynamic views of both kinds of list.
void CStateManagerObject::UpdateObjectInLists(CEntity& entity) {
  for (rstl::reserved_vector< CObjectList*, 9 >::iterator it = mDynamicObjectLists.begin();
       it != mDynamicObjectLists.end(); ++it) {
    const bool contained =
        static_cast< const CObjectList* >(*it)->GetObjectById(entity.GetUniqueId()) != nullptr;
    if (contained && !(*it)->IsQualified(entity)) {
      (*it)->RemoveObject(entity.GetUniqueId());
    } else if (!contained) {
      (*it)->AddObject(entity);
    }
  }

  for (rstl::reserved_vector< CObjectListSmall*, 5 >::iterator it =
           mDynamicObjectListsSmall.begin();
       it != mDynamicObjectListsSmall.end(); ++it) {
    CObjectListSmall* list = *it;
    bool contained = list->IsObjectInList(&entity);
    if (contained && !list->IsQualified(entity)) {
      list->RemoveObject(entity);
    } else if (!contained) {
      list->AddObject(entity);
    }
  }
}

CStateManagerObject::TIdList& CStateManagerObject::ScriptIdMap() { return mScriptIdMap; }

CStateManagerObject::TIdListResult
CStateManagerObject::GetIdListForScript(TEditorId editorId) const {
  const TIdListResult range = mScriptIdMap.equal_range(editorId);
  return range;
}

// With a cached unique id, the result is narrowed to that one entry of its editor id's range.
CStateManagerObject::TIdListResult
CStateManagerObject::GetIdListForScript(const SScriptObjectRef& ref) const {
  if (ref.mUniqueId == kInvalidUniqueId) {
    return GetIdListForScript(ref.mEditorId);
  }

  const CEntity* entity = GetObjectById(ref.mUniqueId);
  if (entity == nullptr) {
    return TIdListResult(mScriptIdMap.end(), mScriptIdMap.end());
  }

  TIdListResult range = mScriptIdMap.equal_range(entity->GetEditorId());
  for (TIdList::const_iterator it = range.first; it != range.second; ++it) {
    if (it->second == ref.mUniqueId) {
      TIdList::const_iterator next = it;
      ++next;
      return TIdListResult(it, next);
    }
  }
  return TIdListResult(mScriptIdMap.end(), mScriptIdMap.end());
}

TUniqueId CStateManagerObject::GetIdForScript(TEditorId editorId) const {
  TIdList::const_iterator it = mScriptIdMap.find(editorId);
  if (it != mScriptIdMap.end()) {
    return it->second;
  }
  return kInvalidUniqueId;
}

TUniqueId CStateManagerObject::GetIdForScript(const SScriptObjectRef& ref) const {
  if (ref.mUniqueId != kInvalidUniqueId) {
    return ref.mUniqueId;
  }
  TIdList::const_iterator it = mScriptIdMap.find(ref.mEditorId);
  if (it != mScriptIdMap.end()) {
    return it->second;
  }
  return kInvalidUniqueId;
}

TEditorId CStateManagerObject::GetEditorIdForUniqueId(TUniqueId uid) const {
  const CEntity* entity = GetObjectById(uid);
  if (entity != nullptr) {
    return entity->GetEditorId();
  }
  return kInvalidEditorId;
}

void CStateManagerObject::AddToGraveyard(CEntity* entity) {
  if (mGraveyard.empty()) {
    rstl::reserved_vector< CEntity*, 32 > batch;
    mGraveyard.push_back(batch);
  } else if (mGraveyard.back().size() == mGraveyard.back().capacity()) {
    rstl::reserved_vector< CEntity*, 32 > batch;
    mGraveyard.push_back(batch);
  }

  mGraveyard.back().push_back(entity);
}

// The choices of "Script msg Sender" and "Script msg Target". The classes behind the type ids
// come from the vtables whose TypesMatch override compares that id (CPatterned's cast tests the
// cast flag that only its constructor passes to CAi).
bool CStateManagerObject::MatchesScriptMsgEntity(const CEntity* entity, int choice) {
  switch (choice) {
  case 0:
    return true;
  case 1:
    return TCastToConstPtr< CActor >(entity) != nullptr;
  case 2:
    return TCastToConstPtr< CPlayer >(entity) != nullptr;
  case 3:
    return TCastToConstPtr< CScriptTrigger >(entity) != nullptr;
  case 4:
    return TCastToConstPtr< CScriptPlatform >(entity) != nullptr;
  case 5:
    return TCastToConstPtr< CPatterned >(entity) != nullptr;
  case 6:
    return TCastToConstPtr< CGameCamera >(entity) != nullptr;
  case 7:
    return TCastToConstPtr< CScriptCameraHint >(entity) != nullptr;
  case 8:
    return TCastToConstPtr< CScriptPlayerHint >(entity) != nullptr;
  case 9:
    return TCastToConstPtr< CScriptControlHint >(entity) != nullptr;
  case 10:
    return TCastToConstPtr< CWeapon >(entity) != nullptr;
  case 11:
    return TCastToConstPtr< CScriptDoor >(entity) != nullptr;
  case 12:
    return TCastToConstPtr< CScriptDock >(entity) != nullptr;
  case 13:
    return TCastToConstPtr< CScriptEffect >(entity) != nullptr;
  case 14:
    return TCastToConstPtr< CScriptLayerController >(entity) != nullptr;
  case 15:
    return TCastToConstPtr< CScriptWaypoint >(entity) != nullptr;
  case 16:
    return TCastToConstPtr< CScriptSound >(entity) != nullptr;
  }
  return false;
}

// The choices of "Script msg Type" and "Script msg Exclude Type"; 0 (any) is never looked up.
bool CStateManagerObject::MatchesScriptMsgMessage(EScriptObjectMessage msg, int choice) {
  const EScriptObjectMessage messages[] = {
      kSM_Invalid,   kSM_Action,       kSM_Activate,    kSM_Deactivate,  kSM_Increment,
      kSM_Decrement, kSM_Start,        kSM_Stop,        kSM_SetToZero,   kSM_Entered,
      kSM_Damage,    kSM_EnteredFluid, kSM_ExitedFluid, kSM_InsideFluid,
  };
  if (choice == 0) {
    return true;
  }
  if (choice < ARRAY_SIZE(messages)) {
    return messages[choice] == msg;
  }
  return false;
}

// The choices of "Script msg State" and "Script msg Exclude State".
bool CStateManagerObject::MatchesScriptMsgState(EScriptObjectState state, int choice) {
  const EScriptObjectState states[] = {
      kSS_InvalidState, kSS_Active, kSS_Arrived, kSS_Damage,     kSS_Dead,
      kSS_Entered,      kSS_Exited, kSS_Inside,  kSS_MaxReached, kSS_Zero,
  };
  if (choice == 0) {
    return true;
  }
  if (choice < ARRAY_SIZE(states)) {
    return states[choice] == state;
  }
  return false;
}

// Guessed names. The messages that "Script msg Debugger" leaves out below a given level: level 1
// skips both lists, level 2 only the second, frequent one, and higher levels log everything.
struct SScriptMsgList {
  const EScriptObjectMessage* mMessages;
  int mCount;
};

static const EScriptObjectMessage skDamageMessages[] = {
    kSM_Damage,
    kSM_ResistedDamage,
    kSM_ReflectedDamage,
};

static const EScriptObjectMessage skFrequentMessages[] = {
    kSM_Landed,
    kSM_LandedOnStaticGround,
    kSM_Entered,
    kSM_Clear,
    kSM_OffGround,
    kSM_OnIce,
    kSM_OnOrganic,
    kSM_OnDirt,
    kSM_HitObject,
    kSM_OnPlatform,
    kSM_Falling,
    kSM_WorldLoaded,
    kSM_EnteredFluid,
    kSM_InsideFluid,
    kSM_ExitedFluid,
    kSM_Launching,
    kSM_AcidOnVisor,
    kSM_InShrubbery,
    kSM_EnteredPhazonPool,
    kSM_InsidePhazonPool,
    kSM_ExitedPhazonPool,
    kSM_AIUpdateDisabled,
    kSM_InternalMessage0,
    kSM_InternalMessage1,
    kSM_InternalMessage2,
    kSM_InternalMessage3,
    kSM_InternalMessage4,
    kSM_InternalMessage5,
    kSM_InternalMessage6,
    kSM_InternalMessage7,
    kSM_InternalMessage8,
    kSM_InternalMessage9,
};

static const SScriptMsgList skQuietScriptMsgs[] = {
    {skDamageMessages, ARRAY_SIZE(skDamageMessages)},
    {skFrequentMessages, ARRAY_SIZE(skFrequentMessages)},
};

bool CStateManagerObject::ShouldLogScriptMsg(const CEntity* target, const CEntity* sender,
                                             const CScriptMsg& msg) {
  const int level = gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgDebugger);
  if (level == 0) {
    return false;
  }

  const EScriptObjectMessage message = msg.GetMessage();
  for (int i = 0; i < ARRAY_SIZE(skQuietScriptMsgs); ++i) {
    if (level <= i + 1) {
      const EScriptObjectMessage* end =
          skQuietScriptMsgs[i].mMessages + skQuietScriptMsgs[i].mCount;
      if (rstl::find(skQuietScriptMsgs[i].mMessages, end, message) != end) {
        return false;
      }
    }
  }

  // Inactive objects only log the messages that can wake them up.
  if (!target->GetActive() && msg.GetMessage() != kSM_Activate &&
      msg.GetMessage() != kSM_Increment) {
    return false;
  }
  if (msg.GetMessage() == kSM_Delete && sender == nullptr) {
    return false;
  }

  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgSender) != 0 &&
      !MatchesScriptMsgEntity(sender, gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgSender))) {
    return false;
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgTarget) != 0 &&
      !MatchesScriptMsgEntity(target, gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgTarget))) {
    return false;
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgType) != 0 &&
      !MatchesScriptMsgMessage(msg.GetMessage(),
                               gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgType))) {
    return false;
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgExcludeType) != 0 &&
      MatchesScriptMsgMessage(msg.GetMessage(),
                              gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgExcludeType))) {
    return false;
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgState) != 0 &&
      !MatchesScriptMsgState(msg.GetState(),
                             gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgState))) {
    return false;
  }
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgExcludeState) != 0 &&
      MatchesScriptMsgState(msg.GetState(),
                            gpGameDebug->GetOptionInt(CGameDebug::kDO_ScriptMsgExcludeState))) {
    return false;
  }
  return true;
}

// The name and the CEntity word at 0x58 that the "ScriptDebug" lines print for a message's
// sender and target; the object may be gone.
static inline const char* GetDebugName(const CEntity* entity) {
  return entity != nullptr ? entity->GetName().data() : "UNKNOWN";
}

static inline uint GetDebugId(const CEntity* entity) {
  return entity != nullptr ? entity->GetX58().value : -1;
}

void CStateManagerObject::DispatchScriptMessages() {
  while (!mScriptMsgs->empty()) {
    CScriptMsg msg = mScriptMsgs->Pop();

    if (gpGameDebug->IsOptionSet(CGameDebug::kDO_LogScriptMessageQueue)) {
      const CEntity* sender = GetObjectById(msg.GetSenderId());
      const CEntity* target = GetObjectById(msg.GetTargetId());
      rstl::string text(CBasics::Stringize(
          "ScriptDebug %d 0x%x \"%s[%d] - OUT\" 0x%x 0x%x 0x%x \"%s[%d]\"\n",
          mStateMgr->GetUpdateFrameIndex(), GetDebugId(sender), GetDebugName(sender),
          msg.GetSenderId().value & 0xFFFF, msg.GetState(), msg.GetMessage(), GetDebugId(target),
          GetDebugName(target), msg.GetTargetId().value & 0xFFFF));
      CBBASupport::SendString(text, 0, nullptr);
    }

    CEntity* entity = ObjectById(msg.GetTargetId());
    if (entity != nullptr && gpGameDebug->IsOptionSet(CGameDebug::kDO_ScriptMsgDebugger)) {
      const CEntity* sender = GetObjectById(msg.GetSenderId());
      if (ShouldLogScriptMsg(entity, sender, msg)) {
        rstl::string text(CBasics::Stringize("ScriptDebug %d 0x%x \"%s\" 0x%x 0x%x 0x%x \"%s\"\n",
                                             mStateMgr->GetUpdateFrameIndex(), GetDebugId(sender),
                                             GetDebugName(sender), msg.GetState(), msg.GetMessage(),
                                             entity->GetX58().value, entity->GetName().data()));
        CBBASupport::SendString(text, 0, nullptr);
      }
    }

    if (entity != nullptr) {
      const bool wasActive = entity->GetActive();
      entity->AcceptScriptMsg(*mStateMgr, msg);
      if (wasActive != entity->GetActive()) {
        mStateMgr->CallbackLists().ActiveChanged().Emit(*mStateMgr, *entity);
      }

      if (msg.GetMessage() == kSM_Delete) {
        AddToGraveyard(entity);
        RemoveObject(entity->GetUniqueId());
      }
    }
  }
}

// Unlike Echoes, the message can first be logged, falling back to the debugger output when the
// broadband adapter is not connected.
void CStateManagerObject::SendScriptMsg(const CScriptMsg& msg) {
  if (gpGameDebug->IsOptionSet(CGameDebug::kDO_LogScriptMessageQueue)) {
    const CEntity* sender = GetObjectById(msg.GetSenderId());
    const CEntity* target = GetObjectById(msg.GetTargetId());
    rstl::string text(CBasics::Stringize(
        "ScriptDebug %d 0x%x \"%s[%d] - IN\" 0x%x 0x%x 0x%x \"%s[%d]\"\n",
        mStateMgr->GetUpdateFrameIndex(), GetDebugId(sender), GetDebugName(sender),
        msg.GetSenderId().value & 0xFFFF, msg.GetState(), msg.GetMessage(), GetDebugId(target),
        GetDebugName(target), msg.GetTargetId().value & 0xFFFF));
    if (!CBBASupport::SendString(text, 0, nullptr)) {
      rs_debugger_printf(text.data());
    }
  }

  mScriptMsgs->Push(msg);
  if (mScriptMsgs->Size() > 0x80 && !mDispatchingScriptMessages) {
    mDispatchingScriptMessages = true;
    DispatchScriptMessages();
    mDispatchingScriptMessages = false;
  }
}

void CStateManagerObject::DeliverScriptMsg(const CScriptMsg& msg) {
  if (CEntity* entity = ObjectById(msg.GetTargetId())) {
    entity->AcceptScriptMsg(*mStateMgr, msg);
  }
}

void CStateManagerObject::SendScriptMsg(CEntity* target, TUniqueId sender, EScriptObjectMessage msg,
                                        const SScriptMsgOriginator& originator) {
  if (target) {
    SendScriptMsg(CScriptMsg(msg, sender, target->GetUniqueId(), originator, kSS_InvalidState));
  }
}

void CStateManagerObject::SendScriptMsg(TUniqueId target, TUniqueId sender,
                                        EScriptObjectMessage msg,
                                        const SScriptMsgOriginator& originator) {
  SendScriptMsg(CScriptMsg(msg, sender, target, originator, kSS_InvalidState));
}

void CStateManagerObject::SendScriptMsg(const rstl::vector< TUniqueId >& targets, TUniqueId sender,
                                        EScriptObjectMessage msg,
                                        const SScriptMsgOriginator& originator) {
  for (rstl::vector< TUniqueId >::const_iterator it = targets.begin(); it != targets.end(); ++it) {
    TUniqueId target = *it;
    SendScriptMsg(CScriptMsg(msg, sender, target, originator, kSS_InvalidState));
  }
}

CScriptMsgQueue* CStateManagerObject::ScriptMsgQueue() const { return mScriptMsgs.get(); }

CScriptMailbox* CStateManagerObject::Mailbox() const { return mMailbox.GetPtr(); }

CStringPropertyManager* CStateManagerObject::StringPropertyManager() {
  return mStringPropertyManager.GetPtr();
}

const CStringPropertyManager* CStateManagerObject::GetStringPropertyManager() const {
  return mStringPropertyManager.GetPtr();
}

const CMapWorldInfo* CStateManagerObject::GetMapWorldInfo() const { return mMapWorldInfo.GetPtr(); }

CMapWorldInfo* CStateManagerObject::MapWorldInfo() { return mMapWorldInfo.GetPtr(); }

const CPlayer* CStateManagerObject::GetPlayer() const { return mPlayer; }

CPlayer* CStateManagerObject::Player() { return mPlayer; }

void CStateManagerObject::SetPlayer(CPlayer* player) { mPlayer = player; }
