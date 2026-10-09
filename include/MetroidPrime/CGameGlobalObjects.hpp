#ifndef _CGAMEGLOBALOBJECTS
#define _CGAMEGLOBALOBJECTS

#include "types.h"

#include "Kyoto/Animation/CCECharacterFactoryBuilder.hpp"
#include "Kyoto/CMemoryCardSys.hpp"
#include "Kyoto/CResFactory.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/TOneStatic.hpp"
#include "Kyoto/TToken.hpp"
#include "MetroidPrime/CDbgDraw.hpp"
#include "MetroidPrime/CGameDebug.hpp"

#include "rstl/optional_object.hpp"
#include "rstl/single_ptr.hpp"

class COsContext;
class CMemorySys;
class CGameState;
class CMemoryCard;
class CStringTable;
class IRenderer;
class CInGameTweakManager;
class IController;

// The prototype keeps CGameGlobalObjects in main.cpp: constructor 0x8000C420, PostInitialize
// 0x8000C358, string-table load 0x8000C218 and the (deleting) destructor 0x80008AF8. It lives in
// TOneStatic storage of 0xA418 bytes.
// Unlike Echoes the object also owns the debug menu (CGameDebug), two debug-draw lists and the
// controller, which Echoes only creates for the duration of AddPaksAndFactories. There is no
// CRELFileManager.
class CGameGlobalObjects : public TOneStatic< CGameGlobalObjects > {
public:
  CGameGlobalObjects(COsContext& context, CMemorySys& memorySys);
  ~CGameGlobalObjects();

  void PostInitialize(COsContext& context, CMemorySys& memorySys);
  void AddPaksAndFactories(COsContext& context); // Echoes' name; 0x8000A8DC
  void LoadStringTable();

  // Echoes' names.
  rstl::single_ptr< CGameState >& GameState() { return mGameState; }
  rstl::single_ptr< CMemoryCard >& MemoryCard() { return mMemoryCard; }

private:
  // Echoes' names, except where noted.
  CMemoryCardSys mMemoryCardSys;
  CResFactory mResFactory;                             // gpResourceFactory
  CSimplePool mSimplePool;                             // gpSimplePool
  CCECharacterFactoryBuilder mCharacterFactoryBuilder; // gpCharacterFactoryBuilder
  CGameDebug mGameDebug;                               // Guessed name; gpGameDebug
  CDbgDraw mDbgDraw;                                   // Guessed name; gpDbgDraw
  CDbgDraw mPersistentDbgDraw; // Guessed name; never ages its primitives (gpPersistentDbgDraw)
  rstl::single_ptr< CGameState > mGameState;                          // published as gpGameState
  rstl::single_ptr< CMemoryCard > mMemoryCard;                        // gpMemoryCard once ready
  rstl::optional_object< TLockedToken< CStringTable > > mStringTable; // gpStringTable
  rstl::single_ptr< IRenderer > mRenderer;                            // gpRender
  rstl::single_ptr< CInGameTweakManager > mInGameTweakManager;        // gpTweakManager
  rstl::single_ptr< IController > mController;                        // Guessed name; gpController
};
CHECK_SIZEOF(CGameGlobalObjects, 0xa418)

#endif // _CGAMEGLOBALOBJECTS
