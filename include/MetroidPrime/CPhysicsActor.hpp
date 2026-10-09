#ifndef _CPHYSICSACTOR
#define _CPHYSICSACTOR

#include "types.h"

#include "MetroidPrime/CAxisAngle.hpp"
#include "MetroidPrime/CRenderActor.hpp"

#include "Collision/CCollidableAABox.hpp"
#include "Kyoto/Math/CAABox.hpp"
#include "Kyoto/Math/CMatrix3f.hpp"
#include "Kyoto/Math/CNUQuaternion.hpp"
#include "Kyoto/Math/CVector3f.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/optional_object.hpp"

class CCollisionCache;
class CCollisionInfoList;
class CPhysicsState;
class CQuaternion;

// Echoes' structure; the constructor reads the momentum at 0x18 and the mass at 0x30.
struct SMoverData {
  CVector3f mVelocity;
  CAxisAngle mAngularVelocity;
  CVector3f mMomentum;
  CAxisAngle x24_;
  float mMass;

  SMoverData(float mass, const CVector3f& velocity = CVector3f::Zero(),
             const CAxisAngle& angularVelocity = CAxisAngle::Identity(),
             const CVector3f& momentum = CVector3f::Zero(),
             const CAxisAngle& unk = CAxisAngle::Identity())
  : mVelocity(velocity)
  , mAngularVelocity(angularVelocity)
  , mMomentum(momentum)
  , x24_(unk)
  , mMass(mass) {}
};

// Echoes' class. Both constructors are emitted with CPlayer (0x80018188 and 0x80014754).
class CMotionState {
public:
  CMotionState(const CVector3f& translation, const CNUQuaternion& orientation,
               const CVector3f& velocity, const CAxisAngle& angularMomentum);
  CMotionState(const CMotionState&);

  const CVector3f& GetTranslation() const { return mTranslation; }
  const CNUQuaternion& GetOrientation() const { return mOrientation; }
  const CVector3f& GetVelocity() const { return mVelocity; }
  const CAxisAngle& GetAngularMomentum() const { return mAngularMomentum; }

private:
  CVector3f mTranslation;
  CNUQuaternion mOrientation;
  CVector3f mVelocity;
  CAxisAngle mAngularMomentum;
};
CHECK_SIZEOF(CMotionState, 0x34)

// Echoes' structure. Bit 0 of the flags gives the actor a collision cache.
struct StepData {
  float stepUp;
  float stepDown;
  int unk;

  StepData(float up, float down, int u) : stepUp(up), stepDown(down), unk(u) {}
};

// Layout from the constructor (0x800F0A18) and destructor (0x800F0938); vtable 0x806B3CC8.
// It derives from CRenderActor rather than CActor and adds cast flag 4 (CPhysicsActorList).
//
// Compared with Echoes, the prototype keeps no reciprocal mass and only one inertia word (0x174,
// the inertia tensor divided by the mass): the mass is read through the new virtual GetMass,
// and the inverse mass and inertia tensor are computed on demand. The member order otherwise
// follows Echoes, and so does the order of the functions in CPhysicsActor.cpp.
class CPhysicsActor : public CRenderActor {
public:
  // Guessed name; shared default step settings initialized by this TU.
  static const StepData skDefaultStepData;

  CPhysicsActor(TUniqueId uid, const rstl::string& name, const CEntityInfo& info, uint castFlags,
                const CTransform4f& xf, const CModelData& mData, const CMaterialList& matList,
                const CAABox& aabb, const SMoverData& moverData, const CActorParameters& actParams,
                const StepData& stepData);

  // CEntity
  ~CPhysicsActor();
  CEntity* TypesMatch(int typeId) const; // Emitted in TypesMatch.cpp.

  // CActor
  CVector3f GetOrbitPosition(const CStateManager& mgr) const;
  CVector3f GetAimPosition(const CStateManager& mgr, float dt) const;
  // Draws the bounding box, then what CRenderActor draws. Echoes overrides Render here instead.
  void DrawCollisionBoxes(const CStateManager& mgr);

  // CPhysicsActor. The slots follow Echoes' order, with GetMass made virtual in front and a
  // debug draw at the end.
  virtual float GetMass() const;                                    // 0xA0
  virtual const CCollisionPrimitive* GetCollisionPrimitive() const; // 0xA4
  virtual CTransform4f GetPrimitiveTransform() const;               // 0xA8
  virtual void CollidedWith(const TUniqueId& id, const CCollisionInfoList& list,
                            CStateManager& mgr); // 0xAC
  virtual float GetStepDownHeight() const;       // 0xB0
  virtual float GetStepUpHeight() const;         // 0xB4
  virtual bool IsOnStaticGround() const;         // 0xB8, Echoes' slot
  virtual float GetWeight() const;               // 0xBC
  // Guessed name. 0xC0: draws the bounding box in white while "Draw Object Collision Boxes" is
  // above 1.
  virtual void DrawBoundingBox(const CStateManager& mgr);

  CCollisionCache* GetCollisionCache() const; // Echoes' guessed name.
  void SetMass(float mass);
  void SetInertiaTensorScalar(float tensor);
  float GetInertiaTensor() const; // Guessed name; the mass times 0x174.

  void SetStepUpHeight(float height);
  void SetStepDownHeight(float height); // Guessed name, after SetStepUpHeight.

  const CAABox& GetBaseBoundingBox() const;
  CAABox GetBoundingBox() const;
  void SetBoundingBox(const CAABox& box);
  void SetCollisionPrimitive(const CCollidableAABox& primitive);
  CAABox GetMotionVolume(float dt) const;

