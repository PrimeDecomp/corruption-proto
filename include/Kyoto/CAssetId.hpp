#ifndef _CASSETID
#define _CASSETID

#include "types.h"

#include "rstl/construct.hpp"
#include "rstl/string.hpp"

class CInputStream;
class COutputStream;

// G2MEAB asset IDs are 64-bit (IObj.cpp 0x80508574..0x80508A74).
class CAssetId {
public:
  CAssetId() {}
  explicit CAssetId(unsigned long long id) : mId(id) {}
  explicit CAssetId(CInputStream& in); // 0x80508608
  explicit CAssetId(const char* str);  // 0x80508628

  rstl::string ToHexString() const;                // 0x80508574, Guessed name
  void PutTo(COutputStream& out) const;            // 0x805085CC
  unsigned long long Value() const { return mId; } // Guessed name

  // The operand is taken by value: comparisons copy the right-hand id first
  // (CActor::PlaySoundEffect copies kInvalidAssetId; CTransitionDatabaseGame's lower_bound at
  // 0x8055F8E0 compares against a copy of the searched key).
  bool operator==(CAssetId other) const { return mId == other.mId; }
  bool operator!=(CAssetId other) const { return mId != other.mId; }
  bool operator<(CAssetId other) const { return mId < other.mId; }

private:
  static long long ParseDecimalString(const char* str); // 0x80508758, Guessed name

  unsigned long long mId;
};
CHECK_SIZEOF(CAssetId, 0x8)

// Containers of ids are copied flat and freed without destroying their elements
// (CTransitionDatabaseGame's additive table at 0x8055ED04 and 0x8055E1F8, and main.cpp's
// vector<pair<CAssetId, TEditorId> >::clear).
namespace rstl {
RSTL_DECLARE_TRIVIALLY_CONSTRUCTIBLE(CAssetId)
} // namespace rstl

// 0x8079B460: set to -1 by IObj.cpp's static initializer.
extern const CAssetId kInvalidAssetId; // Guessed name

#endif // _CASSETID
