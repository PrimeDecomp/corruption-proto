// NonMatching translation-unit scaffold; no implementation is supplied.
// Existing configured G2MEAB .text: 0x80034B04..0x80037424 (72 retained native functions).
// Preserve all native methods, emitted helpers and the existing initializer registration.
// Existing source-family name retained; historical helper ownership and inlining remain inferred.
// Current corrected boundaries and complete native inventory are recorded externally.
// No declarations, matching claims or compiler-setting conclusions are supplied.
//
// Deferred inlining emits functions in reverse source order. Not implemented yet:
// 0x80037158 +0x30: static initializer for seven TU-local SDA constants (.ctors 0x8065B500)
// 0x80037188..0x800373C0: token-list helpers (not CActor methods)
// 0x80036D08, 0x80036E58..0x8003705C: fluid-list sort and the fluid height compare
// 0x800369C8 / 0x800368E8: Virtual5C, draws the touch bounds
// 0x80036908: Virtual78, pulsing debug color
// 0x80036804, 0x800367E4: square-root helpers
// 0x800367BC: GetDamageVulnerability(), returns the normal vulnerability (0x800E1588)
// 0x80036618 / 0x80036478: Virtual3C, sends the damage states
// 0x80035998 / 0x80035B18: AcceptScriptMsg and its delegate thunk
// 0x80035880: string copy from +0x88 (not a CActor method)
// 0x8003527C / 0x80035208 / 0x8003516C: SetInFluid, reserved_vector erase, RemoveInvalidFluidIds
// 0x80034B04..0x80034FFC: sound playback, Virtual74 and signal-connection helpers

#include "MetroidPrime/CActor.hpp"

#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"

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

CHealthInfo* CActor::HealthInfo() { return nullptr; }

const CDamageVulnerability* CActor::GetDamageVulnerability(const CVector3f&, const CVector3f&,
                                                           const CDamageInfo&) const {
  return GetDamageVulnerability();
}

rstl::optional_object< CAABox > CActor::GetTouchBounds() const {
  return rstl::optional_object_null();
}

void CActor::Touch(CActor&, CStateManager&) {}

bool CActor::GetUseInSortedLists() const { return mUseInSortedLists; }

void CActor::SetUseInSortedLists(bool use) { mUseInSortedLists = use; }

bool CActor::GetCallTouch() const { return mCallTouch; }

void CActor::SetCallTouch(bool value) { mCallTouch = value; }

void CActor::Virtual7C() {}

void CActor::AddMaterial(EMaterialTypes mat1, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         EMaterialTypes mat4, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mMaterial.Add(mat4);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::AddMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                         EMaterialTypes mat4, EMaterialTypes mat5, CStateManager& mgr) {
  mMaterial.Add(mat1);
  mMaterial.Add(mat2);
  mMaterial.Add(mat3);
  mMaterial.Add(mat4);
  mMaterial.Add(mat5);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                            CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mMaterial.Remove(mat3);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
}

void CActor::RemoveMaterial(EMaterialTypes mat1, EMaterialTypes mat2, EMaterialTypes mat3,
                            EMaterialTypes mat4, CStateManager& mgr) {
  mMaterial.Remove(mat1);
  mMaterial.Remove(mat2);
  mMaterial.Remove(mat3);
  mMaterial.Remove(mat4);
  mgr.ObjectManager().UpdateObjectInLists(*this);
  Virtual7C();
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
