#ifndef _CGAMEGLOBALOBJECTS
#define _CGAMEGLOBALOBJECTS

#include "types.h"

#include "Kyoto/TOneStatic.hpp"

#include "rstl/single_ptr.hpp"

class COsContext;
class CMemorySys;
class CGameState;
class CMemoryCard;

// The prototype keeps CGameGlobalObjects in main.cpp: constructor 0x8000C420, PostInitialize
// 0x8000C358, string-table load 0x8000C218 and the (deleting) destructor 0x80008AF8. It lives in
// TOneStatic storage of 0xA418 bytes. The resource factory sits at 0x8 and the simple pool at
// 0xE0 (gpResourceFactory/gpSimplePool), and 0x130 holds the CGameDebug published at
// 0x8079710C. Only the game state and memory card slots are modelled so far.
class CGameGlobalObjects : public TOneStatic< CGameGlobalObjects > {
public:
  CGameGlobalObjects(COsContext& context, CMemorySys& memorySys);
  ~CGameGlobalObjects();

  void PostInitialize(COsContext& context, CMemorySys& memorySys);

  // Echoes' names.
  rstl::single_ptr< CGameState >& GameState() { return mGameState; }
  rstl::single_ptr< CMemoryCard >& MemoryCard() { return mMemoryCard; }

private:
  uchar x0_[0xa3f0];
  rstl::single_ptr< CGameState > mGameState;   // Echoes' name; published as gpGameState
  rstl::single_ptr< CMemoryCard > mMemoryCard; // Echoes' name; gpMemoryCard once ready
  uchar xa3f8_[0x20];
};
CHECK_SIZEOF(CGameGlobalObjects, 0xa418)

#endif // _CGAMEGLOBALOBJECTS
