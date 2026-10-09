// G2MEAB prototype NonMatching translation unit.
// Existing configured G2MEAB .text: 0x80034B04..0x80037424 (72 retained native functions).
// Preserve all native methods, emitted helpers and the existing initializer registration.
// Existing source-family name retained; historical helper ownership and inlining remain inferred.
// Current corrected boundaries and complete native inventory are recorded externally.
//
// Deferred inlining emits functions in reverse source order. Not implemented yet:
// 0x80037158 +0x30: static initializer for seven TU-local SDA constants (.ctors 0x8065B500)
// 0x80037188..0x800373C0: token-list helpers (not CActor methods)
// 0x80036D08, 0x80036E58..0x8003705C: rstl::sort over the fluid list with Echoes'
//   CFluidHeightCompare (by-value ids, const GetObjectById, TCastToConstPtr<CScriptWater>), and
//   CScriptWater::GetWRSurfacePlane (0x80036F50, a z-up plane at CScriptWater+0x188). Only
//   SetInFluid instantiates them.
// 0x80036804, 0x800367E4: square-root helpers
// 0x80035880: string copy from +0x88 (not a CActor method)
// 0x8003527C: SetInFluid. Beyond the Echoes logic it prints "'%s' in area '%s' has just
//   entered/exited ..." and "BUG THIS! Fluid list for '%s' is full ..." when CGameDebug's
//   "Show water entry/exit" is above 1.

// SetFluidList and SetInFluid call reserved_vector<TUniqueId, 4>::operator= out of line.
#define RSTL_DONT_INLINE_RESERVED_VECTOR

#include "MetroidPrime/CActor.hpp"

#include "MetroidPrime/CDamageInfo.hpp"
#include "MetroidPrime/CDamageVulnerability.hpp"
#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/CGameDebugDraw.hpp"
#include "MetroidPrime/CRenderManager.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/TCastTo.hpp"

#include "Kyoto/Audio/CAudioSoundEffect.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/Graphics/CGraphics.hpp"
#include "Kyoto/Math/CAbsAngle.hpp"
#include "Kyoto/TToken.hpp"

#include "rstl/algorithm.hpp"

// TypesMatch id 0x5A, whose vtable (lbl_806B3AC8) starts with a function in CScriptWater.cpp.
class CScriptWater;

// As in Echoes, the solid material comes from a variable rather than a constant.
static EMaterialTypes SolidMaterial = kMT_Solid;

CActor::CActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info, uint castFlags,
               const CTransform4f& xf, const CMaterialList& materialList,
               const CVisorParameters& visorParams)
: CEntity(uid, info, name, castFlags | 1)
, mTransform(xf)
, mPosition(xf.GetTranslation())
, mMaterial(materialList)
, mMaterialFilter(
      CMaterialFilter::MakeIncludeExclude(CMaterialList(SolidMaterial), CMaterialList()))
, mVisorParameters(visorParams)
, mFluidIdsChanged(false)
, xe4_1_(true)
, xe4_2_(true)
, mUseInSortedLists(true)
, mCallTouch(true)
, xe4_5_(true)
, xe4_6_(0xF) {}

CActor::~CActor() {}

void CActor::DrawTouchBoundsBox() {
  if (gpGameDebug->GetOptionInt(CGameDebug::kDO_DrawObjectCollisionBoxes) != 0) {
    rstl::optional_object< CAABox > bounds = GetTouchBounds();
    if (bounds) {
      CColor color = GetCollisionBoxColor();
      DrawDebugAABox(bounds.data(), color.GetRed(), color.GetGreen(), color.GetBlue(), 1.f);
    }
  }
}

// Fades between red and green over a ten second cycle.
CColor CActor::GetCollisionBoxColor() const {
  const float t = CMath::ModF(CGraphics::GetSecondsMod900(), 10.f) / 10.f;
  const float green =
      (1.f + static_cast< float >(sin(CAbsAngle::FromDegrees(360.f * t).AsRadians()))) / 2.f;
  return CColor(1.f - green, green, 0.f, 1.f);
}

void CActor::DrawCollisionBoxes(const CStateManager&) { DrawTouchBoundsBox(); }

CHealthInfo* CActor::HealthInfo() { return nullptr; }

const CDamageVulnerability* CActor::GetDamageVulnerability() const {
  return &CDamageVulnerability::NormalVulnerabilty();
}

