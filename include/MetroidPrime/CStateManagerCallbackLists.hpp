#ifndef _CSTATEMANAGERCALLBACKLISTS
#define _CSTATEMANAGERCALLBACKLISTS

#include "types.h"

#include "Kyoto/TSignal2.hpp"

class CEntity;
class CStateManager;

// Guessed name, after CStateManagerCallbackLists.cpp. CStateManager allocates it (0x5E8 bytes)
// and keeps it at 0x0. It is 63 signals at a stride of 0x18 (each an rstl::list of slots);
// the constructor (0x8030008C) builds all of them and the destructor (0x802FFB1C) tears them
// down in reverse order through five per-type cleanup instances: 46 lists from 0x0, 4 from
// 0x450, 8 from 0x4B0, the 3 below, and 2 from 0x5B8 (whose cleanup is emitted in main.cpp).
// Only the three that CStateManagerObject fires have known argument types; the rest are
// placeholders.
class CStateManagerCallbackLists {
public:
  typedef TSignal2< CStateManager&, CEntity& > TEntitySignal;

  // Guessed names. AddObject (0x80299E60) fires the first after kSM_Create, RemoveObject
  // (0x80299B18) the second before unlisting, and the message pump (0x80298594) the third when
  // a message flips the target's active flag.
  TEntitySignal& ObjectAdded() { return mObjectAdded; }
  TEntitySignal& ObjectRemoved() { return mObjectRemoved; }
  TEntitySignal& ActiveChanged() { return mActiveChanged; }

private:
  uchar x0_[0x570];
  TEntitySignal mObjectAdded;
  TEntitySignal mObjectRemoved;
  TEntitySignal mActiveChanged;
  uchar x5b8_[0x5E8 - 0x5B8];
};
CHECK_SIZEOF(CStateManagerCallbackLists, 0x5E8)

#endif // _CSTATEMANAGERCALLBACKLISTS
