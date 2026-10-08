/*
 * G2MEAB Collision/CGjkSolver.cpp
 * .text: 0x80480884..0x804816E4 (eight retained native functions).
 * Inferred descriptive basename; original class/source name is unproven. A support-mapped GJK
 * closest-point solver (simplex determinant bookkeeping in the style of the SOLID library),
 * called by the oriented-box contact function.
 */
// The retail binary calls sqrt out of line here.
#define MSL_NO_INLINE_SQRT
#include "Collision/CGjkSolver.hpp"

#include "Collision/CCollisionPrimitive.hpp"
#include "Kyoto/Math/CMath.hpp"

#include <float.h>
#include <string.h>

CGjkSolver::CGjkSolver()
: mRelError(1e-4f)
, mTolError(FLT_EPSILON)
, mSqrTolError(FLT_EPSILON * FLT_EPSILON)
, mP(4, CVector3d::Zero())
, mQ(4, CVector3d::Zero())
, mY(4, CVector3d::Zero())
, mW(CVector3d::Zero())
, mUnk3F0(CVector3d::Zero())
, mUnk408(CVector3d::Zero())
, mV(CVector3d::Zero())
, mNegV(CVector3d::Zero())
, mPointA(CVector3d::Zero())
, mPointB(CVector3d::Zero()) {
  memset(mDet, 0xED, sizeof(mDet));
}

bool CGjkSolver::Degenerate(const CVector3d& w) {
  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if ((mAllBits & bit) && mY[i] == w) {
      return true;
    }
  }
  return false;
}

bool CGjkSolver::Closest(CVector3d& v) {
  ComputeDet();
  for (int s = mBits; s; --s) {
    if (s == (s & mBits) && Valid(s | mLastBit)) {
      mBits = s | mLastBit;
      ComputeVector(mBits, v);
      return true;
    }
  }
  if (Valid(mLastBit)) {
    mBits = mLastBit;
    v = mY[mLast];
    return true;
  }
  return false;
}

void CGjkSolver::ComputePoints(int subset, CVector3d& p1, CVector3d& p2) const {
  double sum = 0.0;
  p1 = CVector3d::Zero();
  p2 = CVector3d::Zero();
  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if (subset & bit) {
      sum += mDet[subset][i];
      p1 += mP[i] * mDet[subset][i];
      p2 += mQ[i] * mDet[subset][i];
    }
  }
  double scale = 1.0 / sum;
  p1 *= scale;
  p2 *= scale;
}

void CGjkSolver::ComputeVector(int subset, CVector3d& v) const {
  double sum = 0.0;
  v = CVector3d::Zero();
  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if (subset & bit) {
      double det = mDet[subset][i];
      sum += det;
      v += mY[i] * det;
    }
  }
  v *= 1.0 / sum;
}

bool CGjkSolver::Valid(int subset) {
  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if (mAllBits & bit) {
      if (subset & bit) {
        if (mDet[subset][i] <= 0.0) {
          return false;
        }
      } else if (mDet[subset | bit][i] > 0.0) {
        return false;
      }
    }
  }
  return true;
}

void CGjkSolver::ComputeDet() {
  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if (mBits & bit) {
      mDp[i][mLast] = mDp[mLast][i] = CVector3d::Dot(mY[i], mY[mLast]);
    }
  }
  mDp[mLast][mLast] = CVector3d::Dot(mY[mLast], mY[mLast]);
  mDet[mLastBit][mLast] = 1.0;

  for (int i = 0, bit = 1; i < 4; ++i, bit <<= 1) {
    if (mBits & bit) {
      int s2 = bit | mLastBit;
      mDet[s2][i] = mDp[mLast][mLast] - mDp[mLast][i];
      mDet[s2][mLast] = mDp[i][i] - mDp[i][mLast];
      for (int j = 0, bit2 = 1; j < i; ++j, bit2 <<= 1) {
        if (mBits & bit2) {
          int s3 = bit2 | s2;
          mDet[s3][j] = mDet[s2][i] * (mDp[i][i] - mDp[i][j]) +
                        mDet[s2][mLast] * (mDp[mLast][i] - mDp[mLast][j]);
          mDet[s3][i] = mDet[bit2 | mLastBit][j] * (mDp[j][j] - mDp[j][i]) +
                        mDet[bit2 | mLastBit][mLast] * (mDp[mLast][j] - mDp[mLast][i]);
          mDet[s3][mLast] = mDet[bit2 | bit][j] * (mDp[j][j] - mDp[j][mLast]) +
                            mDet[bit2 | bit][i] * (mDp[i][j] - mDp[i][mLast]);
        }
      }
    }
  }

  if (mAllBits == 15) {
    mDet[15][0] = mDet[14][1] * (mDp[1][1] - mDp[1][0]) + mDet[14][2] * (mDp[2][1] - mDp[2][0]) +
                  mDet[14][3] * (mDp[3][1] - mDp[3][0]);
    mDet[15][1] = mDet[13][0] * (mDp[0][0] - mDp[0][1]) + mDet[13][2] * (mDp[2][0] - mDp[2][1]) +
                  mDet[13][3] * (mDp[3][0] - mDp[3][1]);
    mDet[15][2] = mDet[11][0] * (mDp[0][0] - mDp[0][2]) + mDet[11][1] * (mDp[1][0] - mDp[1][2]) +
                  mDet[11][3] * (mDp[3][0] - mDp[3][2]);
    mDet[15][3] = mDet[7][0] * (mDp[0][0] - mDp[0][3]) + mDet[7][1] * (mDp[1][0] - mDp[1][3]) +
                  mDet[7][2] * (mDp[2][0] - mDp[2][3]);
  }
}

void CGjkSolver::ClosestPoints(const CCollisionPrimitive& a, const CTransform4f& xfA,
                               const CCollisionPrimitive& b, const CTransform4f& xfB,
                               CVector3d& pointA, CVector3d& pointB) {
  const CTransform4f invA = xfA.GetQuickInverse();
  const CTransform4f invB = xfB.GetQuickInverse();
  CVector3f dir(0.f, 1.f, 0.f);
  mPointA = CVector3d(xfA * a.GetSupportPoint(invA * -dir));
  mPointB = CVector3d(xfB * b.GetSupportPoint(invB * dir));
  mV = mPointA - mPointB;

  double mu = 3.4028234663852886e+38;
  double maxDist = 0.0;
  mBits = 0;
  mAllBits = 0;
  while (mBits < 15 && mu > mTolError) {
    mLast = 0;
    mLastBit = 1;
    while (mBits & mLastBit) {
      ++mLast;
      mLastBit <<= 1;
    }
    mNegV = -mV;
    mP[mLast] = CVector3d(xfA * a.GetSupportPoint(invA * -mV.AsCVector3f()));
    mQ[mLast] = CVector3d(xfB * b.GetSupportPoint(invB * mV.AsCVector3f()));
    mW = mP[mLast] - mQ[mLast];
    double dist = CVector3d::Dot(mV, mW) / mu;
    dist = maxDist < dist ? dist : maxDist;
    maxDist = dist;
    if (mu - dist <= mu * mRelError) {
      break;
    }
    if (Degenerate(mW)) {
      break;
    }
    mY[mLast] = mW;
    mAllBits = mBits | mLastBit;
    if (!Closest(mV)) {
      break;
    }
    mu = sqrt(CVector3d::Dot(mV, mV));
  }
  ComputePoints(mBits, pointA, pointB);
}
