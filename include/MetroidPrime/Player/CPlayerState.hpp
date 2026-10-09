#ifndef _CPLAYERSTATE
#define _CPLAYERSTATE

#include "types.h"

// Minimal view of the prototype's CPlayerState: only what CGameDebug uses is declared. The item
// numbering differs from Echoes (see CGameDebug::GetPlayerItemForOption) and is not mapped yet.
class CPlayerState {
public:
  enum EItemType {
    kIT_Invalid = -1,
  };

  // Echoes name. 0x800898FC: the capacity column of the 0xC byte item table at 0x80665200.
  static int GetPowerUpMaxValue(EItemType type);
};

#endif // _CPLAYERSTATE
