#ifndef _CGJKSOLVER
#define _CGJKSOLVER

#include "types.h"

#include "Kyoto/Math/CTransform4f.hpp"
#include "Kyoto/Math/CVector3d.hpp"
#include "rstl/reserved_vector.hpp"

class CCollisionPrimitive;

// Guessed name: a Gilbert-Johnson-Keerthi closest-point solver over two support-mapped primitives.
class CGjkSolver {
public:
  CGjkSolver();

  // Computes the closest points of two convex primitives placed with the given transforms.
  void ClosestPoints(const CCollisionPrimitive& a, const CTransform4f& xfA,
                     const CCollisionPrimitive& b, const CTransform4f& xfB, CVector3d& pointA,
                     CVector3d& pointB);

private:
  bool Degenerate(const CVector3d& w);
  bool Closest(CVector3d& v);
  bool Valid(int subset);
  void ComputeDet();
  void ComputeVector(int subset, CVector3d& v) const;
  void ComputePoints(int subset, CVector3d& p1, CVector3d& p2) const;

  double mRelError;
  double mTolError;
  double mSqrTolError;
  rstl::reserved_vector< CVector3d, 4 > mP;
  rstl::reserved_vector< CVector3d, 4 > mQ;
  rstl::reserved_vector< CVector3d, 4 > mY;
  int mBits;
  int mLast;
  int mLastBit;
  int mAllBits;
  double mDet[16][4];
  double mDp[4][4];
  CVector3d mW;
  CVector3d mUnk3F0;
  CVector3d mUnk408;
  CVector3d mV;
  CVector3d mNegV;
  CVector3d mPointA;
  CVector3d mPointB;
};
CHECK_SIZEOF(CGjkSolver, 0x480)

#endif // _CGJKSOLVER
