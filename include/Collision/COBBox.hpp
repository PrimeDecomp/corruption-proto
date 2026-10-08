#ifndef _COBBOX
#define _COBBOX

#include "Kyoto/Math/CAABox.hpp"
#include "Kyoto/Math/CTransform4f.hpp"

class CInputStream;
class CMRay;
class CMatrix3f;

class COBBox {
public:
  COBBox(const CTransform4f& xf, const CVector3f& extents);
  // Guessed signature (target-derived): transforms an existing box by xf.
  COBBox(const COBBox& other, const CTransform4f& xf);
  explicit COBBox(CInputStream& in);

  CAABox CalculateAABox(const CTransform4f& xf) const;
  CAABox CalculateLocalAABox() const;
  static COBBox FromAABox(const CAABox& box, const CTransform4f& xf);
  bool OBBIntersectsBox(const COBBox& other) const;
  bool IntersectsAABox(const CAABox& box) const;
  // Guessed name; uses the same face ordering as CAABox.
  CQuad GetQuad(CAABox::EBoxFaceId face) const;
  // Guessed overload for the query with optional world-space normal output.
  bool LineIntersectsBox(const CMRay& ray, CVector3f& point, float& penetration,
                         CVector3f* normal) const;
  bool LineIntersectsBox(const CMRay& ray, float& penetration) const;
  // Guessed names (target-derived): 15-axis separation test between a box at the origin and
  // a box given by translation + rotation, recording the smallest separating gap.
  struct CSeparationInfo {
    int mAxis;
    float mDistance;
  };
  static bool OBBIntersectsBox(CSeparationInfo& info, const COBBox& a, const COBBox& b);
  const CTransform4f& GetTransform() const { return mTransform; }
  const CVector3f& GetSize() const { return mExtents; }

private:
  static bool SeparatingAxisTest(CSeparationInfo& info, const bool& wantGap,
                                 const CVector3f& aExtents, const CVector3f& translation,
                                 const CMatrix3f& rot, const CVector3f& bExtents);
  static bool TestAxis(CSeparationInfo& info, const int& axis, const float& distance,
                       const float& radius, const bool& wantGap, bool& hit);

  CTransform4f mTransform;
  CVector3f mExtents;
};
CHECK_SIZEOF(COBBox, 0x3c)

#endif // _COBBOX
