/*
 * G2MEAB Collision/CMRay.cpp translation-unit scaffold.
 * .text: 0x8047E58C..0x8047E860 (4
 * native functions, including emitted helpers).
 * NonMatching: implementation has not been
 * reconstructed.
 * Boundary evidence: Four natives: GetInvUnscaledTransformRay7E58C
 * transpose-rotates point and
 * direction then calls endpoint/length constructor7E6FC; direction
 * constructor7E664,
 * endpoint/length7E6FC and endpoint-only7E790+ D0. Both reference
 * families/interfaces checked
 * (Prime emits three, Echoes four). Next7E860 is an asserted
 * resource factory for another class.
 */
#include "Collision/CMRay.hpp"
#include "Kyoto/Math/CTransform4f.hpp"

CMRay::CMRay(const CVector3f& start, const CVector3f& end)
: mStart(start)
, mEnd(end)
, mDelta(mEnd - mStart)
, mLength(mDelta.Magnitude())
, mInvLength(1.f / mLength)
, mDirection(mInvLength * mDelta) {}

CMRay::CMRay(const CVector3f& start, const CVector3f& end, float length, float invLength)
: mStart(start)
, mEnd(end)
, mDelta(mEnd - mStart)
, mLength(length)
, mInvLength(invLength)
, mDirection(mInvLength * mDelta) {}

CMRay::CMRay(const CVector3f& start, const CVector3f& dir, float length)
: mStart(start)
, mEnd(start + length * dir)
, mDelta(mEnd - mStart)
, mLength(length)
, mInvLength(1.f / length)
, mDirection(dir) {}

CMRay CMRay::GetInvUnscaledTransformRay(const CTransform4f& xf) const {
  CTransform4f invXf = xf.GetQuickInverse();
  return CMRay(invXf * mStart, invXf * mEnd, mLength, mInvLength);
}
