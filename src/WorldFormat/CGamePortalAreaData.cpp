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

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern unsigned char lbl_806B1B80[12];
extern unsigned char lbl_806B1B8C[12];
extern unsigned char lbl_806E2520[16];
extern "C" void fn_805AF0E4(int, int);
extern "C" int fn_805AFA44(int obj, int val);
extern "C" int fn_805AFA44(int obj, int val) {
    if (obj) {
        *(int*)obj = (int)lbl_806E2520;
        if ((unsigned int)*(int*)((char*)obj + 0x4) != 0) {
            fn_805AF0E4(*(int*)((char*)obj + 0x4), 1);
        }
        if ((unsigned int)obj != 0) {
            *(int*)obj = (int)lbl_806B1B8C;
            if ((unsigned int)obj != 0) {
                *(int*)obj = (int)lbl_806B1B80;
            }
        }
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern "C" void fn_805AFD90(int, int);
extern "C" int fn_805AFD3C(int obj, int val);
extern "C" int fn_805AFD3C(int obj, int val) {
    if (obj) {
        fn_805AFD90(obj, -1);
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern "C" void fn_805AF3D0(unsigned char*, int);
extern "C" void fn_805B0228(int, unsigned char*);
extern "C" void fn_805B05EC(int, int);
extern "C" int fn_805B018C(int obj, int obj2);
extern "C" int fn_805B018C(int obj, int obj2) {
    unsigned char val2[24];
    *(int*)((char*)obj + 0x4) = 0;
    *(int*)((char*)obj + 0x8) = 0;
    *(int*)((char*)obj + 0xc) = 0;
    int val3 = *(int*)((char*)obj2 + 0x8);
    *(int*)((char*)obj2 + 0x8) = val3 + 4;
    int val = *(int*)val3;
    fn_805B05EC(obj, val);
    for (int i = 0; i < val; i++) {
        fn_805AF3D0(val2, obj2);
        fn_805B0228(obj, val2);
    }
    return obj;
}

extern "C" void fn_805AF890(unsigned char*, int);
extern "C" void fn_805B0458(int, unsigned char*);
extern "C" void fn_805B0534(int, int);
extern "C" int fn_805B03BC(int obj, int obj2);
extern "C" int fn_805B03BC(int obj, int obj2) {
    unsigned char val2[40];
    *(int*)((char*)obj + 0x4) = 0;
    *(int*)((char*)obj + 0x8) = 0;
    *(int*)((char*)obj + 0xc) = 0;
    int val3 = *(int*)((char*)obj2 + 0x8);
    *(int*)((char*)obj2 + 0x8) = val3 + 4;
    int val = *(int*)val3;
    fn_805B0534(obj, val);
    for (int i = 0; i < val; i++) {
        fn_805AF890(val2, obj2);
        fn_805B0458(obj, val2);
    }
    return obj;
}

extern "C" void fn_805B08B0();
extern "C" void fn_805B0890();
extern "C" void fn_805B0890() {
    fn_805B08B0();
}

extern "C" void fn_805AFC34(int, int);
extern "C" int fn_805B08D4(int obj, int val);
extern "C" int fn_805B08D4(int obj, int val) {
    if (obj) {
        fn_805AFC34(obj, -1);
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern "C" void fn_805B0960(int obj, int obj2);
extern "C" void fn_805B0960(int obj, int obj2) {
    int i = *(int*)obj;
    while (i != (unsigned int)(*(int*)obj2)) {
        i += 72;
    }
}

extern "C" void fn_805B099C();
extern "C" void fn_805B097C();
extern "C" void fn_805B097C() {
    fn_805B099C();
}

extern "C" void fn_805B0A70();
extern "C" void fn_805B0A48(int val);
extern "C" void fn_805B0A48(int val) {
    if ((unsigned int)val != 0) {
        fn_805B0A70();
    }
}

extern "C" int fn_805B0AE4(int obj, int val, int obj2);
extern "C" int fn_805B0AE4(int obj, int val, int obj2) {
    for (int i = 0; i != val; i++) {
        if ((unsigned int)obj2 > 0) {
            *(float*)obj2 = *(float*)obj;
            char* ptr = (char*)obj;
            char* ptr2 = (char*)obj2;
            *(float*)(ptr2 + 0x4) = *(float*)(ptr + 0x4);
            *(float*)(ptr2 + 0x8) = *(float*)(ptr + 0x8);
            *(float*)(ptr2 + 0xc) = *(float*)(ptr + 0xc);
            *(unsigned short*)(ptr2 + 0x10) = *(short*)(ptr + 0x10);
            *(unsigned short*)(ptr2 + 0x12) = *(short*)(ptr + 0x12);
        }
        obj += 20;
        obj2 += 20;
    }
    return obj2;
}

extern "C" void fn_805B0BA4(int, int);
extern "C" int fn_805B0B3C(int val, int val2, int val3);
extern "C" int fn_805B0B3C(int val, int val2, int val3) {
    int val4 = val;
    int result = val3;
    while (val2) {
        fn_805B0BA4(result, val4);
        val2--;
        val4 += 32;
        result += 32;
    }
    return result;
}

extern "C" void fn_805B0BEC();
extern "C" void fn_805B0BC4(int val);
extern "C" void fn_805B0BC4(int val) {
    if ((unsigned int)val != 0) {
        fn_805B0BEC();
    }
}

extern "C" void fn_805B0E38();
extern "C" void fn_805B0E18();
extern "C" void fn_805B0E18() {
    fn_805B0E38();
}

extern "C" void fn_805B0EE8();
extern "C" void fn_805B0EC8();
extern "C" void fn_805B0EC8() {
    fn_805B0EE8();
}

extern "C" void fn_805AF684();
extern "C" void fn_805B0F10();
extern "C" void fn_805B0F10() {
    fn_805AF684();
}

extern "C" void fn_805AF370();
extern "C" void fn_805B0F30();
extern "C" void fn_805B0F30() {
    fn_805AF370();
}

