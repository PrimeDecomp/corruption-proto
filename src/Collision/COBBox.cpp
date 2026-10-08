/*
 * G2MEAB Collision/COBBox.cpp translation-unit scaffold.
 * .text: 0x8047C680..0x8047E58C (14
 * native functions, including emitted helpers).
 * NonMatching: implementation has not been
 * reconstructed.
 * Boundary evidence: Retain new leading SAT core7C680 (15axes), contact-axis
 * helper7CF48 and
 * transform wrapper7CF9C before familiar OBBIntersect7D028. Preserve
 * AABox/quad/line tests,
 * FromAABox7E220, local-AABox wrapper7E2DC, CalculateAABox7E310,
 * streamctor7E484,
 * transformed-copyctor7E4CC and paramctor7E538+54. Both complete native
 * inventories/source/header
 * support the remainder; target extra helper closure is established by
 * calls, not retail counts.
 * Next7E58C transforms a CMRay into inverse coordinates.
 */
#include "Collision/COBBox.hpp"

#include "Collision/CMRay.hpp"
#include "Collision/CollisionUtil.hpp"

#include "Kyoto/Math/CMath.hpp"
#include "Kyoto/Math/CMatrix3f.hpp"
#include "Kyoto/Math/CQuad.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

#include <float.h>
#include <math.h>

