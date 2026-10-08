/*
 * G2MEAB WorldFormat/CGamePortalAreaData.cpp translation-unit scaffold.
 * .text: 0x805AEF30..0x805B0F74 (71 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Leading overlapping-volumes5AEF30 uses volume stride0x2C and recursive BSP
 * test5AF1E8; factory5AEFE8 validates magicDEAFBEEF with CGamePortalAreaData.cpp257 and
 * allocates0x50 at259. Destructor5AF0E4 and streamctor5AF16C own volumes/portals/two ushort
 * arrays/AABoxNodeTree at+0/+10/+20/+30/+40. Preserve volume/BSP/portal ctor, center/distance
 * routines, AABoxNodeTree recursion5AF6E0 asserting CGamePortalAreaData.cpp75 and reserved
 * leaf-capacity message, wrapper5AF80C/ctor5AF838/node5AF890, eraser5AF908, complete factory
 * owner/token chain5AF980..5AFB00 and all vector construction/destruction/reserve/copy helpers
 * through final Get<vector<node>>5B0F50+24, directly called by tree constructor5AF838. Exact
 * semantic counterpart/full native inventory is Echoes MetroidPrime/CPortalAreaData; Prime has no
 * corresponding standalone file/header/TU, and related area/octree/PVS interfaces were compared.
 * Prototype assertion proves basename; WorldFormat folder/Kyoto library are recommendations from
 * contiguous resource-data placement, not source-map proof. Next5B0F74 asserts
 * CCollisionPrimitiveData.cpp250, separately assigned.
 */

#include "MetroidPrime/CPortalAreaData.hpp"

#include "Collision/CollisionUtil.hpp"
#include "Kyoto/CFactoryMgr.hpp"
#include "Kyoto/Math/CMath.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

CPortalAreaData::SBoundingTreeNode::SBoundingTreeNode(CInputStream& in)
: mBounds(in), mLeft(in.ReadInt16()), mRight(in.ReadInt16()), mVolumeIndex(in.ReadInt16()) {}

CPortalAreaData::CBoundingTree::CBoundingTree(CInputStream& in)
: mNodes(in.Get< rstl::vector< SBoundingTreeNode > >()) {}

void CPortalAreaData::CBoundingTree::FindOverlappingVolumes(
    const CAABox& bounds, rstl::reserved_vector< short, 64 >& volumes) const {
  FindOverlappingVolumes(bounds, volumes, mNodes.size() - 1);
}

void CPortalAreaData::CBoundingTree::FindOverlappingVolumes(
    const CAABox& bounds, rstl::reserved_vector< short, 64 >& volumes, short node) const {
  const SBoundingTreeNode& entry = mNodes[node];
  if (!entry.mBounds.DoBoundsOverlap(bounds)) {
    return;
  }

  if (entry.mVolumeIndex != -1) {
    volumes.push_back(entry.mVolumeIndex);
  } else {
    FindOverlappingVolumes(bounds, volumes, entry.mLeft);
    FindOverlappingVolumes(bounds, volumes, entry.mRight);
  }
}

CPortalAreaData::SPortal::SPortal(CInputStream& in)
: mVertices(in), mPlane(in), mVolumeIndexStart(in.ReadUint16()) {}

float CPortalAreaData::SPortal::DistanceToPoint(const CVector3f& point) const {
  float minDistance = 3.4028235e38f;
  for (int i = 0; i < mVertices.size() - 2; ++i) {
    const float distance = CollisionUtil::TriPointSqrDist_Float(
        point, mVertices[0], mVertices[i + 1], mVertices[i + 2], nullptr, nullptr);
    if (distance < minDistance) {
      minDistance = distance;
    }
  }
  return CMath::SqrtF(minDistance);
}

CVector3f CPortalAreaData::SPortal::GetCenterPoint() const {
  CVector3f center = CVector3f::Zero();
  for (int i = 0; i < mVertices.size(); ++i) {
    center += mVertices[i];
  }
  return (1.f / mVertices.size()) * center;
}

CPortalAreaData::SBspNode::SBspNode(CInputStream& in)
: mPlane(in), mFront(in.ReadInt16()), mBack(in.ReadInt16()) {}

CPortalAreaData::SVolume::SVolume(CInputStream& in)
: mNodes(in), mPortalIndexStart(in.ReadUint16()), mBounds(in) {}

bool CPortalAreaData::SVolume::Intersects(const CAABox& bounds, int node) const {
  const SBspNode& entry = mNodes[node];
  const CVector3f closest = bounds.ClosestPointAlongVector(entry.mPlane.GetNormal());
  const CVector3f furthest = bounds.FurthestPointAlongVector(entry.mPlane.GetNormal());
  if (entry.mPlane.IsFacing(closest)) {
    if (entry.mFront != -1) {
      return Intersects(bounds, entry.mFront);
    }
    return true;
  }
  if (!entry.mPlane.IsFacing(furthest)) {
    if (entry.mBack != -1) {
      return Intersects(bounds, entry.mBack);
    }
    return false;
  }
  if (entry.mFront == -1 || Intersects(bounds, entry.mFront)) {
    return true;
  }
  if (entry.mBack != -1 && Intersects(bounds, entry.mBack)) {
    return true;
  }
  return false;
}

CPortalAreaData::CPortalAreaData(CInputStream& in)
: mVolumes(in), mPortals(in), mPortalIndices(in), mVolumeIndices(in), mVolumeTree(in) {}

CPortalAreaData::~CPortalAreaData() {}

CFactoryFnReturn FPortalAreaDataFactory(const SObjectTag& tag, CInputStream& in,
                                        const CVParamTransfer& xfer) {
  in.ReadInt32();
  return rs_new CPortalAreaData(in);
}

void CPortalAreaData::FindOverlappingVolumes(const CAABox& bounds,
                                             rstl::reserved_vector< short, 64 >& volumes) const {
  mVolumeTree.FindOverlappingVolumes(bounds, volumes);
  if (volumes.size() <= 1) {
    return;
  }

  rstl::reserved_vector< short, 64 >::iterator it = volumes.begin();
  while (it != volumes.end()) {
    if (mVolumes[*it].Intersects(bounds, 0)) {
      ++it;
    } else {
      it = volumes.erase(it);
    }
  }
}