const CDamageVulnerability* CActor::GetDamageVulnerability(const CVector3f&, const CVector3f&,
                                                           const CDamageInfo&) const {
  return GetDamageVulnerability();
}

void CActor::NotifyDamage(CStateManager& mgr, TUniqueId sender, int, const CDamageInfo& info,
                          bool damaged) {
  if (damaged) {
    SendScriptMsgs(kSS_Damage, mgr, SScriptMsgOriginator(kInvalidUniqueId), kSM_Invalid);
    mgr.ObjectManager().SendScriptMsg(this, sender, kSM_Damage,
                                      SScriptMsgOriginator(kInvalidUniqueId));
  } else {
    SendScriptMsgs(kSS_ResistedDamage, mgr, SScriptMsgOriginator(kInvalidUniqueId), kSM_Invalid);
    mgr.ObjectManager().SendScriptMsg(this, sender, kSM_ResistedDamage,
                                      SScriptMsgOriginator(kInvalidUniqueId));
  }
  SendWeaponDamageState(mgr, info, damaged);
}

// The states are not in CScriptLUA's name table, so they are spelled as FourCCs. Weapon types
// 13 and 14 share 'DBAI'.
void CActor::SendWeaponDamageState(CStateManager& mgr, const CDamageInfo& info, bool damaged) {
  if (!damaged) {
    return;
  }
  int state = kSS_InvalidState;
  switch (info.GetWeaponType()) {
  case 0:
    state = 'DPWR';
    break;
  case 1:
    state = 'DPLS';
    break;
  case 2:
    state = 'DNOV';
    break;
  case 3:
    state = 'DPHZ';
    break;
  case 4:
    state = 'DMIS';
    break;
  case 5:
    state = 'DIMS';
    break;
  case 6:
    state = 'DPMS';
    break;
  case 7:
    state = 'DBMB';
    break;
  case 8:
    state = 'DGUP';
    break;
  case 9:
    state = 'DPGR';
    break;
  case 10:
    state = 'DBAL';
    break;
  case 11:
    state = 'DPZB';
    break;
  case 12:
    state = 'DSCW';
    break;
  case 13:
    state = 'DBAI';
    break;
  case 14:
    state = 'DBAI';
    break;
  case 15:
    state = 'DUNS';
    break;
  case 16:
    state = 'DPWT';
    break;
  case 17:
    state = 'DLAV';
    break;
  case 18:
    state = 'DHOT';
    break;
  case 19:
    state = 'DCLD';
    break;
  }
  if (state != kSS_InvalidState) {
    SendScriptMsgs(static_cast< EScriptObjectState >(state), mgr,
                   SScriptMsgOriginator(kInvalidUniqueId), kSM_Invalid);
  }
}

rstl::optional_object< CAABox > CActor::GetTouchBounds() const {
  return rstl::optional_object_null();
}

void CActor::Touch(CActor&, CStateManager&) {}

bool CActor::GetUseInSortedLists() const { return mUseInSortedLists; }

void CActor::SetUseInSortedLists(bool use) { mUseInSortedLists = use; }

bool CActor::GetCallTouch() const { return mCallTouch; }

void CActor::SetCallTouch(bool value) { mCallTouch = value; }

void CActor::MaterialChanged() {}

void CActor::AddMaterial(EMaterialTypes mat1, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         EMaterialTypes mat4, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mMaterial.Add(mat4);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         EMaterialTypes mat4, EMaterialTypes mat5, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mMaterial.Add(mat4);
  mMaterial.Add(mat5);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                            CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mMaterial.Remove(mat3);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                            EMaterialTypes mat4, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mMaterial.Remove(mat3);
  mMaterial.Remove(mat4);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  MaterialChanged();
}

EWeaponCollisionResponseTypes CActor::GetCollisionResponseType(const CVector3f&, const CVector3f&,
                                                               const CWeaponMode&, int) const {
  return kWCR_OtherProjectile;
}

CVector3f CActor::GetOrbitPosition(const CStateManager&) const { return mPosition; }

CVector3f CActor::GetAimPosition(const CStateManager&, float) const { return mPosition; }

CVector3f CActor::GetHomingPosition(const CStateManager& mgr, float dt) const {
  return GetAimPosition(mgr, dt);
}

CVector3f CActor::GetScanObjectIndicatorPosition(const CStateManager& mgr) const {
  return GetOrbitPosition(mgr);
}

