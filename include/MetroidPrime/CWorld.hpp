#ifndef _CWORLD
#define _CWORLD

#include "MetroidPrime/IWorld.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/vector.hpp"

class CGameArea;
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

  virtual ~CWorld();

  // Echoes' names and signatures; CStateManager's update calls them.
  void TravelToArea(const TAreaId& areaId, CStateManager& mgr, EAreaTravelType travelType);
  void Update(float dt); // 0x80037A60
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

private:
  uchar x4_[0x20 - 0x4];
  rstl::vector< rstl::auto_ptr< CGameArea > > mAreas; // Echoes' name
};

#endif // _CWORLD
