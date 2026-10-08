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

extern "C" unsigned char CMath_IsWithinTolerance(float, float, float);
extern "C" bool fn_805B548C(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5);
// near match: 92.4%
extern "C" bool fn_805B548C(int arg0, int arg1, int arg2, int arg3, int arg4, int arg5) {
    int temp_r31;
    int var_r30;
    int var_r29;
    float temp_f31;
    float temp_f30;
    float temp_f29;
    float temp_f28;
    float temp_f27;
    float temp_f26;
    float temp_f25;
    float temp_f24;
    float temp_f0;
    float temp_f0_2;
    float temp_f0_3;
    float temp_f1;
    float temp_f1_2;
    float temp_f1_3;
    float temp_f1_4;
    float temp_f5;
    float temp_f6;
    *(float*)arg2 = -3.4028235e38f;
    temp_f26 = *(float*)((char*)arg1 + 0x34);
    temp_f27 = *(float*)(((char*)arg1) + 0x2c);
    var_r30 = arg0 + 4;
    *(float*)arg4 = 3.4028235e38;
    var_r29 = 0;
    temp_f28 = *(float*)(((char*)arg1) + 0x30);
    temp_f30 = *(float*)arg1;
    temp_f29 = *(float*)(((char*)arg1) + 0x8);
    temp_f31 = *(float*)((char*)arg1 + 0x4);
    temp_r31 = *(int*)arg0;
    while (var_r29 < temp_r31) {
        float* temp_0 = (float*)((char*)var_r30 + 0x4);
        temp_f0 = *temp_0;
        temp_f5 = *(float*)var_r30;
        temp_f6 = *(float*)((char*)var_r30 + 0x8);
        temp_f25 = temp_f6 * temp_f26 + (temp_f5 * temp_f27 + temp_f0 * temp_f28);
        temp_f24 = temp_f6 * temp_f29 + (temp_f5 * temp_f30 + temp_f0 * temp_f31) - *(float*)((char*)var_r30 + 0xc);
        if (CMath_IsWithinTolerance(temp_f25, 0.0f, 1.1920929e-7f) != 0) {
            if (temp_f24 > 0.0f) {
                return 0;
            }
        } else {
            temp_f1_2 = -temp_f24 / temp_f25;
            if (temp_f25 > 0.0f) {
                if (temp_f1_2 < *(float*)arg4) {
                    if (temp_f1_2 < *(float*)arg2) {
                        return 0;
                    }
                    *(float*)arg4 = temp_f1_2;
                    temp_f1_3 = *temp_0;
                    *(float*)arg5 = *(float*)var_r30;
                    temp_f0_2 = *(float*)(0x8 + (char*)(var_r30));
                    *(float*)((char*)arg5 + 0x4) = temp_f1_3;
                    *(float*)((char*)arg5 + 0x8) = temp_f0_2;
                }
            } else if (temp_f1_2 > *(float*)arg2) {
                *(float*)arg2 = temp_f1_2;
                temp_f1_4 = *temp_0;
                *(float*)arg3 = *(float*)var_r30;
                temp_f0_3 = *(float*)((char*)var_r30 + 0x8);
                *(float*)((char*)arg3 + 0x4) = temp_f1_4;
                *(float*)((char*)arg3 + 0x8) = temp_f0_3;
            }
            if (*(float*)arg2 > *(float*)arg4) {
                return 0;
            }
        }
        var_r30 += 16;
        var_r29 += 1;
    }
    temp_f1 = *(float*)((char*)arg1 + 0x28);
    *(float*)arg2 *= temp_f1;
    *(float*)arg4 *= temp_f1;
    return true;
}

