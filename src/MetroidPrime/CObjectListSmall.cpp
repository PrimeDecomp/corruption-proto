// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80261CD0..0x80262A40 (31 native functions).
// Source identity: the "CObjectListSmall.cpp(41) : " assert.
// The five subclasses (vtables 0x806B9CF8..0x806B9D38; GameObjectLists.hpp) follow the base
// class. Each one's IsQualified casts the entity through a non-null TypesMatch.cpp cast; the
// classes come from the TypesMatch override their type id selects. CStateManagerObject builds them
// in the order dock, door, 0x6A, camera, grapple point.
// Listed below are the functions not implemented yet.
// 0x80261E10 +0x30: CType106ListSmall::IsQualified (cast 0x801D6F78, type 0x6A, whose class no
//   TypesMatch override in the DOL names)
// 0x802628F0 +0x60: CType106ListSmall's implicit destructor (emitted with its vtable, once
//   IsQualified is defined)

#include "MetroidPrime/CObjectListSmall.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "MetroidPrime/CEntity.hpp"
#include "MetroidPrime/GameObjectLists.hpp"
#include "MetroidPrime/TCastTo.hpp"

#include "rstl/algorithm.hpp"

class CGameCamera;
class CScriptDock;
class CScriptDoor;
class CScriptGrapplePoint;

// 0x8026285C
CObjectListSmall::CObjectListSmall(bool dynamic) : mDynamic(dynamic) {}

// 0x8026277C
CObjectListSmall::~CObjectListSmall() {}

// 0x80262774
bool CObjectListSmall::IsQualified(const CEntity& entity) const { return true; }

// 0x8026265C
bool CObjectListSmall::IsObjectInList(const CEntity* object) const {
  return rstl::find(mList.begin(), mList.end(), object) != mList.end();
}

// 0x80262294
// Names from the assert string.
void CObjectListSmall::AddObject(CEntity& object) {
  // clang-format off: the spacing is part of the stringized assert condition.
  RS_VERIFY_THROW(41, IsObjectInList( &object ) == NULL, false,
                  "Object in list already when being added");
  // clang-format on
  if (IsQualified(object)) {
    mList.insert(&object);
  }
}

// 0x8026214C
void CObjectListSmall::RemoveObject(CEntity& entity) {
  TList::iterator it = rstl::find(mList.begin(), mList.end(), &entity);
  if (it != mList.end()) {
    mList.erase(it);
  }
}

// 0x80261F60
void CObjectListSmall::RemoveObject(TUniqueId uid) {
  for (TList::iterator it = mList.begin(); it != mList.end(); ++it) {
    if ((*it)->GetUniqueId() == uid) {
      mList.erase(it);
      return;
    }
  }
}

CDoorListSmall::CDoorListSmall() : CObjectListSmall(false) {}

bool CDoorListSmall::IsQualified(const CEntity& entity) const {
  return TCastToConstPtr< CScriptDoor >(entity) != nullptr;
}

CDockListSmall::CDockListSmall() : CObjectListSmall(false) {}

bool CDockListSmall::IsQualified(const CEntity& entity) const {
  return TCastToConstPtr< CScriptDock >(entity) != nullptr;
}

CType106ListSmall::CType106ListSmall() : CObjectListSmall(false) {}

CGameCameraListSmall::CGameCameraListSmall() : CObjectListSmall(false) {}

bool CGameCameraListSmall::IsQualified(const CEntity& entity) const {
  return TCastToConstPtr< CGameCamera >(entity) != nullptr;
}

CGrapplePointListSmall::CGrapplePointListSmall() : CObjectListSmall(false) {}

bool CGrapplePointListSmall::IsQualified(const CEntity& entity) const {
  return TCastToConstPtr< CScriptGrapplePoint >(entity) != nullptr;
}
