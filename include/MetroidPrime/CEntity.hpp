#ifndef _CENTITY
#define _CENTITY

#include "MetroidPrime/CEntityInfo.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

class CStateManager;

// Layout from the constructor (0x80032B20) and destructor (0x80032A94). The vtable
// (lbl_806B2210) has six entries, in the same order as Echoes.
class CEntity {
public:
  CEntity(TUniqueId uid, const CEntityInfo& info, const rstl::string& name, uint castFlags);

  virtual ~CEntity();
  virtual CEntity* TypesMatch(int typeId) const;
  virtual void PreThink(float dt, CStateManager& mgr);
  virtual void Think(float dt, CStateManager& mgr);
  virtual void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);
  virtual void SetActive(bool active);

  TAreaId GetCurrentAreaId() const { return mAreaId; }
  TUniqueId GetUniqueId() const { return mUniqueId; }
  TEditorId GetEditorId() const { return mEditorId; }
  const rstl::string& GetName() const { return mName; }
  const rstl::vector< SConnection >& GetConnectionList() const { return mConnections; }
  bool GetActive() const { return mActive; }
  uint GetCastFlags() const { return mCastFlags; }

private:
  TAreaId mAreaId;
  TUniqueId mUniqueId;
  TEditorId mEditorId;
  rstl::string mName;
  rstl::vector< TUniqueId > x20_;
  rstl::vector< TUniqueId > x30_;
  int x40_;
  rstl::vector< SConnection > mConnections;
  uint mActive : 1;
  uint x54_1_ : 1; // Set when the area id equals lbl_80797320.
  uint mCastFlags : 6;
  // Echoes names these UpdateWhileOccluded and UpdateDuringCinematicSkip.
  uint x54_8_ : 1;
  uint x54_9_ : 1;
  uint x54_10_ : 1;
  int x58_;
};
CHECK_SIZEOF(CEntity, 0x5C)

#endif // _CENTITY
