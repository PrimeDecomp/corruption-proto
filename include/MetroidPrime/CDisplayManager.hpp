#ifndef _CDISPLAYMANAGER
#define _CDISPLAYMANAGER

#include "types.h"

// Minimal declaration. Guessed name, after CDisplayManager.cpp, which holds its destructor
// (0x802A422C). CStateManager owns it at 0x14 and deletes it in its destructor.
class CDisplayManager {
public:
  ~CDisplayManager();
};

#endif // _CDISPLAYMANAGER
