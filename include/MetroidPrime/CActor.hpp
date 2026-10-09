#ifndef _CACTOR
#define _CACTOR

#include "MetroidPrime/ActorCommon.hpp"
#include "MetroidPrime/CEntity.hpp"
#include "MetroidPrime/CVisorParameters.hpp"

#include "Collision/CMaterialFilter.hpp"
#include "Collision/CMaterialList.hpp"
#include "Kyoto/Audio/CAudioHandle.hpp"
#include "Kyoto/CAssetId.hpp"
#include "Kyoto/Graphics/CColor.hpp"
#include "Kyoto/Math/CAABox.hpp"
#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CVector3f.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/optional_object.hpp"
#include "rstl/reserved_vector.hpp"

class CDamageInfo;
class CDamageVulnerability;
class CHealthInfo;
class CWeaponMode;

// Guessed name. The objects owned at 0xE8 and 0xF0 are signal connections built from a member
// delegate (AcceptScriptMsg fills 0xE8 on 'XCRT'); both are deleted through their vtables.
class CActorSignalConnection {
public:
  virtual ~CActorSignalConnection();
};

// Layout from the constructor (0x80036BAC) and destructor (0x80036AE0); vtable lbl_806B22A8.
// Compared with Echoes, the prototype's CActor has no model data, actor lights, shadow, scan
// info, sounds or render bounds: it keeps only the transform, materials, visor parameters, the
// fluid lists and a handful of flags (0xF8 bytes against Echoes' 0x158).
class CActor : public CEntity {
public:
  CActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info, uint castFlags,
         const CTransform4f& xf, const CMaterialList& materialList,
         const CVisorParameters& visorParams);

  // CEntity
  ~CActor();
  CEntity* TypesMatch(int typeId) const; // Emitted in TypesMatch.cpp.
  void Think(float dt, CStateManager& mgr);
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg);
  void SetActive(bool active);

  // CActor. Slot offsets are from lbl_806B22A8. Echoes names are used where the behavior matches;
  // VirtualNN are placeholders (NN = slot offset) for slots without an identified name.
  virtual void SetTransformDirty();                 // 0x20, a plain method in Echoes
  virtual void ClearFluidList(CStateManager& mgr);  // 0x24
  virtual void Virtual28(CStateManager& mgr);       // 0x28, empty
  virtual CHealthInfo* HealthInfo();                // 0x2C
  virtual const CHealthInfo* GetHealthInfo() const; // 0x30, emitted weak elsewhere
  virtual const CDamageVulnerability* GetDamageVulnerability() const; // 0x34
  virtual const CDamageVulnerability* GetDamageVulnerability(const CVector3f&, const CVector3f&,
                                                             const CDamageInfo&) const; // 0x38
  // Guessed name. 0x3C: sends the Damage or ResistedDamage state, the matching XDMG/XRDG
  // message to the attacker and then a per-weapon damage state.
  virtual void NotifyDamage(CStateManager& mgr, TUniqueId sender, int, const CDamageInfo& info,
                            bool damaged);
  virtual rstl::optional_object< CAABox > GetTouchBounds() const;                   // 0x40
  virtual void Touch(CActor& other, CStateManager& mgr);                            // 0x44
  virtual CVector3f GetOrbitPosition(const CStateManager& mgr) const;               // 0x48
  virtual CVector3f GetAimPosition(const CStateManager& mgr, float dt) const;       // 0x4C
  virtual CVector3f GetHomingPosition(const CStateManager& mgr, float dt) const;    // 0x50
  virtual CVector3f GetScanObjectIndicatorPosition(const CStateManager& mgr) const; // 0x54
  virtual EWeaponCollisionResponseTypes GetCollisionResponseType(const CVector3f&, const CVector3f&,
                                                                 const CWeaponMode&,
                                                                 int) const; // 0x58
  // Guessed name. 0x5C: forwards to a non-virtual helper (0x800369C8) that, while a CGameDebug
  // option is set, draws the touch bounds in GetTouchBoundsColor's color.
  // UpdateTouchBoundsDrawing connects it (pointer to member lbl_806B229C) to a draw signal in 0xF0.
  virtual void DrawTouchBounds();
  virtual void Virtual60();                                    // 0x60, empty
  virtual void Virtual64();                                    // 0x64, weak, returns 0
  virtual void Virtual68();                                    // 0x68, weak, empty
  virtual void Virtual6C();                                    // 0x6C, weak, empty
  virtual CVector3f Virtual70(const CStateManager& mgr) const; // 0x70, weak, GetAimPosition(mgr, 0)
  // Guessed name. 0x74: connects DrawTouchBounds to the draw signal while a CGameDebug option is
  // set and disconnects it otherwise. AcceptScriptMsg subscribes it (pointer to member
  // lbl_806B2290) to CGameDebug option 0xF5 into 0xE8 and then calls it.
  virtual void UpdateTouchBoundsDrawing(CStateManager& mgr);
  // Guessed name. 0x78: only DrawTouchBounds uses this pulsing color.
  virtual CColor GetTouchBoundsColor() const;
  // Guessed name. 0x7C: empty here; called after every material change so subclasses can react.
  virtual void MaterialChanged();

  const CTransform4f& GetTransform() const { return mTransform; }
  void SetTransform(const CTransform4f& xf);
  const CVector3f& GetTranslation() const { return mPosition; }
  void SetTranslation(const CVector3f& vec);

  const CMaterialList& GetMaterialList() const { return mMaterial; }
  void AddMaterial(EMaterialTypes mat1, CStateManager& mgr);
  void AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr);
  void AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                   CStateManager& mgr);
  void AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                   EMaterialTypes mat4, CStateManager& mgr);
  void AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                   EMaterialTypes mat4, EMaterialTypes mat5, CStateManager& mgr);
  void RemoveMaterial(EMaterialTypes mat1, CStateManager& mgr);
  void RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr);
  void RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                      CStateManager& mgr);
  void RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                      EMaterialTypes mat4, CStateManager& mgr);

  const CMaterialFilter& GetMaterialFilter() const;
  void SetMaterialFilter(const CMaterialFilter& filter);

  bool GetUseInSortedLists() const;
  void SetUseInSortedLists(bool use);
  bool GetCallTouch() const;
  void SetCallTouch(bool value);
  void SetE4Flag6(bool value); // Guessed name; sets the flag the constructor seeds with 0xF.

  TUniqueId InFluidId() const;
  const rstl::reserved_vector< TUniqueId, 4 >& GetFluidList() const;
  void SetFluidList(const rstl::reserved_vector< TUniqueId, 4 >& fluids);
  void RemoveInvalidFluidIds(CStateManager& mgr);

  // Guessed names. Play a CAUD sound effect at the actor's position, or panned without a
  // position; an invalid asset id gives an invalid handle.
  CAudioHandle PlaySoundEffect(CAssetId id, float volume);
  CAudioHandle PlayPannedSoundEffect(CAssetId id, float volume, float pan);

