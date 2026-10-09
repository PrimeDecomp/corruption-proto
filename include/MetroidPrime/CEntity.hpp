#ifndef _CENTITY
#define _CENTITY

#include "MetroidPrime/CEntityInfo.hpp"
#include "MetroidPrime/CValidEntityPredicate.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

class CStateManager;

// Layout from the constructor (0x80032B20) and destructor (0x80032A94). The vtable
// (lbl_806B2210) has six entries, in the same order as Echoes.
//
// Compared to Echoes, the prototype entity also keeps its name (0x10), two lists of unique ids
// that order thinking between entities (0x20, 0x30; see AcceptScriptMsg) and a word copied from
// CEntityInfo (0x58). The connection list is searched with -1 wildcards for both the state and
// the message, and a family of helpers fixes the message to kSM_Attach.
class CEntity {
public:
  CEntity(TUniqueId uid, const CEntityInfo& info, const rstl::string& name, uint castFlags);

  virtual ~CEntity();
  virtual CEntity* TypesMatch(int typeId) const;
  virtual void PreThink(float dt, CStateManager& mgr);
  virtual void Think(float dt, CStateManager& mgr);
  virtual void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);
  virtual void SetActive(bool active);

  void SendScriptMsgs(EScriptObjectState state, CStateManager& mgr,
                      const SScriptMsgOriginator& originator, EScriptObjectMessage skipMsg);
  TAreaId GetAreaIdForPersistence() const;

  // Echoes' connection queries.
  TUniqueId FindConnectedObject(const CStateManager& mgr, EScriptObjectState state,
                                EScriptObjectMessage msg) const;
  TUniqueId FindConnectedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                   EScriptObjectMessage msg,
                                   const CValidEntityPredicate& predicate) const;
  rstl::vector< TUniqueId > FindConnectedObjects(const CStateManager& mgr,
                                                 EScriptObjectState state,
                                                 EScriptObjectMessage msg) const;
  rstl::vector< TUniqueId > FindConnectedObjects_if(const CStateManager& mgr,
                                                    EScriptObjectState state,
                                                    EScriptObjectMessage msg,
                                                    const CValidEntityPredicate& predicate) const;
  TUniqueId CheckConnectedObject(const CStateManager& mgr, EScriptObjectState state,
                                 EScriptObjectMessage msg) const;
  TUniqueId CheckConnectedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                    EScriptObjectMessage msg,
                                    const CValidEntityPredicate& predicate) const;

  // Guessed names. The same queries with the message fixed to kSM_Attach.
  SScriptObjectRef GetAttachedObjectRef(EScriptObjectState state) const;
  TUniqueId FindAttachedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                  const CValidEntityPredicate& predicate) const;
  rstl::vector< TUniqueId > FindAttachedObjects(const CStateManager& mgr,
                                                EScriptObjectState state) const;
  TUniqueId CheckAttachedObject_if(const CStateManager& mgr, EScriptObjectState state,
                                   const CValidEntityPredicate& predicate) const;
  TUniqueId FindAttachedObject(const CStateManager& mgr, EScriptObjectState state) const;
  TUniqueId CheckAttachedObject(const CStateManager& mgr, EScriptObjectState state) const;
  static TUniqueId CheckAttachedObject(TUniqueId uid, const CStateManager& mgr,
                                      EScriptObjectState state);

  // Guessed name. Points the connections between freshly generated objects at each other.
  static void ResolveInstanceConnections(const rstl::vector< TUniqueId >& uids, TUniqueId owner,
                                         CStateManager& mgr);

  // Guessed names. Maintain the think-order lists at 0x20 and 0x30.
  bool RemoveThinkAfter(CStateManager& mgr, TUniqueId uid);
  void AddThinkAfter(CStateManager& mgr, TUniqueId uid);
  void ClearThinkBefore(CStateManager& mgr);
  void AddThinkBefore(CStateManager& mgr, TUniqueId uid);

  TAreaId GetCurrentAreaId() const { return mAreaId; }
  TUniqueId GetUniqueId() const { return mUniqueId; }
  TEditorId GetEditorId() const { return mEditorId; }
  const rstl::string& GetName() const { return mName; }
  const rstl::vector< SConnection >& GetConnectionList() const { return mConnections; }
  bool GetActive() const { return mActive; }
  uint GetCastFlags() const { return mCastFlags; }
  // The script message logs (CStateManagerObject.cpp) print it next to the name, as 0x%x.
  TEditorId GetX58() const { return x58_; }

  static rstl::vector< SConnection > NullConnectionList;
  static CEntityInfo NullEntityInfo;

private:
  TAreaId mAreaId;
  TUniqueId mUniqueId;
  TEditorId mEditorId;
  rstl::string mName;
  // Objects that think after this one: kSS_ThinkAfter connections and the objects whose
  // kSS_ThinkBefore connections target this one. CStateManager removes ids (0x80030EDC).
  rstl::vector< TUniqueId > x20_;
  // Objects this one thinks after, from its own kSS_ThinkBefore connections.
  rstl::vector< TUniqueId > x30_;
  int x40_; // Never initialized by the constructor.
  rstl::vector< SConnection > mConnections;
  uint mActive : 1;
  uint mNotInArea : 1; // Echoes' name; set when the area id is kInvalidAreaId.
  uint mCastFlags : 6;
  // Echoes names these UpdateWhileOccluded and UpdateDuringCinematicSkip.
  uint x54_8_ : 1;
  uint x54_9_ : 1;
  uint x54_10_ : 1; // Set whenever x20_ changes.
  TEditorId x58_;   // CEntityInfo's last word.
};
CHECK_SIZEOF(CEntity, 0x5C)

#endif // _CENTITY
