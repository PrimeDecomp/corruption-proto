#ifndef _CVISORPARAMETERS
#define _CVISORPARAMETERS

#include "types.h"

// Weak copies of the constructor and None() are emitted in CScriptTrigger.cpp. The
// constructor asserts "(visorFlags <= kVF_All)" at CVisorParameters.h(85).
class CVisorParameters {
public:
  enum EVisorFlags {
    kVF_All = 0xF,
  };

  CVisorParameters(bool b, uint visorFlags);

  // Echoes has the same all-flags factory under this name. G2MEAB callers do not inline it.
  static CVisorParameters None();

private:
  uint x0_ : 31;
  uint x0_31_ : 1;
};
CHECK_SIZEOF(CVisorParameters, 0x4)

#endif // _CVISORPARAMETERS
