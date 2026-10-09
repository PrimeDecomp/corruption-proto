#ifndef _CGAMEAREA
#define _CGAMEAREA

#include "types.h"

#include "MetroidPrime/IGameArea.hpp"

#include "Kyoto/Math/CTransform4f.hpp"

#include "rstl/single_ptr.hpp"

// Minimal declarations.
class CGamePortalArea;
class CObjectList;
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
  // Guessed name. 0x80053624 reads the area's world lights again from the host
  // ("c:\FIO\FRelight.game_lights"); the console's RELOADAREALIGHTS calls it on every live area.
  void ReloadLights();

  // Echoes' names.
  enum EOcclusionState {
    kOS_Occluded,
    kOS_Visible,
  };
  // Echoes' names. Only the portal area, the object list and the occlusion state and time are
  // modelled; CStateManager::Think, CAi and CStateManagerCollision skip the objects of an area
  // occluded for more than five seconds.
  struct CPostConstructed {
    uchar x0_[0x134];
    rstl::single_ptr< CGamePortalArea > mPortalArea;
    rstl::single_ptr< CObjectList > mAreaObjectList; // Echoes' CAreaObjectList
    uchar x13c_[0x17C - 0x13C];
    EOcclusionState mOcclusionState;
    uchar x180_[0x184 - 0x180];
    float mOccludedTime;
  };
  // Echoes' names; the prototype has one load phase fewer than Echoes (kP_FinishScriptObjects
  // and kP_Loaded are 13 and 16 there).
  enum EPhase {
    kP_FinishScriptObjects = 12,
    kP_Loaded = 15,
  };
  EPhase GetPhase() const { return mPhase; } // Echoes' name
  bool IsLoaded() const { return mPhase == kP_Loaded; }
  const CPostConstructed* GetPostConstructed() const { return mPostConstructed.get(); }
  CPostConstructed* PostConstructed() { return mPostConstructed.get(); } // Guessed name
  // Echoes' names.
  const CObjectList* GetObjectList() const { return GetPostConstructed()->mAreaObjectList.get(); }
  CObjectList* ObjectList() { return PostConstructed()->mAreaObjectList.get(); }
  EOcclusionState GetOcclusionState() const {
    return IsLoaded() ? mPostConstructed->mOcclusionState : kOS_Occluded;
  }
  TAreaId GetId() const { return mSelfIdx; } // Echoes' name
  // Prime's name. The console's transform commands read it inline.
  const CTransform4f& GetTransform() const { return mTransform; }

private:
  TAreaId mSelfIdx; // Echoes' name
  uchar x8_[0x10 - 0x8];
  CTransform4f mTransform;
  uchar x40_[0xA8 - 0x40];
  EPhase mPhase;
  CGameArea* mNext; // Echoes' name
  uchar xb0_[0xB8 - 0xB0];
  rstl::single_ptr< CPostConstructed > mPostConstructed;
};

#endif // _CGAMEAREA
