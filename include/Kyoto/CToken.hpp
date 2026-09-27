#ifndef KYOTO_CTOKEN_HPP
#define KYOTO_CTOKEN_HPP

#include "Kyoto/CObjectReference.hpp"
#include "Kyoto/IObj.hpp"

class CToken {
public:
  CToken(CObjectReference* reference);
  CToken(IObj* object);
  CToken(const CToken& other);
  ~CToken();

  CToken& operator=(const CToken& other);
  void Lock();
  void Unlock();
  void RemoveRef();
  CObjOwnerDerivedFromIObjUntyped* GetObj();

private:
  CObjectReference* mReference;
  bool mLockHeld;
};
CHECK_SIZEOF(CToken, 8)

#endif
