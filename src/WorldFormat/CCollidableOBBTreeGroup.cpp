/*
 * G2MEAB WorldFormat/CCollidableOBBTreeGroup.cpp translation-unit scaffold.
 * .text: 0x805ABBE4..0x805AE030 (36 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Leading group table getter5ABBE4 and primitive type/bounds, full
 * cache/moving/boolean/contact collision sequence, GetOBBTree5ACF28, owned/nonowned group
 * constructors5ACF40/5ACFA4, owner destruction/type and COBBTreeGroup
 * builders5AD108/5AD43C/stream5AD710 (CCollidableOBBTreeGroup.cpp40 allocation) form one TU as in
 * both full reference inventories. Preserve factory5ADA50 and table accessors then all
 * vector/auto_ptr/token wrappers through NewDerived5ADF80+B0. Group container and factory helpers
 * share target vtables806E24D0/806E24F4; do not split at reference count or last class method.
 * Next5AE030 is a PVS lookup belonging to another family.
 */

#include "WorldFormat/CCollidableOBBTreeGroup.hpp"

#include "Collision/CCollidableSphere.hpp"
#include "Collision/CCollisionInfoList.hpp"
#include "Collision/CInternalRayCastStructure.hpp"
#include "Collision/CRayCastResult.hpp"
#include "Collision/CollisionUtil.hpp"
#include "Kyoto/CFactoryMgr.hpp"
#include "WorldFormat/CCollidableOBBTree.hpp"
#include "WorldFormat/CCollisionCache.hpp"

uint CCollidableOBBTreeGroup::sTableIndex = -1;

CFactoryFnReturn FCollidableOBBTreeGroupFactory(const SObjectTag& tag, CInputStream& in,
                                                const CVParamTransfer& xfer) {
  return rs_new COBBTreeGroup(in);
}

COBBTreeGroup::COBBTreeGroup(CInputStream& in) : mAabox(CAABox::MakeMaxInvertedBox()) {
  int obbCount = in.ReadInt32();
  mTrees.reserve(obbCount);
  for (uint i = 0; i < obbCount; ++i) {
    mTrees.push_back(rs_new COBBTree(in));
  }

  mAabbs.reserve(mTrees.size());
  for (rstl::vector< rstl::auto_ptr< COBBTree > >::iterator it = mTrees.begin(); it != mTrees.end();
       ++it) {
    CCollidableOBBTree tree(it->get(), CMaterialList());
    CAABox box = tree.CalculateLocalAABox();
    mAabbs.push_back(box);
    mAabox.AccumulateBounds(box.GetMinPoint());
    mAabox.AccumulateBounds(box.GetMaxPoint());
  }
}

COBBTreeGroup::COBBTreeGroup(rstl::auto_ptr< COBBTree >& tree)
: mAabox(CAABox::MakeMaxInvertedBox()) {
  mTrees.reserve(1);
  mTrees.push_back(tree);
  mAabbs.reserve(1);
  for (rstl::vector< rstl::auto_ptr< COBBTree > >::iterator it = mTrees.begin(); it != mTrees.end();
       ++it) {
    CCollidableOBBTree tree(it->get(), CMaterialList());
    CAABox box = tree.CalculateLocalAABox();
    mAabbs.push_back(box);
    mAabox.AccumulateBounds(box.GetMinPoint());
    mAabox.AccumulateBounds(box.GetMaxPoint());
  }
}

COBBTreeGroup::COBBTreeGroup(const CVector3f& extent, const CVector3f& center)
: mAabox(CAABox::MakeMaxInvertedBox()) {
  mTrees.reserve(1);
  mTrees.push_back(COBBTree::BuildOrientedBoundingBoxTree(extent, center));
  mAabbs.reserve(1);
  for (rstl::vector< rstl::auto_ptr< COBBTree > >::iterator it = mTrees.begin(); it != mTrees.end();
       ++it) {
    CCollidableOBBTree tree(it->get(), CMaterialList());
    CAABox box = tree.CalculateLocalAABox();
    mAabbs.push_back(box);
    mAabox.AccumulateBounds(box.GetMinPoint());
    mAabox.AccumulateBounds(box.GetMaxPoint());
  }
}

CCollidableOBBTreeGroup::~CCollidableOBBTreeGroup() {}

void CCollidableOBBTreeGroup::SetStaticTableIndex(uint index) { sTableIndex = index; }