bool COBBox::SeparatingAxisTest(CSeparationInfo& info, const bool& wantGap,
                                const CVector3f& aExtents, const CVector3f& translation,
                                const CMatrix3f& rot, const CVector3f& bExtents) {
  info.mAxis = -1;
  info.mDistance = FLT_MAX;
  float absT = -1.f;
  float sum = -1.f;
  bool hit = false;
  CMatrix3f absR(CMatrix3f::Identity());
  absR = CMatrix3f(1e-15f + CMath::AbsF(rot.Get00()),
                   1e-15f + CMath::AbsF(rot.Get01()),
                   1e-15f + CMath::AbsF(rot.Get02()),
                   1e-15f + CMath::AbsF(rot.Get10()),
                   1e-15f + CMath::AbsF(rot.Get11()),
                   1e-15f + CMath::AbsF(rot.Get12()),
                   1e-15f + CMath::AbsF(rot.Get20()),
                   1e-15f + CMath::AbsF(rot.Get21()),
                   1e-15f + CMath::AbsF(rot.Get22()));
  sum = aExtents[0] + bExtents[0]*absR.Get00() + bExtents[1]*absR.Get10() + bExtents[2]*absR.Get20();
  absT = CMath::AbsF(translation[0]);
  if (TestAxis(info, 1, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[1] + bExtents[0]*absR.Get01() + bExtents[1]*absR.Get11() + bExtents[2]*absR.Get21();
  absT = CMath::AbsF(translation[1]);
  if (TestAxis(info, 2, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[2] + bExtents[0]*absR.Get02() + bExtents[1]*absR.Get12() + bExtents[2]*absR.Get22();
  absT = CMath::AbsF(translation[2]);
  if (TestAxis(info, 3, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = bExtents[0] + aExtents[0]*absR.Get00() + aExtents[1]*absR.Get10() + aExtents[2]*absR.Get20();
  absT = CMath::AbsF(translation[0]*rot.Get00() + translation[1]*rot.Get10() + translation[2]*rot.Get20());
  if (TestAxis(info, 4, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = bExtents[1] + aExtents[0]*absR.Get01() + aExtents[1]*absR.Get11() + aExtents[2]*absR.Get21();
  absT = CMath::AbsF(translation[0]*rot.Get01() + translation[1]*rot.Get11() + translation[2]*rot.Get21());
  if (TestAxis(info, 5, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = bExtents[2] + aExtents[0]*absR.Get02() + aExtents[1]*absR.Get12() + aExtents[2]*absR.Get22();
  absT = CMath::AbsF(translation[0]*rot.Get02() + translation[1]*rot.Get12() + translation[2]*rot.Get22());
  if (TestAxis(info, 6, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[1]*absR.Get20() + aExtents[2]*absR.Get10() + bExtents[1]*absR.Get02() + bExtents[2]*absR.Get01();
  absT = CMath::AbsF(translation[2]*rot.Get10() - translation[1]*rot.Get20());
  if (TestAxis(info, 7, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[2]*absR.Get00() + aExtents[0]*absR.Get20() + bExtents[1]*absR.Get12() + bExtents[2]*absR.Get11();
  absT = CMath::AbsF(translation[0]*rot.Get20() - translation[2]*rot.Get00());
  if (TestAxis(info, 8, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[0]*absR.Get10() + aExtents[1]*absR.Get00() + bExtents[1]*absR.Get22() + bExtents[2]*absR.Get21();
  absT = CMath::AbsF(translation[1]*rot.Get00() - translation[0]*rot.Get10());
  if (TestAxis(info, 9, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[1]*absR.Get21() + aExtents[2]*absR.Get11() + bExtents[2]*absR.Get00() + bExtents[0]*absR.Get02();
  absT = CMath::AbsF(translation[2]*rot.Get11() - translation[1]*rot.Get21());
  if (TestAxis(info, 10, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[2]*absR.Get01() + aExtents[0]*absR.Get21() + bExtents[2]*absR.Get10() + bExtents[0]*absR.Get12();
  absT = CMath::AbsF(translation[0]*rot.Get21() - translation[2]*rot.Get01());
  if (TestAxis(info, 11, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[0]*absR.Get11() + aExtents[1]*absR.Get01() + bExtents[2]*absR.Get20() + bExtents[0]*absR.Get22();
  absT = CMath::AbsF(translation[1]*rot.Get01() - translation[0]*rot.Get11());
  if (TestAxis(info, 12, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[1]*absR.Get22() + aExtents[2]*absR.Get12() + bExtents[0]*absR.Get01() + bExtents[1]*absR.Get00();
  absT = CMath::AbsF(translation[2]*rot.Get12() - translation[1]*rot.Get22());
  if (TestAxis(info, 13, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[2]*absR.Get02() + aExtents[0]*absR.Get22() + bExtents[0]*absR.Get11() + bExtents[1]*absR.Get10();
  absT = CMath::AbsF(translation[0]*rot.Get22() - translation[2]*rot.Get02());
  if (TestAxis(info, 14, absT, sum, wantGap, hit)) {
    return true;
  }
  sum = aExtents[0]*absR.Get12() + aExtents[1]*absR.Get02() + bExtents[0]*absR.Get21() + bExtents[1]*absR.Get20();
  absT = CMath::AbsF(translation[1]*rot.Get02() - translation[0]*rot.Get12());
  if (TestAxis(info, 15, absT, sum, wantGap, hit)) {
    return true;
  }
  return hit;
}

bool COBBox::TestAxis(CSeparationInfo& info, const int& axis, const float& distance,
                      const float& radius, const bool& wantGap, bool& hit) {
  if (distance > radius) {
    if (!wantGap) {
      return true;
    }
    hit = true;
    float gap = distance - radius;
    if (gap < info.mDistance) {
      info.mDistance = gap;
      info.mAxis = axis;
    }
  }
  return false;
}

bool COBBox::OBBIntersectsBox(CSeparationInfo& info, const COBBox& a, const COBBox& b) {
  CVector3f translation = b.mTransform.GetTranslation();
  CMatrix3f rot = b.mTransform.BuildMatrix3f();
  return !SeparatingAxisTest(info, false, a.mExtents, translation, rot, b.mExtents);
}

COBBox::COBBox(const CTransform4f& xf, const CVector3f& extents)
: mTransform(xf), mExtents(extents) {}

COBBox::COBBox(const COBBox& other, const CTransform4f& xf)
: mTransform(xf * other.mTransform), mExtents(other.mExtents) {}

COBBox::COBBox(CInputStream& in) : mTransform(in), mExtents(in) {}

CAABox COBBox::CalculateAABox(const CTransform4f& xf) const {
  const CTransform4f transform = xf * mTransform;
  CAABox bounds = CAABox::MakeMaxInvertedBox();
  const CVector3f& positive = mExtents * 1.f;
  const CVector3f& negative = mExtents * -1.f;

  for (int i = 0; i < 8; ++i) {
    const CVector3f& point = CVector3f(i & 1 ? positive.GetX() : negative.GetX(),
                                       i & 2 ? positive.GetY() : negative.GetY(),
                                       i & 4 ? positive.GetZ() : negative.GetZ());
    bounds.AccumulateBounds(transform * point);
  }

  return bounds;
}

CAABox COBBox::CalculateLocalAABox() const { return CalculateAABox(CTransform4f::Identity()); }

COBBox COBBox::FromAABox(const CAABox& box, const CTransform4f& xf) {
  CVector3f center = box.GetCenterPoint();
  CVector3f extents = box.GetMaxPoint() - center;
  CTransform4f finalTransform = xf * CTransform4f::Translate(center);
  return COBBox(finalTransform, extents);
}

bool COBBox::LineIntersectsBox(const CMRay& ray, float& penetration) const {
  CAABox box(-mExtents, mExtents);
  CMRay unscaledRay = ray.GetInvUnscaledTransformRay(mTransform);
  CVector3f direction = CVector3f::Zero();
  return CollisionUtil::RayAABoxIntersection(unscaledRay, box, direction, penetration) != 0;
}

bool COBBox::LineIntersectsBox(const CMRay& ray, CVector3f& point, float& penetration,
                               CVector3f* normal) const {
  CAABox box(-mExtents, mExtents);
  CMRay unscaledRay = ray.GetInvUnscaledTransformRay(mTransform);
  CVector3f direction = CVector3f::Zero();
  if (CollisionUtil::RayAABoxIntersection(unscaledRay, box, direction, penetration) != 0) {
    point = ray.GetPoint(penetration);
    if (normal != nullptr) {
      *normal = mTransform.Rotate(direction);
    }
    return true;
  }
  return false;
}

CQuad COBBox::GetQuad(CAABox::EBoxFaceId face) const {
  switch (face) {
  case CAABox::kF_YMin: {
    return CQuad(CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())));
  }
  case CAABox::kF_YMax: {
    return CQuad(CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())));
  }
  case CAABox::kF_XMin: {
    return CQuad(CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())));
  }
  case CAABox::kF_XMax: {
    return CQuad(CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())));
  }
  case CAABox::kF_ZMax: {
    return CQuad(CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())));
  }
  case CAABox::kF_ZMin: {
    return CQuad(CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), mExtents.GetY(), -mExtents.GetZ())));
  }
  default: {
    return CQuad(CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())),
                 CVector3f(mTransform * CVector3f(-mExtents.GetX(), -mExtents.GetY(), -mExtents.GetZ())));
  }
  }
}

bool COBBox::IntersectsAABox(const CAABox& box) const {
  return OBBIntersectsBox(FromAABox(box, CTransform4f::Identity()));
}

bool COBBox::OBBIntersectsBox(const COBBox& other) const {
  const CVector3f delta = other.mTransform.GetTranslation() - mTransform.GetTranslation();
  const CVector3f axes[3] = {mTransform.GetColumn(kDX), mTransform.GetColumn(kDY),
                             mTransform.GetColumn(kDZ)};
  const CVector3f otherAxes[3] = {other.mTransform.GetColumn(kDX), other.mTransform.GetColumn(kDY),
                                  other.mTransform.GetColumn(kDZ)};
  const CVector3f translation(CVector3f::Dot(delta, axes[0]), CVector3f::Dot(delta, axes[1]),
                              CVector3f::Dot(delta, axes[2]));
  const CVector3f& extents = mExtents;
  const CVector3f& otherExtents = other.GetSize();
  float rotation[3][3];
  float ra, rb, t;
  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      rotation[i][j] = CVector3f::Dot(axes[i], otherAxes[j]);
    }
  }

  for (int i = 0; i < 3; ++i) {
    ra = extents[i];
    rb = otherExtents[kDX] * CMath::AbsF(rotation[i][0]) +
         otherExtents[kDY] * CMath::AbsF(rotation[i][1]) +
         otherExtents[kDZ] * CMath::AbsF(rotation[i][2]);
    t = CMath::AbsF(translation[i]);
    if (t > ra + rb + FLT_EPSILON) {
      return false;
    }
  }

  for (int j = 0; j < 3; ++j) {
    ra = extents[kDX] * CMath::AbsF(rotation[0][j]) + extents[kDY] * CMath::AbsF(rotation[1][j]) +
         extents[kDZ] * CMath::AbsF(rotation[2][j]);
    rb = otherExtents[j];
    t = CMath::AbsF(translation[0] * rotation[0][j] + translation[1] * rotation[1][j] +
                    translation[2] * rotation[2][j]);
    if (t > ra + rb + FLT_EPSILON) {
      return false;
    }
  }

  // Test the axis A0 x B0.
  ra = extents[kDY] * CMath::AbsF(rotation[2][0]) + extents[kDZ] * CMath::AbsF(rotation[1][0]);
  rb = otherExtents[kDY] * CMath::AbsF(rotation[0][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[0][1]);
  t = CMath::AbsF(translation[2] * rotation[1][0] - translation[1] * rotation[2][0]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A0 x B1.
  ra = extents[kDY] * CMath::AbsF(rotation[2][1]) + extents[kDZ] * CMath::AbsF(rotation[1][1]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[0][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[0][0]);
  t = CMath::AbsF(translation[2] * rotation[1][1] - translation[1] * rotation[2][1]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A0 x B2.
  ra = extents[kDY] * CMath::AbsF(rotation[2][2]) + extents[kDZ] * CMath::AbsF(rotation[1][2]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[0][1]) +
       otherExtents[kDY] * CMath::AbsF(rotation[0][0]);
  t = CMath::AbsF(translation[2] * rotation[1][2] - translation[1] * rotation[2][2]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A1 x B0.
  ra = extents[kDX] * CMath::AbsF(rotation[2][0]) + extents[kDZ] * CMath::AbsF(rotation[0][0]);
  rb = otherExtents[kDY] * CMath::AbsF(rotation[1][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[1][1]);
  t = CMath::AbsF(translation[0] * rotation[2][0] - translation[2] * rotation[0][0]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A1 x B1.
  ra = extents[kDX] * CMath::AbsF(rotation[2][1]) + extents[kDZ] * CMath::AbsF(rotation[0][1]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[1][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[1][0]);
  t = CMath::AbsF(translation[0] * rotation[2][1] - translation[2] * rotation[0][1]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A1 x B2.
  ra = extents[kDX] * CMath::AbsF(rotation[2][2]) + extents[kDZ] * CMath::AbsF(rotation[0][2]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[1][1]) +
       otherExtents[kDY] * CMath::AbsF(rotation[1][0]);
  t = CMath::AbsF(translation[0] * rotation[2][2] - translation[2] * rotation[0][2]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A2 x B0.
  ra = extents[kDX] * CMath::AbsF(rotation[1][0]) + extents[kDY] * CMath::AbsF(rotation[0][0]);
  rb = otherExtents[kDY] * CMath::AbsF(rotation[2][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[2][1]);
  t = CMath::AbsF(translation[1] * rotation[0][0] - translation[0] * rotation[1][0]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A2 x B1.
  ra = extents[kDX] * CMath::AbsF(rotation[1][1]) + extents[kDY] * CMath::AbsF(rotation[0][1]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[2][2]) +
       otherExtents[kDZ] * CMath::AbsF(rotation[2][0]);
  t = CMath::AbsF(translation[1] * rotation[0][1] - translation[0] * rotation[1][1]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  // Test the axis A2 x B2.
  ra = extents[kDX] * CMath::AbsF(rotation[1][2]) + extents[kDY] * CMath::AbsF(rotation[0][2]);
  rb = otherExtents[kDX] * CMath::AbsF(rotation[2][1]) +
       otherExtents[kDY] * CMath::AbsF(rotation[2][0]);
  t = CMath::AbsF(translation[1] * rotation[0][2] - translation[0] * rotation[1][2]);
  if (t > ra + rb + FLT_EPSILON) {
    return false;
  }

  return true;
}
