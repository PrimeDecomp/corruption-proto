// G2MEAB prototype NonMatching translation unit.
// .text: 0x800EE76C..0x800F0DB8 (72 native functions).
//
// Echoes' CPhysicsActor.cpp, in the same order. Functions are emitted in reverse source order;
// rstl::auto_ptr<CCollisionCache>'s destructor (0x800F09B4) is emitted between the destructor and
// the constructor, and the static initializer (0x800F0D74) last.
#include "MetroidPrime/CPhysicsActor.hpp"

#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/CGameDebugDraw.hpp"
#include "MetroidPrime/CPhysicsState.hpp"

#include "Collision/CCollisionInfoList.hpp"
#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Math/CMath.hpp"
#include "Kyoto/Math/CQuaternion.hpp"
#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CloseEnough.hpp"
#include "MetaRender/CCubeRenderer.hpp"
#include "WorldFormat/CCollisionCache.hpp"

#include "rstl/math.hpp"

#include <float.h>
#include <math.h>

static const float gkEpsilon32 = FLT_EPSILON;

const StepData CPhysicsActor::skDefaultStepData(0.3f, 0.3f, 0);

CPhysicsActor::CPhysicsActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info,
                             uint castFlags, const CTransform4f& xf, const CModelData& mData,
                             const CMaterialList& matList, const CAABox& aabb,
                             const SMoverData& moverData, const CActorParameters& actParams,
                             const StepData& stepData)
: CRenderActor(uid, name, info, castFlags | 4, xf, mData, matList, actParams, kInvalidUniqueId)
, mMass(moverData.mMass)
, x174_(1.f)
, mMovable(true)
, mAngularEnabled(false)
, mStandardCollider(false)
, mConstantForce(0.f, 0.f, 0.f)
, mAngularMomentum(CAxisAngle::Identity())
, x194_(CMatrix3f::Identity())
, mVelocity(0.f, 0.f, 0.f)
, mAngularVelocity(CAxisAngle::Identity())
, mMomentum(moverData.mMomentum)
, mForce(0.f, 0.f, 0.f)
, mImpulse(0.f, 0.f, 0.f)
, mTorque(CAxisAngle::Identity())
, mAngularImpulse(CAxisAngle::Identity())
, mMoveImpulse(0.f, 0.f, 0.f)
, mMoveAngularImpulse(CAxisAngle::Identity())
, mBaseBoundingBox(aabb)
, mCollisionPrimitive(aabb, matList)
, mPrimitiveOffset(xf.GetTranslation())
, mLastNonCollidingState(xf.GetTranslation(), CNUQuaternion::BuildFromMatrix3f(xf.BuildMatrix3f()),
                         CVector3f::Zero(), CAxisAngle::Identity())
, mMaximumCollisionVelocity(1000000.f)
, mStepUpHeight(stepData.stepUp)
, mStepDownHeight(stepData.stepDown)
, mRestitutionCoefModifier(0.f)
, mCollisionAccuracyModifier(1.f)
, mNumTicksStuck(0)
, mNumTicksPartialUpdate(0)
, mCollisionCache(stepData.unk & 1 ? rs_new_line(84)
                                         CCollisionCache(CAABox::Identity(), 1, 2, uid.value)
                                   : nullptr) {
  SetMass(moverData.mMass);
  MoveCollisionPrimitive(CVector3f::Zero());
  SetVelocityOR(moverData.mVelocity);
  SetAngularVelocityOR(moverData.mAngularVelocity);
  ComputeDerivedQuantities();
}

CPhysicsActor::~CPhysicsActor() {}

void CPhysicsActor::ApplyImpulseWR(const CVector3f& impulse, const CAxisAngle& angularImpulse) {
  mImpulse = mImpulse + impulse;
  mAngularImpulse = mAngularImpulse + angularImpulse;
}

void CPhysicsActor::ApplyTorqueWR(const CVector3f& torque) {
  mTorque = mTorque + CAxisAngle(torque);
}

void CPhysicsActor::ApplyForceWR(const CVector3f& force, const CAxisAngle& torque) {
  mForce = mForce + force;
  mTorque = mTorque + torque;
}

