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
  int GetItemAmount(EItemType type, bool respectFieldToQuery = true) const; // Echoes; 0x80088830
  void AddPowerUp(EItemType type, int delta);  // Echoes name; 0x80088B34, clamps the capacity
  void IncrPickUp(EItemType type, int amount); // Echoes name; 0x8008898C
};

#endif // _CPLAYERSTATE
