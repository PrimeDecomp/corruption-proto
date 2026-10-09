#ifndef _CSORTEDLISTS
#define _CSORTEDLISTS

#include "types.h"

#include "Kyoto/TSignal1.hpp"

#include "rstl/auto_ptr.hpp"

class CStateManager;

namespace SL {

// Minimal view, with Echoes' and Prime's names (CSortedLists.cpp). CStateManagerCollision embeds
// one (0x1C020 bytes) at 0x8, where Echoes kept it in the state manager's container. The node
// array and the six sorted lists are sized for 2048 objects (0x2C-byte nodes, 0x1004-byte lists).
class CSortedListManager {
public:
  // 0x8010CA08. Unlike Echoes, it takes the state manager: it subscribes to the
  // CStateManagerCallbackLists object-removed signal, then resets its lists.
  explicit CSortedListManager(CStateManager& mgr);

private:
  uchar x0_[0x1C018];
  // Guessed name. The only member the implicit destructor (inlined into CStateManagerCollision's)
  // has to destroy.
  rstl::auto_ptr< IConnection > mObjectRemovedConnection;
};
CHECK_SIZEOF(CSortedListManager, 0x1C020)

} // namespace SL

#endif // _CSORTEDLISTS
