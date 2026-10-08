#ifndef _CASSETID
#define _CASSETID

#include "types.h"

#include "Kyoto/Streams/CInputStream.hpp"

// G2MEAB asset IDs are 64-bit (IObj.cpp 0x80508574..0x80508A74). The user-provided
// constructor makes it non-POD: it is returned through a hidden pointer.
class CAssetId {
public:
  CAssetId() {}
  explicit CAssetId(unsigned long long id) : mId(id) {}

  unsigned long long Value() const { return mId; } // Guessed name

  bool operator==(const CAssetId& other) const { return mId == other.mId; }
  bool operator!=(const CAssetId& other) const { return mId != other.mId; }
  bool operator<(const CAssetId& other) const { return mId < other.mId; }

private:
  unsigned long long mId;
};
CHECK_SIZEOF(CAssetId, 0x8)

class COutputStream;

extern "C" void CAssetId_WriteToStream(const CAssetId& id, COutputStream& output);
extern "C" CAssetId CAssetId_ReadFromStream(CInputStream& input);

template <>
inline CAssetId CInputStream::Get< CAssetId >(const TType< CAssetId >& type) {
  return CAssetId_ReadFromStream(*this);
}

// 0x8079B460: set to -1 by IObj.cpp's static initializer.
extern const CAssetId kInvalidAssetId; // Guessed name

#endif // _CASSETID
