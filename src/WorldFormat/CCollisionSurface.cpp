/*
 * G2MEAB WorldFormat/CCollisionSurface.cpp translation-unit scaffold.
 * .text: 0x8059C9C8..0x8059CBFC (3 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Three complete methods GetEdgePlane59C9C8, GetPlane59CAE0, GetNormal59CB64+98,
 * preserving Echoes exact nontrivial fingerprints and inspected cross/normal semantics. No target
 * IsDegenerate native precedes59C9C8: previous59C904 is optional-surface assignment owned by
 * OctTree tests. Prime full three-native inventory/source/header checked, with different
 * constructor emission. Next59CBFC is edge stream constructor.
 */

#include "WorldFormat/CCollisionSurface.hpp"

// Guessed name
CPlane CCollisionSurface::GetEdgePlane(int edge) const {
  const CVector3f surfaceNormal = GetNormal();
  const CUnitVector3f normal(surfaceNormal);
  const int nextVertex[] = {1, 2, 0};
  const CVector3f edgeDirection = mVertices[nextVertex[edge]] - mVertices[edge];
  const CUnitVector3f edgeNormal(CVector3f::Cross(normal, edgeDirection));
  return CPlane(CVector3f::Dot(edgeNormal, mVertices[edge]), edgeNormal);
}

CPlane CCollisionSurface::GetPlane() const {
  const CVector3f surfaceNormal = GetNormal();
  const CUnitVector3f normal(surfaceNormal);
  return CPlane(CVector3f::Dot(normal, mVertices[0]), normal);
}

CUnitVector3f CCollisionSurface::GetNormal() const {
  return CUnitVector3f(
      CVector3f::Cross(mVertices[1] - mVertices[0], mVertices[2] - mVertices[0]).AsNormalized(),
      CUnitVector3f::kN_No);
}
