#ifndef _CENVIRONMENTVARIABLE
#define _CENVIRONMENTVARIABLE

#include "types.h"

// Guessed name (Echoes' guess). Minimal view: a named integer kept between its minimum and
// maximum; the prototype's functions live in CGameState.cpp.
class CEnvironmentVariable {
public:
  int GetValue() const { return mValue; }
  // Echoes' name. 0x8015E8B0: stores the value and clamps it (0x8015E86C).
  void Set(int value);

private:
  int mMin;
  int mMax;
  int mValue;
};
CHECK_SIZEOF(CEnvironmentVariable, 0xc)

#endif // _CENVIRONMENTVARIABLE