void CPhysicsActor::ApplyImpulseOR(const CVector3f& impulse, const CAxisAngle& angularImpulse) {
  mImpulse = mImpulse + GetTransform().Rotate(impulse);
  CAxisAngle rotated(GetTransform().Rotate(angularImpulse.GetVector()));
  mAngularImpulse = mAngularImpulse + rotated;
}

void CPhysicsActor::ApplyForceOR(const CVector3f& force, const CAxisAngle& torque) {
  mForce = mForce + GetTransform().Rotate(force);
  CAxisAngle rotated(GetTransform().Rotate(torque.GetVector()));
  mTorque = mTorque + rotated;
}

void CPhysicsActor::ComputeDerivedQuantities() {
  mVelocity = (1.f / GetMass()) * mConstantForce;
  x194_ = GetTransform().BuildMatrix3f();
  float inertia = GetInertiaTensor();
  mAngularVelocity = CAxisAngle((1.f / inertia) * mAngularMomentum.GetVector());
}

CPhysicsState CPhysicsActor::GetPhysicsState() const {
  return CPhysicsState(GetTranslation(), CQuaternion::FromMatrix(GetTransform()), mConstantForce,
                       mAngularMomentum, mMomentum, mForce, mImpulse, mTorque, mAngularImpulse);
}

void CPhysicsActor::SetPhysicsState(const CPhysicsState& state) {
  SetTranslation(state.GetTranslation());
  SetTransform(state.GetOrientation().BuildTransform4f(GetTranslation()));
  mConstantForce = state.GetConstantForceWR();
  mAngularMomentum = state.GetAngularMomentumWR();
  mMomentum = state.GetMomentumWR();
  mForce = state.GetForceWR();
  mImpulse = state.GetImpulseWR();
  mTorque = state.GetTorque();
  mAngularImpulse = state.GetAngularImpulseWR();
  ComputeDerivedQuantities();
}

CVector3f CPhysicsActor::CalculateNewVelocityWR_UsingImpulses() const {
  float invMass = 1.f / GetMass();
  return mVelocity + invMass * (mImpulse + mMoveImpulse);
}

CMotionState CPhysicsActor::PredictMotion(float dt) const {
  CMotionState msl = PredictLinearMotion(dt);
  CMotionState msa = PredictAngularMotion(dt);
  return CMotionState(msl.GetTranslation(), msa.GetOrientation(), msl.GetVelocity(),
                      msa.GetAngularMomentum());
}

CMotionState CPhysicsActor::PredictAngularMotion(float dt) const {
  float inertia = GetInertiaTensor();
  CVector3f v1 = (1.f / inertia) * (mAngularImpulse.GetVector() + mMoveAngularImpulse.GetVector());
  CVector3f v2 = mAngularVelocity.GetVector() + v1;

  CNUQuaternion q3 = (0.5f * CNUQuaternion(0.f, v2)) *
                     CNUQuaternion::BuildFromQuaternion(CQuaternion::FromMatrix(GetTransform()));
  CAxisAngle torque = mTorque;

  return CMotionState(CVector3f::Zero(), q3 * dt, CVector3f::Zero(),
                      (torque * dt) + mAngularImpulse);
}

CMotionState CPhysicsActor::PredictLinearMotion(float dt) const {
  CVector3f velocity = CalculateNewVelocityWR_UsingImpulses();
  CVector3f sum = GetConstantTotalForceWR();

  return CMotionState(dt * velocity, CNUQuaternion(0.f, CVector3f::Zero()), dt * sum + mImpulse,
                      CAxisAngle::Identity());
}

CMotionState CPhysicsActor::PredictMotion_Internal(float dt) const {
  if (!mAngularEnabled) {
    CMotionState msl = PredictLinearMotion(dt);
    CMotionState msa = PredictAngularMotion(dt);
    return CMotionState(msl.GetTranslation(), msa.GetOrientation(), msl.GetVelocity(),
                        msa.GetAngularMomentum());
  } else {
    return PredictLinearMotion(dt);
  }
}

