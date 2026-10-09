/*
 * G2MEAB WorldFormat/COBBTree.cpp translation-unit scaffold.
 * .text: 0x805A687C..0x805A7CD0 (30 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Allocator Alloc5A687C, destructor5A68B0, constructor5A690C (COBBTree.cpp503
 * allocation) lead full leaf/node allocator, construction/destruction, prebuilt-tree access,
 * Build/local bounds/destructor/stream constructor family. verify_version5A7320 asserts
 * COBBTree.cpp118/version4; verify_deafbabe5A73DC asserts108. Preserve SIndexData
 * stream/copy5A7580/5A7738 and all emitted vector copies5A7630/5A77C8/5A7918 plus asserted edge/u64
 * stream helpers5A7A68/5A7BA8+128. Both full reference inventories/source/header checked. Earlier
 * atlas CFBStreamedAnimReader total-destructor candidate5A68B0 was a generic-destructor collision
 * rejected using caller5A7140 and actual tree allocator layout. Next5A7CD0 is LineIntersectsLeaf
 * for CCollidableOBBTree.
 */

#include "WorldFormat/COBBTree.hpp"

#include "Kyoto/Alloc/CMemory.hpp"

COBBTree::CSimpleAllocator* COBBTree::CNode::spAllocator = nullptr;
COBBTree* COBBTree::sPrebuiltTrees[4] = {};

COBBTree::CSimpleAllocator::CSimpleAllocator(uint size)
: mBuffer(NEW char[size]), mSize(size), mOffset(0) {}

COBBTree::CSimpleAllocator::~CSimpleAllocator() {
  if (mBuffer) {
    delete[] mBuffer;
  }
}

void* COBBTree::CSimpleAllocator::Alloc(size_t size) {
  void* result = mBuffer + mOffset;
  mOffset += size;
  if (mOffset & 3) {
    mOffset += 4 - (mOffset & 3);
  }
  return result;
}

COBBTree::SIndexData::SIndexData(CInputStream& in)
: mMaterials(in)
, mVertMaterials(in)
, mEdgeMaterials(in)
, mSurfaceMaterials(in)
, mEdges(in)
, mSurfaceIndices(in)
, mTriangleAdjacency(in)
, mVertices(in) {}

void COBBTree::BindIndexData() {
  mMaterialCount = mIndexData.mMaterials.size();
  mVertexCount = mIndexData.mVertices.size();
  mEdgeCount = mIndexData.mEdges.size();
  mTriangleCount = mIndexData.mSurfaceIndices.size() / 3;
  mMaterials = mIndexData.mMaterials.data();
  mVertexMaterials = mIndexData.mVertMaterials.data();
  mEdgeMaterials = mIndexData.mEdgeMaterials.data();
  mSurfaceMaterials = mIndexData.mSurfaceMaterials.data();
  mEdges = mIndexData.mEdges.data();
  mSurfaceIndices = mIndexData.mSurfaceIndices.data();
  mTriangleAdjacency = mIndexData.mTriangleAdjacency.data();
  mVertices = mIndexData.mVertices.data();
  mOwnsArrays = false;
}

COBBTree::COBBTree(const SIndexData& indexData, const CNode* root)
: mMemsize(root->GetMemoryUsage()), mAllocator(0), mIndexData(indexData), mRoot(root) {
  BindIndexData();
  CNode::SetAllocator(nullptr);
}

uint verify_deaf_babe(CInputStream& in) { return in.Get< uint >(); }

uint verify_version(CInputStream& in) { return in.Get< uint >(); }

COBBTree::COBBTree(CInputStream& in)
: mMagic(verify_deaf_babe(in))
, mVersion(verify_version(in))
, mMemsize(in.Get< uint >())
, mAllocator(mMemsize)
, mIndexData(in)
, mRoot(nullptr) {
  BindIndexData();
  CNode::SetAllocator(&mAllocator);
  mRoot = NEW CNode(in);
}

