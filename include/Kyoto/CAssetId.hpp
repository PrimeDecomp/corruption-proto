#ifndef _CASSETID
#define _CASSETID

#include "types.h"

class CInputStream;
class COutputStream;

// G2MEAB asset IDs are 64-bit (IObj.cpp 0x80508574..0x80508A74).
class CAssetId {
public:
  CAssetId() {}
  explicit CAssetId(unsigned long long id) : mId(id) {}
  explicit CAssetId(CInputStream& in); // 0x80508608

  void PutTo(COutputStream& out) const;            // 0x805085CC
  unsigned long long Value() const { return mId; } // Guessed name

  bool operator==(const CAssetId& other) const { return mId == other.mId; }
  bool operator!=(const CAssetId& other) const { return mId != other.mId; }
  bool operator<(const CAssetId& other) const { return mId < other.mId; }

private:
  unsigned long long mId;
};
CHECK_SIZEOF(CAssetId, 0x8)

// 0x8079B460: set to -1 by IObj.cpp's static initializer.
extern const CAssetId kInvalidAssetId; // Guessed name

#endif // _CASSETID
