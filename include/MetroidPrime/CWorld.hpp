#ifndef _CWORLD
#define _CWORLD

#include "MetroidPrime/CGameArea.hpp"
#include "MetroidPrime/IWorld.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/vector.hpp"

class CMapWorld;
class CStateManager;

// Minimal: only what CStateManagerObject, CStateManager, CMain and CActor use.
class CWorld : public IWorld {
public:
  // Echoes' names.
  enum EAreaTravelType {
    kATT_LoadAdjacent,
    kATT_SkipAdjacent,
  };
  // Echoes' names. The heads of the area chains are kept at 0x4C (kC_Alive at 0x58).
  enum EChain {
    kC_ToDeallocate,
    kC_Deallocated,
    kC_Loading,
    kC_Alive,
    kC_AliveJudgement,
  };

  virtual ~CWorld();

  // Echoes' names and signatures; CStateManager's update calls them.
  void TravelToArea(const TAreaId& areaId, CStateManager& mgr, EAreaTravelType travelType);
  void Update(float dt); // 0x80037A60
  // Echoes' name and signature (0x80037584): pauses the areas still loading and keeps the flag.
  // CStateManager::SetGameState calls it when entering or leaving the soft pause.
  void SetLoadPauseState(bool paused);
  // 0x80037424. Unlike Echoes' inline getter it asserts "area->IsFullyConstructed()" ("Invalid
  // area passed into GetArea()").
  CGameArea* GetArea(TAreaId id);

  // Echoes' name and signature; reads the map world through the resource at 0x38 (0x80039038).
  CMapWorld* GetMapWorld() const;

  // Guessed name and owner. Clears the static list of locked CTokens (0x8077E108, constructed by
  // CWorld.cpp's static initializer) that 0x80037188 fills from a world's current area on the
  // console's network-asset reload. Called by CMain::ShutdownSubsystems and CGameArea. The
  // configured split places it at the end of CActor.cpp, after that unit's static initializer.
  static void ClearLockedTokens();

  // Echoes' name. CActor::SetInFluid inlines it (areas vector data at 0x2C, 8-byte elements).
  CGameArea* Area(TAreaId id) { return mAreas[id.Value()].get(); }

  // Echoes' names. The end of every chain is a null iterator; CWorld.cpp's static initializer
  // clears it (0x8003B7F8), after skGlobalEnd (0x807973B4).
  CGameArea::CChainIterator ChainHead(EChain chain) const {
    return CGameArea::CChainIterator(mChainHeads[chain]);
  }
  static CGameArea::CChainIterator AliveAreasEnd() { return skGlobalNonConstEnd; }
  // Echoes' name. Not inlined: a weak copy is emitted with CGroundMovement (0x8013A3B4).
  static CGameArea::CChainIterator GetAliveAreasEnd();

private:
  static CGameArea::CChainIterator skGlobalEnd;
  static CGameArea::CChainIterator skGlobalNonConstEnd;

  uchar x4_[0x20 - 0x4];
  rstl::vector< rstl::auto_ptr< CGameArea > > mAreas; // Echoes' name
  uchar x30_[0x48 - 0x30];
  rstl::reserved_vector< CGameArea*, 5 > mChainHeads; // Echoes' name
};

#endif // _CWORLD
