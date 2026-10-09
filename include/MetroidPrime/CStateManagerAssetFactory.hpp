#ifndef _CSTATEMANAGERASSETFACTORY
#define _CSTATEMANAGERASSETFACTORY

#include "types.h"

class CStateManager;

// Minimal declaration. Guessed name, after Factories/CStateManagerAssetFactory.cpp, which holds
// its constructor (0x802FEC54). CStateManager news one (0x40 bytes) and deletes it through its
// first virtual function, the destructor.
class CStateManagerAssetFactory {
public:
  explicit CStateManagerAssetFactory(CStateManager& mgr);
  virtual ~CStateManagerAssetFactory();

private:
  uchar x4_[0x3C];
};
CHECK_SIZEOF(CStateManagerAssetFactory, 0x40)

#endif // _CSTATEMANAGERASSETFACTORY
