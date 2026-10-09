// G2MEAB prototype NonMatching translation unit.
// .text: 0x80030E8C..0x800332B0 (45 native functions).
//
// Echoes' CEntity.cpp, grown with the prototype's think-order lists, kSM_Attach helpers and the
// connection resolution used by generators. Functions are emitted in reverse source order; the
// rstl instances (vector copy/destruction/reserve, __distance) and CScriptMsg's constructor and
// destructor are emitted between them.
//
// Not implemented: 0x80032FF0 is rstl::vector<SConnection>::reserve and 0x80033088 its element
// copy loop; CScriptObjectLoaderHelper calls the former, and nothing here instantiates it.
#include "MetroidPrime/CEntity.hpp"

#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/TCastTo.hpp"

#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/SObjectTag.hpp"

#include "rstl/algorithm.hpp"

rstl::vector< SConnection > CEntity::NullConnectionList;

CEntityInfo CEntity::NullEntityInfo =
    CEntityInfo(kInvalidAreaId, NullConnectionList, true, kInvalidEditorId);

CEntityInfo::CEntityInfo(TAreaId aid, const rstl::vector< SConnection >& connections, bool isActive,
                         TEditorId eid, TEditorId x1c)
: mAreaId(aid)
, mConnections(connections)
, mEditorId(eid)
, mActive(isActive)
, mUpdateWhileOccluded(true)
, mUpdateDuringCinematicSkip(true)
, x1c_(x1c) {}

CEntity::CEntity(TUniqueId uid, const CEntityInfo& info, const rstl::string& name, uint castFlags)
: mAreaId(info.GetAreaId())
, mUniqueId(uid)
, mEditorId(info.GetEditorId())
, mName(name)
, mConnections(info.GetConnectionList())
, mActive(info.GetActive())
, mNotInArea(mAreaId == kInvalidAreaId)
, mCastFlags(castFlags)
, x54_8_(info.GetUpdateWhileOccluded())
, x54_9_(info.GetUpdateDuringCinematicSkip())
, x54_10_(false)
, x58_(info.GetX1C()) {}

CEntity::~CEntity() {}

void CEntity::AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) {
  switch (msg.GetMessage()) {
  case kSM_Activate:
    if (!GetActive()) {
      SetActive(true);
      SendScriptMsgs(kSS_Active, mgr, SScriptMsgOriginator(kInvalidUniqueId), kSM_Invalid);
    }
    break;
  case kSM_Deactivate:
    if (GetActive()) {
      SetActive(false);
      SendScriptMsgs(kSS_Inactive, mgr, SScriptMsgOriginator(kInvalidUniqueId), kSM_Invalid);
    }
    break;
  case kSM_ToggleActive: {
    EScriptObjectMessage next = GetActive() ? kSM_Deactivate : kSM_Activate;
    AcceptScriptMsg(mgr, CScriptMsg(next, msg.GetSenderId(), msg.GetTargetId(), msg.GetOriginator(),
                                    msg.GetState()));
    break;
  }
  case kSM_Delete:
    ClearThinkBefore(mgr);
    break;
  case kSM_AreaLoaded:
    // New in the prototype: connections with the think states order this entity's thinking
    // relative to their targets.
    for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
         it != mConnections.end(); ++it) {
      if (it->state == kSS_ThinkBefore) {
        AddThinkBefore(mgr, mgr.ObjectManager().GetIdForScript(it->objId));
      } else if (it->state == kSS_ThinkAfter) {
        AddThinkAfter(mgr, mgr.ObjectManager().GetIdForScript(it->objId));
      }
    }
    break;
  }
}

// Echoes passes a sender id; the prototype forwards a whole originator into every message.
void CEntity::SendScriptMsgs(EScriptObjectState state, CStateManager& mgr,
                             const SScriptMsgOriginator& originator, EScriptObjectMessage skipMsg) {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if (it->state == state && it->msg != skipMsg) {
      CStateManagerObject::TIdListResult search = mgr.ObjectManager().GetIdListForScript(it->objId);
      for (CStateManagerObject::TIdList::const_iterator current = search.first;
           current != search.second; ++current) {
        mgr.ObjectManager().SendScriptMsg(
            CScriptMsg(it->msg, GetUniqueId(), current->second, originator, state));
      }
    }
  }
}

void CEntity::PreThink(float dt, CStateManager& mgr) {}

void CEntity::Think(float dt, CStateManager& mgr) {}

void CEntity::SetActive(bool active) { mActive = active; }

TAreaId CEntity::GetAreaIdForPersistence() const { return mNotInArea ? kInvalidAreaId : mAreaId; }