const CMaterialFilter& CActor::GetMaterialFilter() const { return mMaterialFilter; }

void CActor::SetMaterialFilter(const CMaterialFilter& filter) { mMaterialFilter = filter; }

// Unlike Echoes, the transform is always marked dirty, not only when the active state changes.
void CActor::SetActive(bool active) {
  SetTransformDirty();
  CEntity::SetActive(active);
}

void CActor::AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) {
  CEntity::AcceptScriptMsg(mgr, msg);
  if (msg.GetMessage() == kSM_Create) {
    mCollisionBoxOptionConnection =
        gpGameDebug->ConnectOption(CGameDebug::kDO_DrawObjectCollisionBoxes,
                                   TFunctor1FromMethod< CActor, CStateManager& >::Make(
                                       *this, &CActor::UpdateCollisionBoxDrawing));
    UpdateCollisionBoxDrawing(mgr);
  }
}

void CActor::Virtual28(CStateManager&) {}

TUniqueId CActor::InFluidId() const {
  if (mFluidIds.empty()) {
    return kInvalidUniqueId;
  }
  return mFluidIds.back();
}

const rstl::reserved_vector< TUniqueId, 4 >& CActor::GetFluidList() const { return mFluidIds; }

void CActor::SetFluidList(const rstl::reserved_vector< TUniqueId, 4 >& fluids) {
  mFluidIds = fluids;
  mPreviousFluidIds = mFluidIds;
  mFluidIdsChanged = false;
}

void CActor::ClearFluidList(CStateManager&) {
  mFluidIds.clear();
  mPreviousFluidIds.clear();
  mFluidIdsChanged = false;
}

void CActor::RemoveInvalidFluidIds(CStateManager& mgr) {
  rstl::reserved_vector< TUniqueId, 4 >::iterator it = mFluidIds.begin();
  while (it != mFluidIds.end()) {
    if (!TCastToConstPtr< CScriptWater >(mgr.ObjectManager().GetObjectById(*it))) {
      it = mFluidIds.erase(it);
    } else {
      ++it;
    }
  }
}

void CActor::SetTranslation(const CVector3f& vec) {
  mTransform.SetTranslation(vec);
  mPosition = vec;
  SetTransformDirty();
}

void CActor::SetE4Flag6(bool value) { xe4_6_ = value; }

void CActor::Think(float dt, CStateManager& mgr) {
  mFluidIdsChanged = false;
  CEntity::Think(dt, mgr);
}

void CActor::Virtual60() {}

void CActor::SetTransformDirty() {
  xe4_1_ = true;
  xe4_2_ = true;
}

void CActor::SetTransform(const CTransform4f& xf) {
  mTransform = xf;
  mPosition = xf.GetTranslation();
  SetTransformDirty();
}

void CActor::SetCollisionBoxDrawing(CRenderManager* renderMgr, bool enable) {
  if (enable == (mCollisionBoxDrawConnection.get() != nullptr)) {
    return;
  }
  if (enable) {
    mCollisionBoxDrawConnection =
        renderMgr->ConnectDebugDraw(TFunctor1FromMethod< CActor, const CStateManager& >::Make(
            *this, &CActor::DrawCollisionBoxes));
  } else {
    mCollisionBoxDrawConnection = rstl::auto_ptr< IConnection >();
  }
}

void CActor::UpdateCollisionBoxDrawing(CStateManager& mgr) {
  SetCollisionBoxDrawing(mgr.RenderManager(),
                         gpGameDebug->GetOptionInt(CGameDebug::kDO_DrawObjectCollisionBoxes) != 0);
}

CAudioHandle CActor::PlayPannedSoundEffect(CAssetId id, float volume, float pan) {
  if (id != kInvalidAssetId) {
    TLockedToken< CAudioSoundEffect > sound(gpSimplePool->GetObj(SObjectTag('CAUD', id)));
    return sound->PlayPanned(GetCurrentAreaId().Value(), volume, pan);
  }
  return CAudioHandle();
}

CAudioHandle CActor::PlaySoundEffect(CAssetId id, float volume) {
  if (id != kInvalidAssetId) {
    TLockedToken< CAudioSoundEffect > sound(gpSimplePool->GetObj(SObjectTag('CAUD', id)));
    return sound->PlaySpatial(GetCurrentAreaId().Value(), GetTranslation(), volume);
  }
  return CAudioHandle();
}