void CPhysicsActor::SetMotionState(const CMotionState& state) {
  SetTransform(
      CQuaternion::FromNUQuaternion(state.GetOrientation()).BuildTransform4f(GetTranslation()));
  SetTranslation(state.GetTranslation());
  mConstantForce = state.GetVelocity();
  mAngularMomentum = state.GetAngularMomentum();
  ComputeDerivedQuantities();
}

CMotionState CPhysicsActor::GetMotionState() const {
  return CMotionState(GetTranslation(),
                      CNUQuaternion::BuildFromQuaternion(CQuaternion::FromMatrix(GetTransform())),
                      mConstantForce, mAngularMomentum);
}

void CPhysicsActor::AddMotionState(const CMotionState& state) {
  CNUQuaternion q(CNUQuaternion::BuildFromQuaternion(CQuaternion::FromMatrix(GetTransform())));
  q += state.GetOrientation();
  SetTransform(CQuaternion::FromNUQuaternion(q).BuildTransform4f(GetTranslation()));
  SetTranslation(GetTranslation() + state.GetTranslation());

  mConstantForce += state.GetVelocity();
  mAngularMomentum += state.GetAngularMomentum();

  ComputeDerivedQuantities();
}

bool CPhysicsActor::WillMove(const CStateManager& mgr) {
  if (close_enough(mVelocity, CVector3f::Zero()) && close_enough(mImpulse, CVector3f::Zero()) &&
      close_enough(mTorque.GetVector(), CVector3f::Zero()) &&
      close_enough(mMoveImpulse, CVector3f::Zero()) &&
      close_enough(mAngularVelocity.GetVector(), CVector3f::Zero()) &&
      close_enough(mAngularImpulse.GetVector(), CVector3f::Zero()) &&
      close_enough(mMoveAngularImpulse.GetVector(), CVector3f::Zero()) &&
      close_enough(GetTotalForceWR(), CVector3f::Zero())) {
    return false;
  }
  return true;
}

void CPhysicsActor::Stop() {
  ClearForcesAndTorques();
  mConstantForce = CVector3f::Zero();
  mAngularMomentum = CAxisAngle::Identity();
  ComputeDerivedQuantities();
}

void CPhysicsActor::ClearForcesAndTorques() {
  mForce = mImpulse = mMoveImpulse = CVector3f::Zero();
  mTorque = mAngularImpulse = mMoveAngularImpulse = CAxisAngle::Identity();
}

void CPhysicsActor::ClearImpulses() {
  mImpulse = mMoveImpulse = CVector3f::Zero();
  mAngularImpulse = mMoveAngularImpulse = CAxisAngle::Identity();
}

void CPhysicsActor::ClearAngularImpulses() {
  mAngularImpulse = mMoveAngularImpulse = CAxisAngle::Identity();
}

void CPhysicsActor::UseCollisionImpulses() {
  mConstantForce += mImpulse;
  mAngularMomentum += mAngularImpulse;
  mImpulse = CVector3f::Zero();
  mAngularImpulse = CAxisAngle::Identity();
  ComputeDerivedQuantities();
}

// The prototype checks the time step of the four "move to" helpers.
void CPhysicsActor::MoveToWR(const CVector3f& trans, float dt) {
  RS_VERIFY_THROW(370, dt > gkEpsilon32, false, "delta time must be positive!");
  mConstantForce = (1.f / dt) * (GetMass() * (trans - GetTranslation()));
  ComputeDerivedQuantities();
}

void CPhysicsActor::MoveToInOneFrameWR(const CVector3f& trans, float dt) {
  RS_VERIFY_THROW(385, dt > gkEpsilon32, false, "delta time must be positive!");
  mMoveImpulse += (1.f / dt) * (GetMass() * (trans - GetTranslation()));
}

CVector3f CPhysicsActor::GetMoveToORImpulseWR(const CVector3f& trans, float dt) const {
  RS_VERIFY_THROW(396, dt > gkEpsilon32, false, "delta time must be positive!");
  CVector3f impulse = GetTransform().Rotate(trans);
  return (1.f / dt) * (GetMass() * impulse);
}