  void ApplyImpulseWR(const CVector3f& impulse, const CAxisAngle& angularImpulse);
  void ApplyTorqueWR(const CVector3f& torque);
  void ApplyForceWR(const CVector3f& force, const CAxisAngle& torque);
  void ApplyImpulseOR(const CVector3f& impulse, const CAxisAngle& angularImpulse);
  void ApplyForceOR(const CVector3f& force, const CAxisAngle& torque);

  void MoveCollisionPrimitive(const CVector3f& offset);
  void SetVelocityWR(const CVector3f& vel);
  void SetAngularVelocityWR(const CAxisAngle& angVel);
  void SetVelocityOR(const CVector3f& vel);
  CAxisAngle GetAngularVelocityOR() const;
  void SetAngularVelocityOR(const CAxisAngle& angVel);
  void ClearForcesAndTorques();
  void ClearImpulses();
  void ClearAngularImpulses(); // Echoes' guessed name.
  void ComputeDerivedQuantities();
  void UseCollisionImpulses();
  bool WillMove(const CStateManager& mgr);
  void Stop();

  CVector3f GetPrimitiveOffset() const;
  // Guessed name. A second copy of MoveCollisionPrimitive, next to GetPrimitiveOffset.
  void SetPrimitiveOffset(const CVector3f& offset);

  const CVector3f& GetConstantForceWR() const { return mConstantForce; }
  const CAxisAngle& GetAngularMomentumWR() const { return mAngularMomentum; }
  const CVector3f& GetVelocityWR() const { return mVelocity; }
  const CAxisAngle& GetAngularVelocityWR() const { return mAngularVelocity; }
  const CVector3f& GetMomentumWR() const { return mMomentum; }
  const CVector3f& GetForceWR() const { return mForce; }
  const CVector3f& GetImpulseWR() const { return mImpulse; }
  const CAxisAngle& GetTorqueWR() const { return mTorque; }
  const CAxisAngle& GetAngularImpulseWR() const { return mAngularImpulse; }
  bool GetMovable() const { return mMovable; }
  void SetMovable(bool movable) { mMovable = movable; }
  bool GetAngularEnabled() const { return mAngularEnabled; }

  float GetCoefficientOfRestitutionModifier() const;
  void SetCoefficientOfRestitutionModifier(float modifier);
  float GetCollisionAccuracyModifier() const;
  void SetCollisionAccuracyModifier(float modifier);
  float GetMaximumCollisionVelocity() const;
  void SetMaxVelocityAfterCollision(float velocity);

  CPhysicsState GetPhysicsState() const;
  void SetPhysicsState(const CPhysicsState& state);
  CMotionState GetMotionState() const;
  void SetMotionState(const CMotionState& state);
  CVector3f CalculateNewVelocityWR_UsingImpulses() const;
  CMotionState PredictMotion(float dt) const;
  CMotionState PredictAngularMotion(float dt) const;
  CMotionState PredictLinearMotion(float dt) const;
  CMotionState PredictMotion_Internal(float dt) const;
  void AddMotionState(const CMotionState& state);

  void MoveToWR(const CVector3f& trans, float dt);
  void MoveToInOneFrameWR(const CVector3f& trans, float dt);
  CVector3f GetMoveToORImpulseWR(const CVector3f& trans, float dt) const;
  CVector3f GetRotateToORAngularMomentumWR(const CQuaternion& q, float dt) const;
  void MoveInOneFrameOR(const CVector3f& trans, float dt);
  void RotateInOneFrameOR(const CQuaternion& q, float dt);
  void MoveToOR(const CVector3f& trans, float dt);
  void RotateToOR(const CQuaternion& q, float dt);

  CVector3f GetTotalForceWR() const;
  CVector3f GetConstantTotalForceWR() const { return mForce + mMomentum; }

  static float GravityConstant() { return 9.81f * 2.5f; } // Prime's definition

private:
  float mMass;       // 0x170
  float x174_;       // inertia tensor / mass
  bool mMovable : 1; // 0x178
  bool mAngularEnabled : 1;
  uchar mStandardCollider;                                  // 0x179
  CVector3f mConstantForce;                                 // 0x17C, the linear momentum
  CAxisAngle mAngularMomentum;                              // 0x188
  CMatrix3f x194_;                                          // the transform's rotation
  CVector3f mVelocity;                                      // 0x1B8
  CAxisAngle mAngularVelocity;                              // 0x1C4
  CVector3f mMomentum;                                      // 0x1D0
  CVector3f mForce;                                         // 0x1DC
  CVector3f mImpulse;                                       // 0x1E8
  CAxisAngle mTorque;                                       // 0x1F4
  CAxisAngle mAngularImpulse;                               // 0x200
  CVector3f mMoveImpulse;                                   // 0x20C
  CAxisAngle mMoveAngularImpulse;                           // 0x218
  CAABox mBaseBoundingBox;                                  // 0x224
  int x23C_;                                                // never written here
  CCollidableAABox mCollisionPrimitive;                     // 0x240
  CVector3f mPrimitiveOffset;                               // 0x268
  CMotionState mLastNonCollidingState;                      // 0x274
  rstl::optional_object< CVector3f > mLastFloorPlaneNormal; // 0x2A8
  float mMaximumCollisionVelocity;                          // 0x2B8
  float mStepUpHeight;                                      // 0x2BC
  float mStepDownHeight;                                    // 0x2C0
  float mRestitutionCoefModifier;                           // 0x2C4
  float mCollisionAccuracyModifier;                         // 0x2C8
  uint mNumTicksStuck;                                      // 0x2CC
  uint mNumTicksPartialUpdate;                              // 0x2D0
  rstl::auto_ptr< CCollisionCache > mCollisionCache;        // 0x2D4, Echoes' guessed name
};
// 0x2DC rounded up to the 8-byte alignment of the material list.
CHECK_SIZEOF(CPhysicsActor, 0x2E0)

#endif // _CPHYSICSACTOR
