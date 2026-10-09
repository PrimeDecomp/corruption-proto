#ifndef _CRUMBLEMANAGER
#define _CRUMBLEMANAGER

#include "types.h"

#include "Kyoto/Input/CRumbleGenerator.hpp"

// Minimal view (CRumbleManager.cpp). CStateManager news one (0x48 bytes) for the controller
// port. Unlike Echoes there is no player index: the constructor (0x8018A088) stores the port and
// hands it to the generator.
class CRumbleManager {
public:
  explicit CRumbleManager(EIOPort port);

  // Echoes' name; CStateManager's destructor inlines it.
  void HardStopAll() { mRumbleGenerator.HardStopAll(); }

private:
  EIOPort mPort; // Echoes' name
  CRumbleGenerator mRumbleGenerator;
};
CHECK_SIZEOF(CRumbleManager, 0x48)

#endif // _CRUMBLEMANAGER
