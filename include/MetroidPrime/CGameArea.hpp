#ifndef _CGAMEAREA
#define _CGAMEAREA

#include "types.h"

#include "MetroidPrime/IGameArea.hpp"

#include "rstl/single_ptr.hpp"

// Minimal declaration.
class CStateManager;

class CGameArea : public IGameArea {
public:
  // Echoes' iterator over one of the world's area chains, which are linked through mNext. Unlike
  // Echoes' inline increment, the prototype's is emitted on its own (0x803050F0).
  class CChainIterator {
  public:
    CChainIterator() : m_area(nullptr) {}
    explicit CChainIterator(CGameArea* area) : m_area(area) {}
    CGameArea& operator*() const { return *m_area; }
    CGameArea* operator->() const { return m_area; }
    CChainIterator& operator++();
    bool operator!=(const CChainIterator& other) const { return other.m_area != m_area; }

  private:
    CGameArea* m_area;
  };

  ~CGameArea();

  // Echoes' name; CStateManager's update calls it on the next area (0x8004E53C).
  void UpdateDocks(CStateManager& mgr);
  // Guessed name. 0x800516BC prints the area's script layers; CStateManager's update calls it
  // once when "Scripting Layers Verbose" is set to 2.
  void DumpScriptLayers(CStateManager& mgr);
  // Echoes' name (0x80051A60); CStateManager's update calls it on every live area.
  void UpdateDynamicLayers(CStateManager& mgr);

  // Echoes' names.
  enum EOcclusionState {
    kOS_Occluded,
    kOS_Visible,
  };
  // Echoes' names. Only the occlusion state and time are modelled; CStateManager::Think, CAi and
  // CStateManagerCollision skip the objects of an area occluded for more than five seconds.
  struct CPostConstructed {
    uchar x0_[0x17C];
    EOcclusionState mOcclusionState;
    uchar x180_[0x184 - 0x180];
    float mOccludedTime;
  };
  // Echoes' names; the prototype has one load phase fewer than Echoes (kP_Loaded is 16 there).
  enum EPhase {
    kP_Loaded = 15,
  };
  bool IsLoaded() const { return mPhase == kP_Loaded; }
  const CPostConstructed* GetPostConstructed() const { return mPostConstructed.get(); }
  EOcclusionState GetOcclusionState() const {
    return IsLoaded() ? mPostConstructed->mOcclusionState : kOS_Occluded;
  }
  TAreaId GetId() const { return mSelfIdx; } // Echoes' name

private:
  TAreaId mSelfIdx; // Echoes' name
  uchar x8_[0xA8 - 0x8];
  EPhase mPhase;
  CGameArea* mNext; // Echoes' name
  uchar xb0_[0xB8 - 0xB0];
  rstl::single_ptr< CPostConstructed > mPostConstructed;
};

#endif // _CGAMEAREA