private:
  // Guessed name. 0x80036478: after NotifyDamage, sends a state that names the weapon type.
  void SendWeaponDamageState(CStateManager& mgr, const CDamageInfo& info, bool damaged);

  CTransform4f mTransform;                         // 0x5C
  CVector3f mPosition;                             // 0x8C, copy of the transform's translation
  CMaterialList mMaterial;                         // 0x98
  CMaterialFilter mMaterialFilter;                 // 0xA0
  CVisorParameters mVisorParameters;               // 0xB8
  rstl::reserved_vector< TUniqueId, 4 > mFluidIds; // 0xBC
  rstl::reserved_vector< TUniqueId, 4 > mPreviousFluidIds; // 0xD0
  bool mFluidIdsChanged : 1;                               // 0xE4
  // Set together by SetTransformDirty; Echoes sets four such flags there.
  uint xe4_1_ : 1;
  uint xe4_2_ : 1;
  uint mUseInSortedLists : 1;
  uint mCallTouch : 1;
  uint xe4_5_ : 1;
  uint xe4_6_ : 1;
  rstl::auto_ptr< CActorSignalConnection > xe8_;
  rstl::auto_ptr< CActorSignalConnection > xf0_;
};
CHECK_SIZEOF(CActor, 0xF8)

#endif // _CACTOR