COBBTree::~COBBTree() {
  CNode::SetAllocator(mAllocator.GetPoolMemSize() ? &mAllocator : nullptr);
  delete mRoot;
}

CAABox COBBTree::CalculateLocalAABox() const {
  if (mRoot) {
    return mRoot->GetOBB().CalculateAABox(CTransform4f::Identity());
  }
  return CAABox(CVector3f::Zero(), CVector3f::Zero());
}

rstl::auto_ptr< COBBTree > COBBTree::BuildOrientedBoundingBoxTree(const CVector3f& extent,
                                                                  const CVector3f& center) {
  const CVector3f halfExtent = extent * 0.5f;
  SIndexData indexData(GetPrebuiltTree(kPBT_UnitCube)->mIndexData);
  for (int i = 0; i < 8; ++i) {
    indexData.mVertices[i] = CVector3f::ByElementMultiply(indexData.mVertices[i], extent) + center;
  }

  rstl::vector< ushort > surfaces;
  surfaces.reserve(12);
  for (ushort i = 0; i < 12; ++i) {
    surfaces.push_back_unsafe(i);
  }
  CNode::SetAllocator(nullptr);
  CLeafData* leaf = NEW CLeafData(surfaces);
  CNode* root = NEW CNode(CTransform4f::Translate(center), halfExtent, nullptr, nullptr, leaf);
  return NEW COBBTree(indexData, root);
}

void COBBTree::SetPrebuiltTree(COBBTree* tree, EPreBuiltTrees which) {
  sPrebuiltTrees[which] = tree;
}

COBBTree* COBBTree::GetPrebuiltTree(EPreBuiltTrees which) { return sPrebuiltTrees[which]; }

COBBTree::CNode::CNode(const CTransform4f& xf, const CVector3f& extents, const CNode* left,
                       const CNode* right, const CLeafData* leaf)
: mObb(xf, extents), mIsLeaf(leaf != nullptr), mLeft(left), mRight(right), mLeaf(leaf) {}

COBBTree::CNode::CNode(CInputStream& in)
: mObb(in)
, mIsLeaf(in.Get< bool >())
, mLeft(mIsLeaf ? nullptr : NEW CNode(in))
, mRight(mIsLeaf ? nullptr : NEW CNode(in))
, mLeaf(mIsLeaf ? NEW CLeafData(in) : nullptr) {}

COBBTree::CNode::~CNode() {
  delete mLeft;
  delete mRight;
  delete mLeaf;
}

uint COBBTree::CNode::GetMemoryUsage() const {
  uint size = sizeof(CNode);
  if (mIsLeaf && mLeaf) {
    size += mLeaf->GetMemoryUsage();
  } else {
    if (mLeft) {
      size += mLeft->GetMemoryUsage();
    }
    if (mRight) {
      size += mRight->GetMemoryUsage();
    }
  }
  if (size & 3) {
    size += 4 - (size & 3);
  }
  return size;
}

void COBBTree::CNode::SetAllocator(CSimpleAllocator* allocator) { spAllocator = allocator; }

void* COBBTree::CNode::operator new(size_t size, const char* file, int line) {
  if (!spAllocator) {
    return NEW char[size];
  }
  return spAllocator->Alloc(size);
}

void COBBTree::CNode::operator delete(void* ptr, size_t size) {
  if (!spAllocator && ptr) {
    delete[] static_cast< char* >(ptr);
  }
}

COBBTree::CLeafData::CLeafData(const rstl::vector< ushort >& surfaces) : mSurfaces(surfaces) {}

COBBTree::CLeafData::CLeafData(CInputStream& in) : mSurfaces(in) {}

uint COBBTree::CLeafData::GetMemoryUsage() const {
  uint size = sizeof(CLeafData) + mSurfaces.size() * sizeof(ushort);
  if (size & 3) {
    size += 4 - (size & 3);
  }
  return size;
}
