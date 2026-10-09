#ifndef _CAUDIOHANDLE
#define _CAUDIOHANDLE

#include "types.h"

class CAudioHandle {
public:
  CAudioHandle(); // 0x80568720, out of line
  CAudioHandle(uint value);

  uint GetIndex() const { return mID & 0xFFF; }
  static CAudioHandle NullHandle() { return CAudioHandle(); }
  void operator=(const CAudioHandle& other) { mID = other.mID; }
  bool operator==(const CAudioHandle& other) const { return mID == other.mID; }
  bool operator!=(const CAudioHandle& other) const { return mID != other.mID; }
  operator bool() const { return mID != 0; }
  void Clear() { mID = 0; }

private:
  uint mID;
  static uint mRefCount;
};
CHECK_SIZEOF(CAudioHandle, 0x4)

#endif // _CAUDIOHANDLE
