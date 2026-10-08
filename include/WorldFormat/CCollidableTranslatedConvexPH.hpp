#ifndef _CCOLLIDABLETRANSLATEDCONVEXPH
#define _CCOLLIDABLETRANSLATEDCONVEXPH

#include "types.h"

#include "Collision/CCollisionPrimitive.hpp"
#include "Kyoto/Math/CPlane.hpp"
#include "WorldFormat/COBBTree.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/vector.hpp"

class CMRay;

// Convex polyhedron collision primitive that is only translated (no rotation) by its transform.
// The registered type name is CCollidableTranslatedConvexPH; the nested data class name and the
// non-registered member names are provisional.
class CCollidableTranslatedConvexPH : public CCollisionPrimitive {
public:
  // Planes and coplanar edge lists derived from an OBB tree's triangles.
  class CPolyhedronData {
  public:
    explicit CPolyhedronData(COBBTree* tree);
    ~CPolyhedronData();

    COBBTree* GetTree();
    COBBTree* GetTree() const;
    bool PointInPlanes(const CVector3f& point) const;
    bool IsPointInside(const CVector3f& point, const CTransform4f& xf) const;
    const ushort* GetPlaneEdgeIndices(int plane, ushort& count) const;

  private:
    void BuildPlanes();
    void CollectEdges(ushort triangle);

    rstl::reserved_vector< CPlane, 32 > mPlanes;
    rstl::vector< ushort > mEdgeIndices;
    int mPlaneEdgeCount;
    ushort mPlaneEdgeStart[32];
    rstl::single_ptr< COBBTree > mOwnedTree;
    COBBTree* mTree;
  };

  CCollidableTranslatedConvexPH(CPolyhedronData* data, const CMaterialList& material);

  // CCollisionPrimitive
  uint GetTableIndex() const override;
  CAABox CalculateAABox(const CTransform4f& xf) const override;
  CAABox CalculateLocalAABox() const override;
  FourCC GetPrimType() const override;
  ~CCollidableTranslatedConvexPH() override;
  CRayCastResult CastRayInternal(const CInternalRayCastStructure& rayCast) const override;

  static void SetStaticTableIndex(uint idx);
  static CCollisionPrimitive::Type GetType();

private:
  rstl::single_ptr< CPolyhedronData > mOwnedData;
  CPolyhedronData* mData;

  static uint sTableIndex;
};
CHECK_SIZEOF(CCollidableTranslatedConvexPH, 0x18)

#endif // _CCOLLIDABLETRANSLATEDCONVEXPH
