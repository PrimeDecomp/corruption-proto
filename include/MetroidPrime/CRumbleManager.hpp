#ifndef _CRUMBLEMANAGER
#define _CRUMBLEMANAGER

#include "types.h"

#include "Kyoto/Input/CRumbleGenerator.hpp"
#include "Kyoto/Input/CRumbleVoice.hpp"

class CStateManager;
class CVector3f;

// Minimal view (CRumbleManager.cpp). CStateManager news one (0x48 bytes) for the controller
// port. Unlike Echoes there is no player index: the constructor (0x8018A088) stores the port and
// hands it to the generator.
class CRumbleManager {
public:
  explicit CRumbleManager(EIOPort port);

  // Echoes' names and bodies, without the player index: the first one (0x8018A038) checks the
  // single rumble option instead of a per-player one.
  short Rumble(CStateManager& mgr, ERumbleFxId fx, float gain, ERumblePriority priority);
  short Rumble(CStateManager& mgr, const CVector3f& pos, ERumbleFxId fx, float dist,
               ERumblePriority priority);
  void StopRumble(short id);
  void Update(float dt);

  // Echoes' name; CStateManager's destructor inlines it.
  void HardStopAll() { mRumbleGenerator.HardStopAll(); }

private:
  EIOPort mPort; // Echoes' name
  CRumbleGenerator mRumbleGenerator;
};
CHECK_SIZEOF(CRumbleManager, 0x48)

#endif // _CRUMBLEMANAGER
