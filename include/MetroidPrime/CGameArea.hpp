#ifndef _CGAMEAREA
#define _CGAMEAREA

#include "types.h"

#include "MetroidPrime/IGameArea.hpp"

#include "rstl/single_ptr.hpp"

// Minimal declaration.
class CStateManager;

class CGameArea : public IGameArea {
public:
  ~CGameArea();

  // Echoes' name; CStateManager's update calls it on the next area (0x8004E53C).
  void UpdateDocks(CStateManager& mgr);
  // Guessed name. 0x800516BC prints the area's script layers; CStateManager's update calls it
  // once when "Scripting Layers Verbose" is set to 2.
  void DumpScriptLayers(CStateManager& mgr);

  // Echoes' names. Only the occluded time is modelled; CStateManager::Think, CAi and
  // CStateManagerCollision skip the objects of an area occluded for more than five seconds.
  struct CPostConstructed {
    uchar x0_[0x184];
    float mOccludedTime;
  };
  // Echoes' names; the prototype has one load phase fewer than Echoes (kP_Loaded is 16 there).
  enum EPhase {
    kP_Loaded = 15,
  };
  bool IsLoaded() const { return mPhase == kP_Loaded; }
  const CPostConstructed* GetPostConstructed() const { return mPostConstructed.get(); }

private:
  uchar x4_[0xA8 - 0x4];
  EPhase mPhase;
  uchar xac_[0xB8 - 0xAC];
  rstl::single_ptr< CPostConstructed > mPostConstructed;
};

#endif // _CGAMEAREA
