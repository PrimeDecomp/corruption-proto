/*
 * G2MEAB Collision/CCollidableOrientedBox.cpp
 * .text: 0x804801DC..0x80480884 (12 native functions, including emitted helpers).
 * Inferred descriptive basename; the original class/source name is unproven. An oriented-box
 * collision primitive (type 'ORBX') whose box-box contact function runs the GJK solver on
 * temporary copies placed with the collision transforms.
 */
#include "Collision/CCollidableOrientedBox.hpp"

#include "Collision/CCollisionInfo.hpp"
#include "Collision/CCollisionInfoList.hpp"
#include "Collision/CGjkSolver.hpp"
#include "Collision/CInternalCollisionStructure.hpp"
#include "Collision/CInternalRayCastStructure.hpp"
#include "Collision/CMaterialFilter.hpp"
#include "Collision/CRayCastResult.hpp"
#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Math/CVector3d.hpp"

// Native SAT test over two oriented boxes expressed in the same space (COBBox.cpp).

uint CCollidableOrientedBox::sTableIndex = -1;

CCollidableOrientedBox::CCollidableOrientedBox(const COBBox& box, const CMaterialList& material)
: CCollisionPrimitive(material), mBox(box) {}

CCollidableOrientedBox::~CCollidableOrientedBox() {}

void CCollidableOrientedBox::SetStaticTableIndex(uint idx) { sTableIndex = idx; }

CCollisionPrimitive::Type CCollidableOrientedBox::GetType() {
  return Type(SetStaticTableIndex, "CCollidableOrientedBox");
}

uint CCollidableOrientedBox::GetTableIndex() const { return sTableIndex; }

FourCC CCollidableOrientedBox::GetPrimType() const { return 'ORBX'; }

CAABox CCollidableOrientedBox::CalculateAABox(const CTransform4f& xf) const {
  return mBox.CalculateAABox(xf);
}

CAABox CCollidableOrientedBox::CalculateLocalAABox() const {
  return CAABox(-mBox.GetSize(), mBox.GetSize());
}

CRayCastResult
CCollidableOrientedBox::CastRayInternal(const CInternalRayCastStructure& rayCast) const {
  if (rayCast.GetFilter().Passes(GetMaterial())) {
    return CRayCastResult::MakeInvalid();
  }
  return CRayCastResult::MakeInvalid();
}

CVector3f CCollidableOrientedBox::GetSupportPoint(const CVector3f& dir) const {
  const CVector3f normalized = dir.AsNormalized();
  const CTransform4f& xf = mBox.GetTransform();
  const CVector3f& extents = mBox.GetSize();
  CVector3f result = CVector3f::Zero();

  const CVector3f axisX = extents.GetX() * xf.GetRight();
  float signX = 0.f;
  const float dotX = CVector3f::Dot(axisX, normalized);
  if (dotX > 0.f) {
    signX = 1.f;
  }
  if (dotX < 0.f) {
    signX = -1.f;
  }
  result += signX * axisX;

  const CVector3f axisY = extents.GetY() * xf.GetForward();
  float signY = 0.f;
  const float dotY = CVector3f::Dot(axisY, normalized);
  if (dotY > 0.f) {
    signY = 1.f;
  }
  if (dotY < 0.f) {
    signY = -1.f;
  }
  result += signY * axisY;

  const CVector3f axisZ = extents.GetZ() * xf.GetUp();
  float signZ = 0.f;
  const float dotZ = CVector3f::Dot(axisZ, normalized);
  if (dotZ > 0.f) {
    signZ = 1.f;
  }
  if (dotZ < 0.f) {
    signZ = -1.f;
  }
  result += signZ * axisZ;

  result += xf.GetTranslation();
  return result;
}

namespace Collide {
bool OBBox_OBBox(const CInternalCollisionStructure& collision, CCollisionInfoList& list) {
  const CCollidableOrientedBox& left =
      static_cast< const CCollidableOrientedBox& >(collision.GetLeft().GetPrim());
  const CCollidableOrientedBox& right =
      static_cast< const CCollidableOrientedBox& >(collision.GetRight().GetPrim());

  CGjkSolver solver;
  CVector3d pointA = CVector3d::Zero();
  CVector3d pointB = CVector3d::Zero();
  CCollidableOrientedBox boxA(COBBox(left.GetBox(), collision.GetLeft().GetTransform()),
                              left.GetMaterial());
  CCollidableOrientedBox boxB(COBBox(right.GetBox(), collision.GetRight().GetTransform()),
                              right.GetMaterial());
  solver.ClosestPoints(boxA, CTransform4f::Identity(), boxB, CTransform4f::Identity(), pointA,
                       pointB);

  CVector3f delta = (pointA - pointB).AsCVector3f();
  float distance = delta.Magnitude();
  if (distance < 0.001f) {
    CVector3f normal = (1.f / distance) * delta;
    CVector3f negNormal = -normal;
    list.Add(CCollisionInfo(pointA.AsCVector3f(), left.GetMaterial(), right.GetMaterial(), normal,
                            negNormal, -1));
    return true;
  }
  return false;
}

bool OBBox_OBBox_Bool(const CInternalCollisionStructure& collision) {
  const CCollidableOrientedBox& left =
      static_cast< const CCollidableOrientedBox& >(collision.GetLeft().GetPrim());
  const CCollidableOrientedBox& right =
      static_cast< const CCollidableOrientedBox& >(collision.GetRight().GetPrim());

  COBBox boxA(left.GetBox(), collision.GetLeft().GetTransform());
  COBBox boxB(right.GetBox(), collision.GetRight().GetTransform());
  bool intersects = boxA.OBBIntersectsBox(boxB);

  COBBox localA(left.GetBox(), CTransform4f::Identity());
  COBBox localB(right.GetBox(),
                collision.GetLeft().GetTransform().GetQuickInverse() *
                    collision.GetRight().GetTransform());
  COBBox::CSeparationInfo satInfo;
  if (COBBox::OBBIntersectsBox(satInfo, localA, localB) != intersects) {
    rs_debugger_printf("Collide::OBBox_OBBox_Bool result mismatch.\n");
  }
  return intersects;
}
} // namespace Collide
