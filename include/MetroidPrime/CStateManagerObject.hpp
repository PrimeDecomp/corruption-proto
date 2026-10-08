#ifndef _CSTATEMANAGEROBJECT
#define _CSTATEMANAGEROBJECT

#include "MetroidPrime/CObjectList.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/bit_vector.hpp"
#include "rstl/rc_ptr.hpp"
#include "rstl/reserved_vector.hpp"

class CEntity;
class CStateManager;

// Guessed name: the repo's inferred split name for the unit whose methods (0x80308934, which
// inserts and notifies, and 0x80308C04, which looks up) receive the pointer held at 0x1118.
class CStringPropertyManager;

// Guessed name, after CStateManagerObject.cpp, the unit that holds this constructor (0x8029A72C)
// and AllocateUniqueId's assert. CStateManager allocates it (0x1138 bytes) and keeps it at 0x4,
// the same way it keeps CStateManagerCallbackLists.cpp's object at 0x0 and
// CStateManagerCollision.cpp's object at 0x8. It owns the object lists and unique-id allocation
// that Echoes kept on CStateManager itself.
class CStateManagerObject {
public:
  TUniqueId AllocateUniqueId();

  // Names from Echoes' CStateManager, whose ObjectById is also emitted first. ObjectById reads the
  // first list directly; GetObjectById goes through the const overload.
  CEntity* ObjectById(TUniqueId uid);
  const CEntity* GetObjectById(TUniqueId uid) const;

  // Guessed names. The const getter's result feeds the lookup (0x80308C04) and the mutable one's
  // feeds the insert (0x80308934); the getter at 0x80298190 is emitted before 0x80298198.
  const CStringPropertyManager* GetStringPropertyManager() const;
  CStringPropertyManager* StringPropertyManager();

private:
  const CObjectList& GetAllObjectList() const { return *mObjectLists[0]; } // Guessed name

  CStateManager* mStateMgr;
  ushort mLastUniqueId; // Named by the "mLastUniqueId != startId" assert.
  // Echoes' name. The constructor fills all 0x800 slots with zero.
  rstl::reserved_vector< ushort, 2048 > mObjectIndexArray;
  // The constructor creates nine lists; index 0 (at 0x1014) is the list every lookup uses.
  rstl::reserved_vector< rstl::auto_ptr< CObjectList >, 9 > mObjectLists;
  uchar x1058_[0x10C4 - 0x1058];
  rstl::bit_vector<> mAllocatedObjectIndices; // Echoes' name; built as (0x800, false).
  uchar x10d8_[0x1118 - 0x10D8];
  rstl::ncrc_ptr< CStringPropertyManager > mStringPropertyManager;
  // Two more reference-counted pointers copied from the constructor's last two arguments.
  uchar x1120_[0x1130 - 0x1120];
  void* x1130_;
  bool x1134_24_ : 1;
};
CHECK_SIZEOF(CStateManagerObject, 0x1138)

#endif // _CSTATEMANAGEROBJECT
