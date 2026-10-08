/*
 * G2MEAB WorldFormat/CCollidableTranslatedConvexPH.cpp translation-unit scaffold.
 * .text: 0x805B36EC..0x805B5D10 (30 native functions, including emitted helpers).
 * Boundary evidence: Complete30-native translated-convex/plane-polyhedron family starts
 * CastRay5B36EC after TriangleCollisionCache final allocator. Collision and boolean
 * wrappers5B38B8..5B472C, primitive type5B49A8, local/world bounds5B49B4/5B4A18, destructor5B4AB0
 * and constructor5B4B28 share vtable806E2570 and baseCollisionPrimitive; constructor stores
 * nonowned data+14 and owned pointer+10. GetType5B4B80 literally registers
 * CCollidableTranslatedConvexPH with setter5B5D00. Preserve underlying convex/convex test5B4BA0,
 * boolean5B5248, segment-plane clipping5B548C, point/transform tests5B5698/5B56F8, plane-edge
 * lookup5B5788, data getters5B57D8/5B57E0, recursively collected edge indices5B57E8 with three
 * vector.h482 assertions, build planes5B5B30, data destructor5B5C44/constructor5B5CB0, setter5B5D00
 * and table getter5B5D08+8. Data builder uses tree surfaces/triangle edges, plane array and
 * ushortvector; allocator/vector helpers called elsewhere remain external. Next5B5D10 is an
 * adjustor thunk subtracting0x20 and branching to5C2318, unrelated to this primitive family. No
 * same named counterpart in Prime/Echoes; both primitive/OBBTree/collider source/header/native
 * inventories checked. Filename inferred from individually inspected literal, not from generic
 * PointInPlanes fingerprint.
 */

#include "WorldFormat/CCollidableTranslatedConvexPH.hpp"

#include "Collision/CCollisionInfo.hpp"
#include "Collision/CInternalRayCastStructure.hpp"
#include "Collision/CRayCastResult.hpp"
#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Basics/CBasics.hpp"
#include "WorldFormat/CCollidableOBBTree.hpp"
#include "WorldFormat/CCollisionSurface.hpp"
#include "WorldFormat/CMetroidAreaCollider.hpp"

namespace {
// Edge material flag marking an edge shared with a coplanar neighbouring triangle.
const u64 kCoplanarEdge = 0x01000000;
} // namespace

uint CCollidableTranslatedConvexPH::sTableIndex = -1;

CCollidableTranslatedConvexPH::CCollidableTranslatedConvexPH(CPolyhedronData* data,
                                                             const CMaterialList& material)
: CCollisionPrimitive(material), mOwnedData(), mData(data) {}

CCollidableTranslatedConvexPH::~CCollidableTranslatedConvexPH() {}

uint CCollidableTranslatedConvexPH::GetTableIndex() const { return sTableIndex; }

FourCC CCollidableTranslatedConvexPH::GetPrimType() const { return 'TCPH'; }

CAABox CCollidableTranslatedConvexPH::CalculateLocalAABox() const {
  CCollidableOBBTree tree(mData->GetTree(), GetMaterial());
  return tree.CalculateLocalAABox();
}

CAABox CCollidableTranslatedConvexPH::CalculateAABox(const CTransform4f& xf) const {
  CCollidableOBBTree tree(mData->GetTree(), GetMaterial());
  return tree.CalculateAABox(CTransform4f::Translate(xf.GetTranslation()));
}

void CCollidableTranslatedConvexPH::SetStaticTableIndex(uint idx) { sTableIndex = idx; }

CCollisionPrimitive::Type CCollidableTranslatedConvexPH::GetType() {
  return Type(SetStaticTableIndex, "CCollidableTranslatedConvexPH");
}

bool CCollidableTranslatedConvexPH::CPolyhedronData::PointInPlanes(const CVector3f& point) const {
  for (int i = 0; i < mPlanes.size(); ++i) {
    if (mPlanes[i].IsFacing(point)) {
      return false;
    }
  }
  return true;
}

bool CCollidableTranslatedConvexPH::CPolyhedronData::IsPointInside(const CVector3f& point,
                                                                   const CTransform4f& xf) const {
  CVector3f local = xf.TransposeRotate(point - xf.GetTranslation());
  return PointInPlanes(local);
}

const ushort* CCollidableTranslatedConvexPH::CPolyhedronData::GetPlaneEdgeIndices(
    int plane, ushort& count) const {
  ushort start = mPlaneEdgeStart[plane];
  if (plane == mPlanes.size() - 1) {
    count = mEdgeIndices.size() - start;
  } else {
    count = mPlaneEdgeStart[plane + 1] - start;
  }
  return mEdgeIndices.data() + start;
}

COBBTree* CCollidableTranslatedConvexPH::CPolyhedronData::GetTree() { return mTree; }

COBBTree* CCollidableTranslatedConvexPH::CPolyhedronData::GetTree() const { return mTree; }

void CCollidableTranslatedConvexPH::CPolyhedronData::CollectEdges(ushort triangle) {
  if (CMetroidAreaCollider::DupTriangleListValue(triangle) ==
      CMetroidAreaCollider::GetDupPrimitiveCheckCount()) {
    return;
  }
  CMetroidAreaCollider::DupTriangleListValue(triangle) =
      CMetroidAreaCollider::GetDupPrimitiveCheckCount();

  const ushort* edges = mTree->GetTriangleEdgeIndices(triangle);
  const ushort* adjacent = mTree->GetTriangleAdjacentIndices(triangle);
  ushort edge = edges[0];
  if (mTree->GetEdgeMaterial(edge) & kCoplanarEdge) {
    CollectEdges(adjacent[0]);
  } else {
    mEdgeIndices.push_back_unsafe(edge);
  }
  edge = edges[1];
  if (mTree->GetEdgeMaterial(edge) & kCoplanarEdge) {
    CollectEdges(adjacent[1]);
  } else {
    mEdgeIndices.push_back_unsafe(edge);
  }
  edge = edges[2];
  if (mTree->GetEdgeMaterial(edge) & kCoplanarEdge) {
    CollectEdges(adjacent[2]);
  } else {
    mEdgeIndices.push_back_unsafe(edge);
  }
}

CCollidableTranslatedConvexPH::CPolyhedronData::CPolyhedronData(COBBTree* tree)
: mPlanes(), mEdgeIndices(), mPlaneEdgeCount(0), mOwnedTree(), mTree(tree) {
  BuildPlanes();
}

CCollidableTranslatedConvexPH::CPolyhedronData::~CPolyhedronData() {}

void CCollidableTranslatedConvexPH::CPolyhedronData::BuildPlanes() {
  mEdgeIndices.reserve(mTree->GetTriangleCount() * 3);
  CMetroidAreaCollider::ResetInternalCounters();
  uchar mark = CMetroidAreaCollider::GetDupPrimitiveCheckCount();
  for (ushort i = 0; i < mTree->GetTriangleCount(); ++i) {
    if (CMetroidAreaCollider::DupTriangleListValue(i) != mark) {
      CPlane plane = mTree->GetTriangle(i).GetPlane();
      mPlanes.push_back(plane);
      mPlaneEdgeStart[mPlaneEdgeCount++] = mEdgeIndices.size();
      CollectEdges(i);
    }
  }
}
