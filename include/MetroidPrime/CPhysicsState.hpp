#ifndef _CPHYSICSSTATE
#define _CPHYSICSSTATE

#include "types.h"

#include "MetroidPrime/CAxisAngle.hpp"

#include "Kyoto/Math/CQuaternion.hpp"
#include "Kyoto/Math/CVector3f.hpp"

// Echoes' class. The constructor is CPhysicsState.cpp's only function (0x800F0DB8);
// CPhysicsActor::SetPhysicsState reads the members at the same offsets as Echoes (0x70 bytes).
class CPhysicsState {
public:
  CPhysicsState(const CVector3f& translation, const CQuaternion& orientation,
                const CVector3f& constantForce, const CAxisAngle& angularMomentum,
                const CVector3f& momentum, const CVector3f& force, const CVector3f& impulse,
                const CAxisAngle& torque, const CAxisAngle& angularImpulse);

  CVector3f GetTranslation() const { return mTranslation; }
  CQuaternion GetOrientation() const { return mOrientation; }
  CVector3f GetConstantForceWR() const { return mConstantForce; }
  CAxisAngle GetAngularMomentumWR() const { return mAngularMomentum; }
  CVector3f GetMomentumWR() const { return mMomentum; }
  CVector3f GetForceWR() const { return mForce; }
  CVector3f GetImpulseWR() const { return mImpulse; }
  CAxisAngle GetTorque() const { return mTorque; }
  CAxisAngle GetAngularImpulseWR() const { return mAngularImpulse; }

private:
  CVector3f mTranslation;
  CQuaternion mOrientation;
  CVector3f mConstantForce;
  CAxisAngle mAngularMomentum;
  CVector3f mMomentum;
  CVector3f mForce;
  CVector3f mImpulse;
  CAxisAngle mTorque;
  CAxisAngle mAngularImpulse;
};
CHECK_SIZEOF(CPhysicsState, 0x70)

#endif // _CPHYSICSSTATE
