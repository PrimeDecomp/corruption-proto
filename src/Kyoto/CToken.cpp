#include "Kyoto/CToken.hpp"

#include "Kyoto/Alloc/CMemory.hpp"
#include "rstl/auto_ptr.hpp"

CToken::CToken(CObjectReference* reference) : mReference(reference), mLockHeld(false) {
  mReference->AddReference();
}

CToken::CToken(IObj* object) {
  mReference = new ("CToken.cpp(36) : ", 0) CObjectReference(rstl::auto_ptr< IObj >(object));
  mLockHeld = false;
  mReference->AddReference();
  Lock();
}

CToken::CToken(const CToken& other) : mReference(other.mReference), mLockHeld(false) {
  mReference->AddReference();
  if (other.mLockHeld) {
    Lock();
  }
}

CToken::~CToken() {
  if (mLockHeld) {
    mReference->Unlock();
  }
  RemoveRef();
}

CObjOwnerDerivedFromIObjUntyped* CToken::GetObj() {
  Lock();
  return reinterpret_cast< CObjOwnerDerivedFromIObjUntyped* >(mReference->GetObject());
}

void CToken::RemoveRef() {
  if (mReference->RemoveReference() == 0) {
    delete mReference;
  }
}

void CToken::Lock() {
  if (!mLockHeld) {
    mReference->Lock();
    mLockHeld = true;
  }
}

void CToken::Unlock() {
  if (mLockHeld) {
    mReference->Unlock();
    mLockHeld = false;
  }
}

CToken& CToken::operator=(const CToken& other) {
  if (&other == this) {
    return *this;
  }
  Unlock();
  RemoveRef();
  mReference = other.mReference;
  mReference->AddReference();
  if (other.mLockHeld) {
    Lock();
  }
  return *this;
}