CVector3f CPhysicsActor::GetRotateToORAngularMomentumWR(const CQuaternion& q, float dt) const {
  RS_VERIFY_THROW(407, dt > gkEpsilon32, false, "delta time must be positive!");
  if (q.GetScalar() > 0.99999976f) {
    return CVector3f::Zero();
  }
  const CQuaternion rotated(q.GetScalar(), GetTransform().Rotate(q.GetVector()));
  double ac = acos(rotated.GetScalar());
  float inertia = GetInertiaTensor();
  return inertia *
         (((2.f * static_cast< float >(ac)) * (1.f / dt)) * rotated.GetVector().AsNormalized());
}

void CPhysicsActor::MoveToOR(const CVector3f& trans, float dt) {
  mConstantForce = GetMoveToORImpulseWR(trans, dt);
  ComputeDerivedQuantities();
}

void CPhysicsActor::RotateToOR(const CQuaternion& q, float dt) {
  mAngularMomentum = CAxisAngle(GetRotateToORAngularMomentumWR(q, dt));
  ComputeDerivedQuantities();
}

void CPhysicsActor::MoveInOneFrameOR(const CVector3f& trans, float dt) {
  mMoveImpulse += GetMoveToORImpulseWR(trans, dt);
}

void CPhysicsActor::RotateInOneFrameOR(const CQuaternion& q, float dt) {
  mMoveAngularImpulse += CAxisAngle(GetRotateToORAngularMomentumWR(q, dt));
}

void CPhysicsActor::SetVelocityOR(const CVector3f& vel) {
  SetVelocityWR(GetTransform().Rotate(vel));
}

CVector3f CPhysicsActor::GetTotalForceWR() const { return mForce + mMomentum; }

void CPhysicsActor::SetVelocityWR(const CVector3f& vel) {
  mVelocity = vel;
  mConstantForce = GetMass() * mVelocity;
}

void CPhysicsActor::SetAngularVelocityWR(const CAxisAngle& angVel) {
  mAngularVelocity = angVel;
  float inertia = GetInertiaTensor();
  mAngularMomentum = CAxisAngle(inertia * mAngularVelocity.GetVector());
}

CAxisAngle CPhysicsActor::GetAngularVelocityOR() const {
  return CAxisAngle(GetTransform().TransposeRotate(mAngularVelocity.GetVector()));
}

void CPhysicsActor::SetAngularVelocityOR(const CAxisAngle& angVel) {
  mAngularVelocity = CAxisAngle(GetTransform().Rotate(angVel.GetVector()));
  float inertia = GetInertiaTensor();
  mAngularMomentum = CAxisAngle(inertia * mAngularVelocity.GetVector());
}

// Unlike Echoes, neither the mass nor the inertia tensor is clamped to be positive.
void CPhysicsActor::SetMass(float mass) {
  mMass = mass;
  SetInertiaTensorScalar(0.16666667f * mMass);
}

void CPhysicsActor::SetInertiaTensorScalar(float tensor) { x174_ = tensor / GetMass(); }

float CPhysicsActor::GetInertiaTensor() const { return x174_ * GetMass(); }

const CCollisionPrimitive* CPhysicsActor::GetCollisionPrimitive() const {
  return &mCollisionPrimitive;
}

void CPhysicsActor::SetCollisionPrimitive(const CCollidableAABox& primitive) {
  mCollisionPrimitive = primitive;
}

void CPhysicsActor::MoveCollisionPrimitive(const CVector3f& offset) { mPrimitiveOffset = offset; }

CTransform4f CPhysicsActor::GetPrimitiveTransform() const {
  return CTransform4f::Translate(GetTransform().GetTranslation() + mPrimitiveOffset);
}

void CPhysicsActor::CollidedWith(const TUniqueId& id, const CCollisionInfoList& list,
                                 CStateManager& mgr) {}

const CAABox& CPhysicsActor::GetBaseBoundingBox() const { return mBaseBoundingBox; }