CCollisionPrimitive::Type CCollidableOBBTreeGroup::GetType() {
  return Type(SetStaticTableIndex, "CCollidableOBBTreeGroup");
}

CCollidableOBBTreeGroup::CCollidableOBBTreeGroup(const COBBTreeGroup* container,
                                                 const CMaterialList& material)
: CCollisionPrimitive(material), mOwnedContainer(), mContainer(container) {}

CCollidableOBBTreeGroup::CCollidableOBBTreeGroup(COBBTreeGroup* container,
                                                 const CMaterialList& material)
: CCollisionPrimitive(material), mOwnedContainer(container), mContainer(container) {}

COBBTree* CCollidableOBBTreeGroup::GetOBBTree(int idx) const {
  return mContainer->mTrees[idx].get();
}

void CRayCastResult::Transform(const CTransform4f& xf) {
  mPoint = xf * mPoint;
  CVector3f normal = xf.Rotate(mPlane.GetNormal());
  mPlane = CPlane(mPoint, CUnitVector3f(normal.GetX(), normal.GetY(), normal.GetZ()));
}

CRayCastResult
CCollidableOBBTreeGroup::CastRayInternal(const CInternalRayCastStructure& rayCast) const {
  CRayCastResult result;
  const CMaterialFilter filter = rayCast.GetFilter().WithImplicitMaterials(GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return result;
  }
  float mag = rayCast.GetMaxTime();
  rstl::vector< rstl::auto_ptr< COBBTree > >::const_iterator treeIt = mContainer->mTrees.begin();
  rstl::vector< CAABox >::const_iterator aabbIt = mContainer->mAabbs.begin();
  CMRay ray = rayCast.GetRay().GetInvUnscaledTransformRay(rayCast.GetTransform());

  for (; treeIt != mContainer->mTrees.end(); ++treeIt, ++aabbIt) {
    CCollidableOBBTree tree(treeIt->get(), GetMaterial());
    float tMin = 0.f;
    float tMax = 0.f;
    if (CollisionUtil::RayAABoxIntersection(ray, *aabbIt, tMin, tMax)) {
      const CRayCastResult& localResult = tree.CastRayInternal(CInternalRayCastStructure(
          ray.GetStart(), ray.GetDirection(), mag, CTransform4f::Identity(), filter));
      if (localResult.IsValid()) {
        if (result.IsValid()) {
          if (localResult.GetTime() < result.GetTime()) {
            result = localResult;
            mag = localResult.GetTime();
          }
        } else {
          result = localResult;
          mag = localResult.GetTime();
        }
      }
    }
  }
  result.Transform(rayCast.GetTransform());
  return result;
}

