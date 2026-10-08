/*
 * G2MEAB Collision/CCollisionInfo.cpp translation-unit scaffold.
 * .text: 0x80474364..0x80474888 (7 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Leading prototype-only Transform74364 translates point and rotates extents and
 * normals in a 0x60 collision record; do not misname it as Prime GetSwapped. Swap74494,
 * Extreme74500 and four invalid/extent/point constructors74564..747A8 finish at74888. Both
 * reference source/header and full inventories support the seven-native coherent family. Next74888
 * registers primitive collider callbacks.
 */
#include "Collision/CCollisionInfo.hpp"

#include "Kyoto/Math/CAABox.hpp"

#include "rstl/algorithm.hpp"

CCollisionInfo::CCollisionInfo(const CVector3f& point, const CMaterialList& rightMat,
                               const CMaterialList& leftMat, const CVector3f& normal,
                               const ushort value)
: mPoint(point)
, mExtentX(CVector3f::Zero())
, mExtentY(CVector3f::Zero())
, mExtentZ(CVector3f::Zero())
, mMaterialLeft(leftMat)
, mMaterialRight(rightMat)
, mNormalLeft(normal)
, mNormalRight(-normal)
, mObjectId(value)
, mValid(true)
, mHasExtents(false) {}

CCollisionInfo::CCollisionInfo(const CVector3f& point, const CMaterialList& rightMat,
                               const CMaterialList& leftMat, const CVector3f& leftNormal,
                               const CVector3f& rightNormal, const ushort value)
: mPoint(point)
, mExtentX(CVector3f::Zero())
, mExtentY(CVector3f::Zero())
, mExtentZ(CVector3f::Zero())
, mMaterialLeft(leftMat)
, mMaterialRight(rightMat)
, mNormalLeft(leftNormal)
, mNormalRight(rightNormal)
, mObjectId(value)
, mValid(true)
, mHasExtents(false) {}

CCollisionInfo::CCollisionInfo(const CAABox& aabox, const CMaterialList& rightMat,
                               const CMaterialList& leftMat, const CVector3f& leftNormal,
                               const CVector3f& rightNormal, const ushort value)
: mPoint(aabox.GetMinPoint())
, mExtentX(aabox.GetMaxPoint().GetX() - aabox.GetMinPoint().GetX(), 0.f, 0.f)
, mExtentY(0.f, aabox.GetMaxPoint().GetY() - aabox.GetMinPoint().GetY(), 0.f)
, mExtentZ(0.f, 0.f, aabox.GetMaxPoint().GetZ() - aabox.GetMinPoint().GetZ())
, mMaterialLeft(leftMat)
, mMaterialRight(rightMat)
, mNormalLeft(leftNormal)
, mNormalRight(rightNormal)
, mObjectId(value)
, mValid(true)
, mHasExtents(true) {}

CCollisionInfo::CCollisionInfo(EInvalid)
: mPoint(0.f, 0.f, 0.f)
, mExtentX(0.f, 0.f, 0.f)
, mExtentY(0.f, 0.f, 0.f)
, mExtentZ(0.f, 0.f, 0.f)
, mMaterialLeft(CMaterialList())
, mMaterialRight(CMaterialList())
, mNormalLeft(0.f, 0.f, 0.f)
, mNormalRight(0.f, 0.f, 0.f)
, mObjectId(0xffff)
, mValid(false)
, mHasExtents(false) {}

CVector3f CCollisionInfo::GetExtreme() const { return mPoint + mExtentX + mExtentY + mExtentZ; }

void CCollisionInfo::Swap() {
  mNormalLeft = -mNormalLeft;
  mNormalRight = -mNormalRight;
  rstl::swap(mMaterialLeft, mMaterialRight);
}