CAABox CPhysicsActor::GetBoundingBox() const {
  CVector3f offset = mPrimitiveOffset + GetTranslation();
  return CAABox(mBaseBoundingBox.GetMinPoint() + offset, mBaseBoundingBox.GetMaxPoint() + offset);
}

// The margins are 0.3 where Echoes uses 0.5, 1 and 1.5.
CAABox CPhysicsActor::GetMotionVolume(float dt) const {
  CAABox aabox = GetCollisionPrimitive()->CalculateAABox(GetPrimitiveTransform());
  CVector3f velocity = CalculateNewVelocityWR_UsingImpulses();

  const CVector3f dv = dt * velocity;
  aabox.AccumulateBounds(aabox.GetMaxPoint() + dv);
  aabox.AccumulateBounds(aabox.GetMinPoint() + dv);

  float up = rstl::max_val(GetStepUpHeight(), 0.f);
  aabox.AccumulateBounds(aabox.GetMaxPoint() + CVector3f(0.3f, 0.3f, 0.3f + up));

  float down = rstl::max_val(GetStepDownHeight(), 0.f);
  aabox.AccumulateBounds(aabox.GetMinPoint() - CVector3f(0.3f, 0.3f, 0.3f + down));
  return aabox;
}

void CPhysicsActor::SetBoundingBox(const CAABox& box) {
  mBaseBoundingBox = box;
  MoveCollisionPrimitive(CVector3f::Zero());
}

float CPhysicsActor::GetWeight() const { return GravityConstant() * GetMass(); }

CVector3f CPhysicsActor::GetPrimitiveOffset() const { return mPrimitiveOffset; }

void CPhysicsActor::SetPrimitiveOffset(const CVector3f& offset) { mPrimitiveOffset = offset; }

void CPhysicsActor::SetStepDownHeight(float height) { mStepDownHeight = height; }

float CPhysicsActor::GetStepDownHeight() const { return mStepDownHeight; }

void CPhysicsActor::SetStepUpHeight(float height) { mStepUpHeight = height; }

float CPhysicsActor::GetStepUpHeight() const { return mStepUpHeight; }

CVector3f CPhysicsActor::GetOrbitPosition(const CStateManager&) const {
  return GetBoundingBox().GetCenterPoint();
}

CVector3f CPhysicsActor::GetAimPosition(const CStateManager&, float dt) const {
  if (dt > 0.f) {
    CMotionState s = PredictMotion(dt);
    return GetBoundingBox().GetCenterPoint() + s.GetTranslation();
  }
  return GetBoundingBox().GetCenterPoint();
}

void CPhysicsActor::DrawBoundingBox(const CStateManager&) {
  gpRender->SetModelMatrix(CTransform4f::Identity());
  if (gpGameDebug->GetOptionValue(CGameDebug::kDO_DrawObjectCollisionBoxes) > 1.f) {
    DrawDebugAABox(GetBoundingBox(), 1.f, 1.f, 1.f, 1.f);
  }
}

void CPhysicsActor::DrawCollisionBoxes(const CStateManager& mgr) {
  DrawBoundingBox(mgr);
  CRenderActor::DrawCollisionBoxes(mgr);
}

void CPhysicsActor::SetCoefficientOfRestitutionModifier(float modifier) {
  mRestitutionCoefModifier = modifier;
}

float CPhysicsActor::GetCoefficientOfRestitutionModifier() const {
  return mRestitutionCoefModifier;
}

void CPhysicsActor::SetCollisionAccuracyModifier(float modifier) {
  mCollisionAccuracyModifier = modifier;
}

float CPhysicsActor::GetCollisionAccuracyModifier() const { return mCollisionAccuracyModifier; }

void CPhysicsActor::SetMaxVelocityAfterCollision(float velocity) {
  mMaximumCollisionVelocity = velocity;
}

float CPhysicsActor::GetMaximumCollisionVelocity() const { return mMaximumCollisionVelocity; }

// Echoes returns false.
bool CPhysicsActor::IsOnStaticGround() const { return true; }

CCollisionCache* CPhysicsActor::GetCollisionCache() const { return mCollisionCache.get(); }

float CPhysicsActor::GetMass() const { return mMass; }