// Called by the generators after they spawn copies of their connected objects: a connection
// between two copies is pointed at the matching copy, and a connection back to the generator's
// own editor id at the generator.
void CEntity::ResolveInstanceConnections(const rstl::vector< TUniqueId >& uids, TUniqueId owner,
                                         CStateManager& mgr) {
  rstl::vector< TEditorId > editorIds;
  editorIds.reserve(uids.size());
  rstl::vector< CEntity* > entities;
  entities.reserve(uids.size());

  CStateManagerObject& objectManager = mgr.ObjectManager();
  TEditorId ownerEditorId = objectManager.GetEditorIdForUniqueId(owner);
  for (rstl::vector< TUniqueId >::const_iterator it = uids.begin(); it != uids.end(); ++it) {
    CEntity* entity = objectManager.ObjectById(*it);
    editorIds.push_back(entity->GetEditorId());
    entities.push_back(entity);
  }

  for (int i = 0; i < entities.size(); ++i) {
    rstl::vector< SConnection >& connections = entities[i]->mConnections;
    for (rstl::vector< SConnection >::iterator conn = connections.begin();
         conn != connections.end(); ++conn) {
      rstl::vector< TEditorId >::iterator found =
          rstl::find(editorIds.begin(), editorIds.end(), conn->objId.mEditorId);
      if (found != editorIds.end()) {
        conn->objId.mUniqueId = uids[found - editorIds.begin()];
      } else if (conn->objId.mEditorId == ownerEditorId) {
        conn->objId.mUniqueId = owner;
      }
    }
  }
}

TUniqueId CEntity::CheckConnectedObject(const CStateManager& mgr, EScriptObjectState state,
                                        EScriptObjectMessage msg) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        if (rstl::distance(ids.first, ids.second) != 1) {
          for (CStateManagerObject::TIdList::const_iterator current = ids.first;
               current != ids.second; ++current) {
            rs_debugger_printf(
                "%s connected\n",
                mgr.ObjectManager().GetObjectById(current->second)->GetName().data());
          }
          RS_VERIFY_THROW(282, false, false,
                          CBasics::Stringize("More than one object connected to %s state on "
                                             "object %s with message %s.  Only one object is "
                                             "allowed.",
                                             rstl::string(SObjectTag::Type2Text(state)).data(),
                                             GetName().data(),
                                             rstl::string(SObjectTag::Type2Text(msg)).data()));
        }
        return ids.first->second;
      }
    }
  }
  return kInvalidUniqueId;
}

TUniqueId CEntity::CheckConnectedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                           EScriptObjectMessage msg,
                                           const CValidEntityPredicate& predicate) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        if (rstl::distance(ids.first, ids.second) != 1) {
          for (CStateManagerObject::TIdList::const_iterator current = ids.first;
               current != ids.second; ++current) {
            rs_debugger_printf(
                "%s connected\n",
                mgr.ObjectManager().GetObjectById(current->second)->GetName().data());
          }
          RS_VERIFY_THROW(330, false, false,
                          CBasics::Stringize("More than one object connected to %s state on "
                                             "object %s with message %s.  Only one object is "
                                             "allowed.",
                                             rstl::string(SObjectTag::Type2Text(state)).data(),
                                             GetName().data(),
                                             rstl::string(SObjectTag::Type2Text(msg)).data()));
        }
        if (predicate.IsValid(mgr, ids.first->second)) {
          return ids.first->second;
        }
      }
    }
  }
  return kInvalidUniqueId;
}

rstl::vector< TUniqueId > CEntity::FindConnectedObjects(const CStateManager& mgr,
                                                        EScriptObjectState state,
                                                        EScriptObjectMessage msg) const {
  rstl::vector< TUniqueId > result;
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        result.reserve(result.size() + rstl::distance(ids.first, ids.second));
        for (CStateManagerObject::TIdList::const_iterator current = ids.first;
             current != ids.second; ++current) {
          result.push_back(current->second);
        }
      }
    }
  }
  return result;
}

rstl::vector< TUniqueId >
CEntity::FindConnectedObjects_if(const CStateManager& mgr, EScriptObjectState state,
                                 EScriptObjectMessage msg,
                                 const CValidEntityPredicate& predicate) const {
  rstl::vector< TUniqueId > result;
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        result.reserve(result.size() + rstl::distance(ids.first, ids.second));
        for (CStateManagerObject::TIdList::const_iterator current = ids.first;
             current != ids.second; ++current) {
          if (predicate.IsValid(mgr, current->second)) {
            result.push_back(current->second);
          }
        }
      }
    }
  }
  return result;
}

