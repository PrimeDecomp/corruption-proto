#ifndef _TGAMETYPES
#define _TGAMETYPES

#include "rstl/construct.hpp"
#include "rstl/pair.hpp"
#include "types.h"

class CInputStream;
class COutputStream;

struct TAreaId {
  int value;

  TAreaId() : value(-1) {}
  TAreaId(int value) : value(value) {}
  int Value() const { return value; }

  bool operator==(const TAreaId& other) const { return value == other.value; }
  bool operator!=(const TAreaId& other) const { return value != other.value; }
};
CHECK_SIZEOF(TAreaId, 0x4)

struct TEditorId {
  uint value;

  TEditorId(uint value) : value(value) {}
  TEditorId(CInputStream& in);
  // TODO
  uint Value() const { return value & 0x3FFFFFF; }
  uint Id() const { return value & 0xffff; }
  int AreaNum() const { return (value >> 16) & 0x3ff; }

  void PutTo(COutputStream&) const;

  bool operator==(const TEditorId& other) const { return Value() == other.Value(); }
  bool operator!=(const TEditorId& other) const { return Value() != other.Value(); }
  bool operator<(const TEditorId& other) const { return Value() < other.Value(); }
};
CHECK_SIZEOF(TEditorId, 0x4)

// G2MEAB stores and copies unique ids as words: CEntity keeps one at 0x8, CScriptMsg packs
// sender/target at 0x0/0x4, and kInvalidUniqueId is a 4-byte -1. Echoes uses a ushort.
// Unsigned: CStateManagerObject::RemoveObject and the SScriptObjectRef lookups compare ids
// with cmplw.
struct TUniqueId {
  uint value;

  explicit TUniqueId(uint packed) : value(packed) {}

  bool operator==(const TUniqueId& other) const { return value == other.value; }
  bool operator!=(const TUniqueId& other) const { return value != other.value; }
  bool operator<(const TUniqueId& other) const { return value < other.value; }
};
CHECK_SIZEOF(TUniqueId, 0x4)

// Every G2MEAB unit that includes this header has its own copies of these constants: its static
// initializer stores -1, -1, -1, 0, 1, 2, -1 into seven consecutive local .sbss words (CEntity:
// 0x80797318..0x80797330), in declaration order. The 0/1/2 area ids are compared against
// CGameArea's area id, index CWorld's area list (CStateManager), and CWorld passes the third as
// the start area of SetWhichMapAreasLoaded where Echoes passes area 0.
static const TEditorId kInvalidEditorId(-1);
static const TUniqueId kInvalidUniqueId(-1);
static const TAreaId kInvalidAreaId(-1);
static const TAreaId kAreaId0(0);  // Guessed name
static const TAreaId kAreaId1(1);  // Guessed name
static const TAreaId kAreaId2(2);  // Guessed name
static const TEditorId kUnkId(-1); // Unreferenced in every G2MEAB unit seen so far

namespace rstl {
RSTL_DECLARE_TRIVIALLY_CONSTRUCTIBLE(TUniqueId)
RSTL_DECLARE_TRIVIALLY_CONSTRUCTIBLE(TEditorId)

// The native pickup, seeker and area-damage containers use the conservative
// element policy for these combinations. Other ID pairs inherit the member traits.
template <>
struct is_trivially_destructible< pair< int, TEditorId > > {
  enum { value = false };
};

template <>
struct use_assignment_for_construction< pair< int, TEditorId > > {
  enum { value = false };
};

template <>
struct is_trivially_destructible< pair< TUniqueId, int > > {
  enum { value = false };
};

template <>
struct use_assignment_for_construction< pair< TUniqueId, int > > {
  enum { value = false };
};

template <>
struct is_trivially_destructible< pair< TUniqueId, float > > {
  enum { value = false };
};

template <>
struct use_assignment_for_construction< pair< TUniqueId, float > > {
  enum { value = false };
};
} // namespace rstl

// struct TGameScriptId {
//   TEditorId editorId;
//   bool b;
// };
// CHECK_SIZEOF(TGameScriptId, 0x8)

typedef ushort TSfxId;
struct TLayerId {
  explicit TLayerId(int value) : mValue(value) {}
  int Value() const { return mValue; }

private:
  int mValue;
};
CHECK_SIZEOF(TLayerId, 0x4)

const TSfxId InvalidSfxId = 0xFFFFu;

#define ALIGN_UP(x, a) (((x) + (a - 1)) & ~(a - 1))

#endif // _TGAMETYPES
