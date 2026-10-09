#ifndef _CPLAYERSTATE
#define _CPLAYERSTATE

#include "types.h"

#include "rstl/reserved_vector.hpp"

// Minimal view of the prototype's CPlayerState. The item numbering differs from Echoes (see
// CGameDebug::GetPlayerItemForOption) and is not mapped yet.
class CPlayerState {
public:
  enum EItemType {
    kIT_Invalid = -1,
    // CStateManager's update ticks the timed power-ups of all 0x25 items.
    kIT_Max = 0x25,
  };

  // Echoes' class and member names; 0xC bytes.
  class CPowerUp {
  public:
    int mAmount;
    int mCapacity;
    float mTimeLeft;
  };

  // Echoes' name.
  CPowerUp& PowerUp(EItemType type) { return mPowerUps[type]; }

  // Echoes name. 0x800898FC: the capacity column of the 0xC byte item table at 0x80665200.
  static int GetPowerUpMaxValue(EItemType type);
  int GetItemAmount(EItemType type, bool respectFieldToQuery = true) const; // Echoes; 0x80088830
  void AddPowerUp(EItemType type, int delta);  // Echoes name; 0x80088B34, clamps the capacity
  void IncrPickUp(EItemType type, int amount); // Echoes name; 0x8008898C
  // Echoes' name. The first bit of the byte at 0x0; CStateManager's escape timer checks it.
  bool IsPlayerAlive() const { return mAlive; }

private:
  bool mAlive : 1; // Echoes' name
  uchar x1_[0x5B];
  // Echoes' name. The loader (0x800890DC) pushes all 0x25 entries, counting at 0x5C.
  rstl::reserved_vector< CPowerUp, kIT_Max > mPowerUps;
};

#endif // _CPLAYERSTATE
