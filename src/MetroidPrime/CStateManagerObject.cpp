// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80298128..0x8029B708 (77 native functions). Listed below are the ones not yet
// implemented.
// Source identity: asserted target basename; inferred root placement.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80298130 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x80298150 +0x20: owned native method/helper retained; exact source-level name unresolved
// 0x802981B0 +0xB4: owned native method/helper retained; exact source-level name unresolved
// 0x80298264 +0x78: owned native method/helper retained; exact source-level name unresolved
// 0x802982DC +0x84: owned native method/helper retained; exact source-level name unresolved
// 0x802983C8 +0x1CC: Echoes' SendScriptMsg(const CScriptMsg&) shape: appends to the CScriptMsgQueue
// and pumps it past 0x80 entries 0x80298594 +0x390: Echoes' DispatchScriptMessages shape: drains
// the CScriptMsgQueue at 0x10D8 0x80298924 +0x7C: invokes one CStateManagerCallbackLists list
// (0x570 added, 0x588 removed, 0x5A0 active changed) with (mgr, entity) 0x802989A0 +0x4E8: owned
// native method/helper retained; exact source-level name unresolved 0x80298E88 +0xAC: owned native
// method/helper retained; exact source-level name unresolved 0x80298F34 +0xC4: owned native
// method/helper retained; exact source-level name unresolved 0x80298FF8 +0x188: owned native
// method/helper retained; exact source-level name unresolved 0x80299408 +0x98: GetIdForScript-like
// lookup over an {TEditorId, TUniqueId} pair 0x802995E8 +0x12C: GetIdListForScript-like lookup over
// an {TEditorId, TUniqueId} pair 0x80299880 +0x14C: owned native method/helper retained; exact
// source-level name unresolved 0x80299B18 +0x1B0: Echoes' RemoveObject(TUniqueId); no area or
// sorted-list work here 0x80299CC8 +0x88: owned native method/helper retained; exact source-level
// name unresolved 0x80299D50 +0x78: owned native method/helper retained; exact source-level name
// unresolved 0x80299DC8 +0x4C: owned native method/helper retained; exact source-level name
// unresolved 0x80299E14 +0x4C: owned native method/helper retained; exact source-level name
// unresolved 0x80299E60 +0x174: Echoes' AddObject(CEntity&); no area or sorted-list work here,
// delivers kSM_Create 0x80299FD4 +0xAC: add object; original nonnull assertion284 0x8029A080 +0xCC:
// Echoes' SetCurrentAreaId(TAreaId) shape (CMapWorldInfo visited flags, CMapWorld refresh)
// 0x8029A298 +0x88: Echoes' DeleteObjectRequest(TUniqueId): sends kSM_Delete
// 0x8029A3C0 +0x4: owned native method/helper retained; exact source-level name unresolved
// 0x8029A3C4 +0x40: Echoes' FreeScriptObjects(TAreaId) shape: forwards to the loader helper
// 0x8029A404 +0x2C: owned native method/helper retained; exact source-level name unresolved
// 0x8029A46C +0x74: world setter taking an auto_ptr<CWorld>
// 0x8029A4E0 +0x1D8: owned native method/helper retained; exact source-level name unresolved
// 0x8029A6B8 +0x74: owned native method/helper retained; exact source-level name unresolved
// 0x8029A72C +0xC80: state manager object constructor; original source61/70/75..89
// 0x8029B3AC +0x60: owned native method/helper retained; exact source-level name unresolved
// 0x8029B480 +0x74: owned native method/helper retained; exact source-level name unresolved
// 0x8029B4F4 +0x17C: owned native method/helper retained; exact source-level name unresolved
// 0x8029B670 +0x68: owned native method/helper retained; exact source-level name unresolved
// 0x8029B6D8 +0x30: registered static initializer; .ctors8065B964,seven independent SDA constants

#include "MetroidPrime/CStateManagerObject.hpp"

#include "MetroidPrime/CEntity.hpp"

#include "Kyoto/Alloc/Assert.hpp"

CWorld* CStateManagerObject::World() { return mWorld.get(); }

const CWorld* CStateManagerObject::GetWorld() const { return mWorld.get(); }

bool CStateManagerObject::HasWorld() const { return mWorld.get() != nullptr; }

TAreaId CStateManagerObject::GetNextAreaId() const { return mNextAreaId; }

TAreaId CStateManagerObject::GetPreviousAreaId() const { return mPreviousAreaId; }

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

CStateManagerObject::TIdList& CStateManagerObject::ScriptIdMap() { return mScriptIdMap; }

CStateManagerObject::TIdListResult
CStateManagerObject::GetIdListForScript(TEditorId editorId) const {
  const TIdListResult range = mScriptIdMap.equal_range(editorId);
  return range;
}

TUniqueId CStateManagerObject::GetIdForScript(TEditorId editorId) const {
  TIdList::const_iterator it = mScriptIdMap.find(editorId);
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

CScriptMsgQueue* CStateManagerObject::ScriptMsgQueue() const { return mScriptMsgs; }

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
