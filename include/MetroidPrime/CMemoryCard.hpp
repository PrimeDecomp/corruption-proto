#ifndef _CMEMORYCARD
#define _CMEMORYCARD

#include "types.h"

#include "Kyoto/TToken.hpp"
#include "MetroidPrime/CGameHintInfo.hpp"

// Minimal view of the game's memory card front end (Echoes' names; the prototype's functions
// sit in the CUniverseInfo.cpp split). 0x50 bytes.
class CMemoryCard {
public:
  CMemoryCard();  // 0x80199F14
  ~CMemoryCard(); // 0x80199AC0
  // 0x801988F0: true once the card's save info is ready.
  bool InitializePump();

  // Echoes' accessor. CStateManager::UpdateHintState reads the hint table at 0x0.
  const rstl::vector< CGameHintInfo::CGameHint >& GetHints() const {
    return mHints.GetObject()->GetHints();
  }

private:
  TCachedToken< CGameHintInfo > mHints;
  uchar xc_[0x50 - 0xc];
};
CHECK_SIZEOF(CMemoryCard, 0x50)

extern CMemoryCard* gpMemoryCard;

#endif // _CMEMORYCARD
