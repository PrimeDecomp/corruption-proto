// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80261CD0..0x80262A40 (31 native functions).
// Source identity: the "CObjectListSmall.cpp(41) : " assert.
// The five subclasses (vtables 0x806B9CF8..0x806B9D38) are not implemented: their names are not
// known. Each one's IsQualified casts the entity to one class through a non-null TypesMatch.cpp
// cast (0x801D753C: type 0x3B, 0x801D7D94: CGameCamera, 0x801D6F78: type 0x6A, 0x801D762C:
// CScriptDock, 0x801D75D8: CScriptDoor). CStateManagerObject builds them in the order dock, door,
// 0x6A, camera, 0x3B, so its small list 3 holds the cameras.
// 0x80261CD0 +0x60: subclass (type 0x3B) destructor
// 0x80261D30 +0x30: its IsQualified
// 0x80261D60 +0x40: its constructor
// 0x80261DA0 +0x30: camera subclass IsQualified
// 0x80261DD0 +0x40: camera subclass constructor
// 0x80261E10 +0x30: subclass (type 0x6A) IsQualified
// 0x80261E40 +0x40: its constructor
// 0x80261E80 +0x30: dock subclass IsQualified
// 0x80261EB0 +0x40: dock subclass constructor
// 0x80261EF0 +0x30: door subclass IsQualified
// 0x80261F20 +0x40: door subclass constructor
// 0x80262890 +0x60: camera subclass destructor
// 0x802628F0 +0x60: subclass (type 0x6A) destructor
// 0x80262950 +0x60: dock subclass destructor
// 0x802629B0 +0x60: door subclass destructor
// 0x80262A10 +0x30: static initializer (.ctors 0x8065B8FC) for seven SDA constants shared through
//   a header (three -1 ids, then 0, 1, 2 and -1)

#include "MetroidPrime/CObjectListSmall.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "MetroidPrime/CEntity.hpp"

#include "rstl/algorithm.hpp"

// 0x8026285C
CObjectListSmall::CObjectListSmall(bool flag) : x20_(flag) {}

// 0x8026277C
CObjectListSmall::~CObjectListSmall() {}

// 0x80262774
bool CObjectListSmall::IsQualified(const CEntity& entity) const { return true; }

// 0x8026265C
bool CObjectListSmall::Contains(const CEntity& entity) const {
  return rstl::find(mList.begin(), mList.end(), &entity) != mList.end();
}

// 0x80262294
void CObjectListSmall::AddObject(CEntity& entity) {
  bool notInList = Contains(entity) == false;
  if (notInList == false) {
    RS_VERIFY_FAILURE(41, "IsObjectInList( &object ) == NULL", "false",
                      "Object in list already when being added");
  }
  if (IsQualified(entity)) {
    mList.insert(&entity);
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
