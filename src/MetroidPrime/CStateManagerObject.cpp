// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80298128..0x8029B708 (77 native functions). Listed below are the ones not yet
// implemented.
// Source identity: asserted target basename; inferred root placement.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80298130 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x80298150 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x802983C8 +0x1CC: Echoes' SendScriptMsg(const CScriptMsg&): queues the message (blocked on
//   the CGameDebug "ScriptDebug" option block)
// 0x80298594 +0x390: Echoes' message pump: drains the CScriptMsgQueue (same debug block)
// 0x802989A0 +0x4E8: owned native method/helper retained; exact source-level name unresolved
// 0x80298E88 +0xAC: owned native method/helper retained; exact source-level name unresolved
// 0x80298F34 +0xC4: owned native method/helper retained; exact source-level name unresolved
// 0x80298FF8 +0x188: owned native method/helper retained; exact source-level name unresolved
// 0x8029A46C +0x74: world setter: takes an rstl::auto_ptr<CWorld>& and releases it into mWorld
// 0x8029A72C +0xC80: state manager object constructor; original source61/70/75..89 (news nine
//   CObjectList and five CObjectListSmall subclasses whose names are unknown)
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

#include "Kyoto/Alloc/Assert.hpp"

CStateManagerObject::~CStateManagerObject() {}

CWorld* CStateManagerObject::World() { return mWorld.get(); }

const CWorld* CStateManagerObject::GetWorld() const { return mWorld.get(); }

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
    bool contained = list->Contains(entity);
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
