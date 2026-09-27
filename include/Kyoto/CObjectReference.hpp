#ifndef KYOTO_COBJECTREFERENCE_HPP
#define KYOTO_COBJECTREFERENCE_HPP

#include "types.h"
#include "rstl/auto_ptr.hpp"

class IObj;

class CObjectReference {
public:
  CObjectReference(const rstl::auto_ptr<IObj>& object);
  ~CObjectReference();

  void AddReference();
  int RemoveReference();
  void Lock();
  void Unlock();
  IObj* GetObject();

private:
  char mData[0x30];
};
CHECK_SIZEOF(CObjectReference, 0x30)

#endif
