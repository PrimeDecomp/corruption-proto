#ifndef _CMEMORYCARD
#define _CMEMORYCARD

#include "types.h"

// Minimal view of the game's memory card front end (Echoes' names; the prototype's functions
// sit in the CUniverseInfo.cpp split). 0x50 bytes.
class CMemoryCard {
public:
  CMemoryCard();  // 0x80199F14
  ~CMemoryCard(); // 0x80199AC0
  // 0x801988F0: true once the card's save info is ready.
  bool InitializePump();

private:
  uchar x0_[0x50];
};
CHECK_SIZEOF(CMemoryCard, 0x50)

extern CMemoryCard* gpMemoryCard;

#endif // _CMEMORYCARD
