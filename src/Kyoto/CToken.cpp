#include "Kyoto/CToken.hpp"

#include "Kyoto/Alloc/CMemory.hpp"

CToken& CToken::operator=(const CToken& other) {
  if (this != &other) {
    Unlock();
    RemoveRef();
    mReference = other.mReference;
    mReference->AddReference();
    if (other.mLockHeld) {
      Lock();
    }
  }
  return *this;
}

void CToken::Unlock() {
  if (mLockHeld) {
    mReference->Unlock();
    mLockHeld = false;
  }
}

void CToken::Lock() {
  if (!mLockHeld) {
    mReference->Lock();
    mLockHeld = true;
  }
}

void CToken::RemoveRef() {
  if (mReference->RemoveReference() == 0) {
    delete mReference;
  }
}

CObjOwnerDerivedFromIObjUntyped* CToken::GetObj() {
  Lock();
  return reinterpret_cast<CObjOwnerDerivedFromIObjUntyped*>(mReference->GetObject());
}

CToken::~CToken() {
  if (mLockHeld) {
    mReference->Unlock();
  }
  RemoveRef();
}

CToken::CToken(const CToken& other) : mReference(other.mReference), mLockHeld(false) {
  mReference->AddReference();
  if (other.mLockHeld) {
    Lock();
  }
}

CToken::CToken(IObj* object) {
  struct TokenOwner {
    bool owns;
    IObj* pointer;
  } owner = {object != 0, object};
  mReference = new ("CToken.cpp(36) : ", 0)
      CObjectReference(*reinterpret_cast<rstl::auto_ptr<IObj>*>(&owner));
  if (owner.owns && owner.pointer) {
    delete owner.pointer;
  }
  mLockHeld = false;
  mReference->AddReference();
  Lock();
}

CToken::CToken(CObjectReference* reference) : mReference(reference), mLockHeld(false) {
  mReference->AddReference();
}
