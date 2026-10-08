#ifndef _CCOLLIDABLEORIENTEDBOX
#define _CCOLLIDABLEORIENTEDBOX

#include "types.h"

#include "Collision/COBBox.hpp"
#include "Collision/CCollisionPrimitive.hpp"

// Guessed name: an oriented-box collision primitive (type 'ORBX') used with the GJK solver.
class CCollidableOrientedBox : public CCollisionPrimitive {
public:
  CCollidableOrientedBox(const COBBox& box, const CMaterialList& material);

  // CCollisionPrimitive
  uint GetTableIndex() const override;
  CAABox CalculateAABox(const CTransform4f& xf) const override;
  CAABox CalculateLocalAABox() const override;
  FourCC GetPrimType() const override;
  CVector3f GetSupportPoint(const CVector3f& dir) const override;
  CRayCastResult CastRayInternal(const CInternalRayCastStructure& ray) const override;
  ~CCollidableOrientedBox() override;

  const COBBox& GetBox() const { return mBox; }

  static void SetStaticTableIndex(uint idx);
  static CCollisionPrimitive::Type GetType();

private:
  static uint sTableIndex;

  COBBox mBox;
};
CHECK_SIZEOF(CCollidableOrientedBox, 0x50)

namespace Collide {
bool OBBox_OBBox_Bool(const CInternalCollisionStructure& collision);
bool OBBox_OBBox(const CInternalCollisionStructure& collision, CCollisionInfoList& list);
} // namespace Collide

#endif // _CCOLLIDABLEORIENTEDBOX
