#ifndef _CAXISANGLE
#define _CAXISANGLE

#include "types.h"

#include "Kyoto/Math/CVector3f.hpp"

class CQuaternion;
class CUnitVector3f;

// Echoes' declaration; the functions are in CAxisAngle.cpp (0x8056098C..0x80560D78) under the
// same names as in Echoes.
class CAxisAngle {
public:
  CAxisAngle() : mVector(CVector3f::Zero()) {}
  explicit CAxisAngle(const CVector3f& vec);
  CAxisAngle(const CUnitVector3f& axis, float angle);
  CAxisAngle(float x, float y, float z) : mVector(x, y, z) {}
  static CAxisAngle FromVector(const CVector3f& vec);
  static CAxisAngle FromQuaternion(const CQuaternion& quat);

  static const CAxisAngle& Identity();
  float GetAngle() const;
  const CVector3f& GetVector() const;

  const CAxisAngle& operator*=(const float& rhs);
  const CAxisAngle& operator+=(const CAxisAngle& rhs);

private:
  static const CAxisAngle sIdentity;

  CVector3f mVector;
};
CHECK_SIZEOF(CAxisAngle, 0xc)

CAxisAngle operator+(const CAxisAngle& lhs, const CAxisAngle& rhs);
CAxisAngle operator*(const CAxisAngle& lhs, const float& rhs);
CAxisAngle operator*(const float& lhs, const CAxisAngle& rhs);

#endif // _CAXISANGLE
