#ifndef _CVALIDENTITYPREDICATE
#define _CVALIDENTITYPREDICATE

#include "MetroidPrime/TGameTypes.hpp"

class CStateManager;

// Echoes' class. The vtable (0x806B2200) holds the destructor and IsValid, both emitted at the
// start of CEntity.cpp's .text.
class CValidEntityPredicate {
public:
  virtual ~CValidEntityPredicate();
  virtual bool IsValid(const CStateManager& mgr, TUniqueId uid) const;
};

#endif // _CVALIDENTITYPREDICATE
