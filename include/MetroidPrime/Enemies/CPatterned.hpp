#ifndef _CPATTERNED
#define _CPATTERNED

#include "types.h"

#include "MetroidPrime/Enemies/CAi.hpp"

// Minimal declaration. The constructor (0x800754F4, vtable 0x806B2D44) builds the CAi base and
// starts its own members at 0x3B0 (flags), 0x3B8 (from the patterned info) and on to at least
// 0x8C8; the destructor is 0x8006E178.
class CPatterned : public CAi {
public:
  ~CPatterned();
};

#endif // _CPATTERNED