TUniqueId CEntity::CheckAttachedObject(TUniqueId uid, const CStateManager& mgr,
                                       EScriptObjectState state) {
  const CEntity* entity = mgr.ObjectManager().GetObjectById(uid);
  if (entity != nullptr) {
    return entity->CheckAttachedObject(mgr, state);
  }
  return kInvalidUniqueId;
}

TUniqueId CEntity::CheckAttachedObject(const CStateManager& mgr, EScriptObjectState state) const {
  return CheckConnectedObject(mgr, state, kSM_Attach);
}

TUniqueId CEntity::FindAttachedObject(const CStateManager& mgr, EScriptObjectState state) const {
  return FindConnectedObject(mgr, state, kSM_Attach);
}

TUniqueId CEntity::CheckAttachedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                          const CValidEntityPredicate& predicate) const {
  return CheckConnectedObject_if(mgr, state, kSM_Attach, predicate);
}

rstl::vector< TUniqueId > CEntity::FindAttachedObjects(const CStateManager& mgr,
                                                       EScriptObjectState state) const {
  return FindConnectedObjects(mgr, state, kSM_Attach);
}

TUniqueId CEntity::FindConnectedObject(const CStateManager& mgr, EScriptObjectState state,
                                       EScriptObjectMessage msg) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        return ids.first->second;
      }
    }
  }
  return kInvalidUniqueId;
}

TUniqueId CEntity::FindConnectedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                          EScriptObjectMessage msg,
                                          const CValidEntityPredicate& predicate) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if ((state == kSS_InvalidState || state == it->state) &&
        (msg == kSM_Invalid || msg == it->msg)) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        if (predicate.IsValid(mgr, ids.first->second)) {
          return ids.first->second;
        }
      }
    }
  }
  return kInvalidUniqueId;
}

// Unlike FindConnectedObject_if, the state is not a wildcard here.
TUniqueId CEntity::FindAttachedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                         const CValidEntityPredicate& predicate) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if (state == it->state && it->msg == kSM_Attach) {
      CStateManagerObject::TIdListResult ids = mgr.ObjectManager().GetIdListForScript(it->objId);
      if (!(ids.first == ids.second)) {
        if (predicate.IsValid(mgr, ids.first->second)) {
          return ids.first->second;
        }
      }
    }
  }
  return kInvalidUniqueId;
}

SScriptObjectRef CEntity::GetAttachedObjectRef(EScriptObjectState state) const {
  for (rstl::vector< SConnection >::const_iterator it = mConnections.begin();
       it != mConnections.end(); ++it) {
    if (it->state == state && it->msg == kSM_Attach) {
      return it->objId;
    }
  }
  return SScriptObjectRef(kInvalidEditorId, kInvalidUniqueId);
}

// The think-order lists grow on demand; rstl::vector's push_back only asserts on this platform.
void CEntity::AddThinkBefore(CStateManager& mgr, TUniqueId uid) {
  CEntity* entity = TCastToPtr< CEntity >(mgr.ObjectManager().ObjectById(uid));
  if (entity != nullptr) {
    entity->AddThinkAfter(mgr, GetUniqueId());
    int size = x30_.size() + 1;
    if (size > x30_.capacity()) {
      int capacity = x30_.capacity() * 2;
      if (capacity < 4) {
        capacity = 4;
      }
      while (capacity < size) {
        capacity *= 2;
      }
      x30_.reserve(capacity);
    }
    x30_.push_back(uid);
  }
}

void CEntity::ClearThinkBefore(CStateManager& mgr) {
  for (rstl::vector< TUniqueId >::iterator it = x30_.begin(); it != x30_.end(); ++it) {
    CEntity* entity = TCastToPtr< CEntity >(mgr.ObjectManager().ObjectById(*it));
    if (entity != nullptr) {
      entity->RemoveThinkAfter(mgr, GetUniqueId());
    }
  }
  x30_.clear();
}

void CEntity::AddThinkAfter(CStateManager& mgr, TUniqueId uid) {
  int size = x20_.size() + 1;
  if (size > x20_.capacity()) {
    int capacity = x20_.capacity() * 2;
    if (capacity < 4) {
      capacity = 4;
    }
    while (capacity < size) {
      capacity *= 2;
    }
    x20_.reserve(capacity);
  }
  x20_.push_back(uid);
  x54_10_ = true;
}

bool CEntity::RemoveThinkAfter(CStateManager& mgr, TUniqueId uid) {
  rstl::vector< TUniqueId >::iterator it = rstl::find(x20_.begin(), x20_.end(), uid);
  if (it == x20_.end()) {
    return false;
  }
  *it = x20_.back();
  x20_.pop_back();
  x54_10_ = true;
  return true;
}

CValidEntityPredicate::~CValidEntityPredicate() {}

bool CValidEntityPredicate::IsValid(const CStateManager&, TUniqueId) const { return true; }
