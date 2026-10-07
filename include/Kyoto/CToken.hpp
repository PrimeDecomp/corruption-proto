#ifndef _CTOKEN
#define _CTOKEN

#include "types.h"

#include "Kyoto/CObjectReference.hpp"
#include "Kyoto/IObj.hpp"

class CObjectReference;

class CToken {
public:
  CToken() {}
  CToken(CObjectReference* ref);
  CToken(IObj* obj); // : x0_objRef(new CObjectReference(obj)), x4_lockHeld(false) {}
  CToken(const CToken& other);
  ~CToken();

  CObjOwnerDerivedFromIObjUntyped* GetObj();
  void Lock();
  void Unlock();
  bool IsLoaded() const { return mReference->IsLoaded(); }
  void RemoveRef();
  CToken& operator=(const CToken&);
  bool HasReference() const { return mReference != nullptr; }
  const SObjectTag& GetTag() const { return mReference->GetTag(); }
  FourCC GetReferenceType() { return GetTag().type; }

  bool HasLock() { return mLockHeld; }

  const CObjectReference* GetRef() const { return mReference; }

private:
  CObjectReference* mReference;
  bool mLockHeld;
};

CHECK_SIZEOF(CToken, 8)

#endif // _CTOKEN
