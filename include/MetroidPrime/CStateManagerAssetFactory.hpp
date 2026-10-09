#ifndef _CSTATEMANAGERASSETFACTORY
#define _CSTATEMANAGERASSETFACTORY

#include "types.h"

// Minimal declaration. Guessed name, after Factories/CStateManagerAssetFactory.cpp, which holds
// its constructor (0x802FEC54). CStateManager news one (0x40 bytes) and deletes it through its
// first virtual function, the destructor.
class CStateManagerAssetFactory {
public:
  virtual ~CStateManagerAssetFactory();
};

#endif // _CSTATEMANAGERASSETFACTORY