bool CCollidableOBBTreeGroup::AABoxCollide(const CInternalCollisionStructure& collision,
                                           CCollisionInfoList& list) {
  const CCollisionPrimitive& left = collision.GetLeft().GetPrim();
  const CCollidableOBBTreeGroup& right =
      static_cast< const CCollidableOBBTreeGroup& >(collision.GetRight().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right.GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CAABox bounds = left.CalculateAABox(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  COBBox obb = COBBox::FromAABox(collision.GetLeft().GetPrim().CalculateLocalAABox(), relativeXf);

  const CVector3f min = bounds.GetMinPoint();
  const CVector3f max = bounds.GetMaxPoint();
  const CUnitVector3f rightNormal(1.f, 0.f, 0.f);
  const CUnitVector3f forwardNormal(0.f, 1.f, 0.f);
  const CUnitVector3f upNormal(0.f, 0.f, 1.f);
  CPlane planes[6] = {CPlane(min, rightNormal),   CPlane(max, -rightNormal),
                      CPlane(min, forwardNormal), CPlane(max, -forwardNormal),
                      CPlane(min, upNormal),      CPlane(max, -upNormal)};
  bool result = false;

  for (int i = 0; i < right.GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right.GetOBBTree(i), right.GetMaterial());
    if (tree.AABoxCollision(*tree.GetOBBTree().GetRoot(), xf, bounds, obb,
                            collision.GetLeft().GetPrim().GetMaterial(), filter, planes, list)) {
      result = true;
    }
  }
  return result;
}

bool CCollidableOBBTreeGroup::AABoxCollideBoolean(const CInternalCollisionStructure& collision) {
  const CCollisionPrimitive& left = collision.GetLeft().GetPrim();
  const CCollidableOBBTreeGroup& right =
      static_cast< const CCollidableOBBTreeGroup& >(collision.GetRight().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right.GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CAABox bounds = left.CalculateAABox(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  COBBox obb = COBBox::FromAABox(collision.GetLeft().GetPrim().CalculateLocalAABox(), relativeXf);

  for (int i = 0; i < right.GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right.GetOBBTree(i), right.GetMaterial());
    if (tree.AABoxCollisionBoolean(*tree.GetOBBTree().GetRoot(), xf, bounds, obb, filter)) {
      return true;
    }
  }
  return false;
}

bool CCollidableOBBTreeGroup::SphereCollide(const CInternalCollisionStructure& collision,
                                            CCollisionInfoList& list) {
  const CCollidableOBBTreeGroup& right =
      static_cast< const CCollidableOBBTreeGroup& >(collision.GetRight().GetPrim());
  const CCollidableSphere& left =
      static_cast< const CCollidableSphere& >(collision.GetLeft().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right.GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CSphere sphere = left.Transform(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  COBBox obb = COBBox::FromAABox(left.CalculateLocalAABox(), relativeXf);
  bool result = false;

  for (int i = 0; i < right.GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right.GetOBBTree(i), right.GetMaterial());
    if (tree.SphereCollision(*tree.GetOBBTree().GetRoot(), xf, sphere, obb, left.GetMaterial(),
                             filter, list)) {
      result = true;
    }
  }
  return result;
}

bool CCollidableOBBTreeGroup::SphereCollideBoolean(const CInternalCollisionStructure& collision) {
  const CCollidableSphere& left =
      static_cast< const CCollidableSphere& >(collision.GetLeft().GetPrim());
  const CCollidableOBBTreeGroup& right =
      static_cast< const CCollidableOBBTreeGroup& >(collision.GetRight().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right.GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CSphere sphere = left.Transform(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  COBBox obb = COBBox::FromAABox(left.CalculateLocalAABox(), relativeXf);

  for (int i = 0; i < right.GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right.GetOBBTree(i), right.GetMaterial());
    if (tree.SphereCollisionBoolean(*tree.GetOBBTree().GetRoot(), xf, sphere, obb, filter)) {
      return true;
    }
  }
  return false;
}

bool CCollidableOBBTreeGroup::CollideMovingAABox(const CInternalCollisionStructure& collision,
                                                 const CVector3f& dir, double& mag,
                                                 CCollisionInfo& info) {
  const CCollisionPrimitive& left = collision.GetLeft().GetPrim();
  const CCollidableOBBTreeGroup* right =
      static_cast< const CCollidableOBBTreeGroup* >(&collision.GetRight().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right->GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CAABox bounds = left.CalculateAABox(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  CMetroidAreaCollider::CMovingAABoxComponents components(bounds, dir);
  CAABox movedBounds = collision.GetLeft().GetPrim().CalculateLocalAABox();
  CVector3f move = static_cast< float >(mag) * dir;
  movedBounds.AccumulateBounds(movedBounds.GetMaxPoint() + move);
  movedBounds.AccumulateBounds(movedBounds.GetMinPoint() + move);
  COBBox obb = COBBox::FromAABox(movedBounds, relativeXf);
  bool result = false;

  for (int i = 0; i < right->GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right->GetOBBTree(i), right->GetMaterial());
    CMetroidAreaCollider::ResetInternalCounters();
    if (tree.AABoxCollisionMoving(*tree.GetOBBTree().GetRoot(), xf, bounds, obb,
                                  collision.GetLeft().GetPrim().GetMaterial(), filter, components,
                                  dir, mag, info)) {
      result = true;
    }
  }
  return result;
}

bool CCollidableOBBTreeGroup::CollideMovingSphere(const CInternalCollisionStructure& collision,
                                                  const CVector3f& dir, double& mag,
                                                  CCollisionInfo& info) {
  const CCollisionPrimitive& left = collision.GetLeft().GetPrim();
  const CCollidableOBBTreeGroup& right =
      static_cast< const CCollidableOBBTreeGroup& >(collision.GetRight().GetPrim());

  const CMaterialFilter filter =
      collision.GetLeft().GetFilter().WithImplicitMaterials(right.GetMaterial());
  if (filter.GetType() == CMaterialFilter::kFT_Never) {
    return false;
  }

  CSphere sphere =
      static_cast< const CCollidableSphere& >(left).Transform(collision.GetLeft().GetTransform());
  CTransform4f xf = collision.GetRight().GetTransform();
  CTransform4f relativeXf = xf.GetQuickInverse() * collision.GetLeft().GetTransform();
  CAABox movedBounds = collision.GetLeft().GetPrim().CalculateLocalAABox();
  CVector3f move = static_cast< float >(mag) * dir;
  movedBounds.AccumulateBounds(movedBounds.GetMaxPoint() + move);
  movedBounds.AccumulateBounds(movedBounds.GetMinPoint() + move);
  COBBox obb = COBBox::FromAABox(movedBounds, relativeXf);
  bool result = false;

  for (int i = 0; i < right.GetContainer()->NumTrees(); ++i) {
    CCollidableOBBTree tree(right.GetOBBTree(i), right.GetMaterial());
    CMetroidAreaCollider::ResetInternalCounters();
    if (tree.SphereCollisionMoving(*tree.GetOBBTree().GetRoot(), xf, sphere, obb,
                                   collision.GetLeft().GetPrim().GetMaterial(), filter, dir, mag,
                                   info)) {
      result = true;
    }
  }
  return result;
}

void CCollidableOBBTreeGroup::CacheTree(CCollisionCache& cache, const CTransform4f& xf,
                                        short ownerId, u64 material) const {
  CTransform4f inverse = xf.GetQuickInverse();
  COBBox obb = COBBox::FromAABox(cache.GetBounds(), inverse);
  CVector3f center = cache.GetBounds().GetCenterPoint();
  CVector3f halfExtent = cache.GetBounds().GetHalfExtent();
  CCollisionCacheWriter writer(cache);

  for (int i = 0; i < mContainer->NumTrees(); ++i) {
    CCollidableOBBTree tree(GetOBBTree(i), CMaterialList(material));
    writer.BeginGeometry(tree.GetOBBTree(), &xf, ownerId, material);
    tree.CacheTree(writer, *tree.GetOBBTree().GetRoot(), xf, center, halfExtent, obb);
  }
}

CAABox CCollidableOBBTreeGroup::CalculateAABox(const CTransform4f& xf) const {
  return mContainer->mAabox.GetTransformedAABox(xf);
}

CAABox CCollidableOBBTreeGroup::CalculateLocalAABox() const { return mContainer->mAabox; }

FourCC CCollidableOBBTreeGroup::GetPrimType() const { return 'OBTG'; }

uint CCollidableOBBTreeGroup::GetTableIndex() const { return sTableIndex; }

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" void fn_805AC3E4(int, unsigned char*);
extern "C" void fn_805AC3B8(int val);
extern "C" void fn_805AC3B8(int val) {
    unsigned char val2[3080];
    *(int*)val2 = 0;
    fn_805AC3E4(val, val2);
}

extern unsigned char lbl_806E24D0[36];
extern "C" void fn_80475DDC(int, int);
extern "C" void fn_805AD0A4(int, int);
extern "C" int fn_805AD020(int obj, int val);
extern "C" int fn_805AD020(int obj, int val) {
    if (obj) {
        *(int*)obj = (int)lbl_806E24D0;
        if (obj + 16 && (*(unsigned char*)((char*)obj + 0x10))) {
            fn_805AD0A4(*(int*)((char*)obj + 0x14), 1);
        }
        fn_80475DDC(obj, 0);
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern int lbl_80796EE0;
extern "C" int fn_805ADAD4();
extern "C" int fn_805ADAD4() {
    return lbl_80796EE0;
}


extern "C" void fn_805ADADC(int val);
extern "C" void fn_805ADADC(int val) {
    lbl_80796EE0 = val;
}

extern "C" int fn_805ADB68(int obj, int val);
extern "C" int fn_805ADB68(int obj, int val) {
    if (obj) {
        CMemory::Free((const void*)*(int*)((char*)obj + 0xc));
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern unsigned char lbl_806B1B80[12];
extern unsigned char lbl_806B1B8C[12];
extern unsigned char lbl_806E24F4[12];
extern "C" int fn_805ADEC4(int obj, int val);
extern "C" int fn_805ADEC4(int obj, int val) {
    if (obj) {
        *(int*)obj = (int)lbl_806E24F4;
        if ((unsigned int)*(int*)((char*)obj + 0x4) != 0) {
            fn_805AD0A4(*(int*)((char*)obj + 0x4), 1);
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

