/*
 * G2MEAB WorldFormat/CRenderingOctree.cpp translation-unit scaffold.
 * .text: 0x805AE338..0x805AE940 (8 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Preserve two separately emitted identical TestBit functions5AE338/5AE364
 * (individually decompiled; distinct external callers802A8D78/80579CB8), followed by recursive
 * bitmap overlaps5AE390, raw/vector overlap wrappers5AE4B8/5AE4F4, NodeBounds5AE568,
 * ChildCount5AE868 and constructor5AE880+C0. NodeBounds explicitly asserts CRenderingOctree.cpp266
 * and CRenderingOctree::CNode Asking for children on a leaf node; target extra assertion accounts
 * for size300 versus smaller reference function. Constructor bitmap/node counts and serialized
 * offset tables match both complete reference inventories/interfaces. Vector resize helper8004F08C
 * is emitted elsewhere and not migrated. Next5AE940 iterates a nested float-record vector with
 * different data layout. Both duplicate bit tests retained as target native emissions, without
 * inventing overload names.
 */

#include "WorldFormat/CAreaRenderOctTree.hpp"

#include "Kyoto/Basics/CBasics.hpp"

static const int skChildCounts[] = {0, 2, 2, 4, 2, 4, 4, 8};
static const int skAxes[][3] = {
    {-1, -1, -1}, {-1, -1, -1}, {-1, -1, -1}, {0, 1, 2}, {-1, -1, -1}, {0, 2, 1}, {2, 0, 1},
};

inline const CAreaRenderOctTree::Node* CAreaRenderOctTree::GetNode(int index) const {
  return reinterpret_cast< const Node* >(mEntries + CBasics::SwapBytes(mIndirectionTable[index]));
}

CAreaRenderOctTree::CAreaRenderOctTree(const rstl::auto_ptr< const uchar >& buffer)
: mBuffer(buffer)
, mBitmapCount(CBasics::SwapBytes(*reinterpret_cast< const uint* >(buffer.get() + 8)))
, mMeshCount(CBasics::SwapBytes(*reinterpret_cast< const uint* >(buffer.get() + 12)))
, mNodeCount(CBasics::SwapBytes(*reinterpret_cast< const uint* >(buffer.get() + 16)))
, mBitmapWordCount((mMeshCount + 31) / 32)
, mBounds(*reinterpret_cast< const CAABox* >(buffer.get() + 20))
, mBitmaps(reinterpret_cast< const uint* >(buffer.get() + 64))
, mIndirectionTable(mBitmaps + mBitmapCount * mBitmapWordCount)
, mEntries(reinterpret_cast< const uchar* >(mIndirectionTable + mNodeCount)) {}

int CAreaRenderOctTree::Node::GetChildCount() const { return skChildCounts[mFlags]; }

CAABox CAreaRenderOctTree::Node::GetNodeBounds(const CAABox& bounds, int childIndex) const {
  CVector3f min = bounds.GetMinPoint();
  CVector3f max = bounds.GetMaxPoint();
  const uint flags = mFlags;
  switch (flags) {
  case kS_Leaf:
  default:
    break;
  case kS_X: {
    const float center = 0.5f * (max.GetX() + min.GetX());
    if (childIndex == 0) {
      max.SetX(center);
    } else {
      min.SetX(center);
    }
    break;
  }
  case kS_Y: {
    const float center = 0.5f * (max.GetY() + min.GetY());
    if (childIndex == 0) {
      max.SetY(center);
    } else {
      min.SetY(center);
    }
    break;
  }
  case kS_Z: {
    const float center = 0.5f * (max.GetZ() + min.GetZ());
    if (childIndex == 0) {
      max.SetZ(center);
    } else {
      min.SetZ(center);
    }
    break;
  }
  case kS_XY:
  case kS_XZ:
  case kS_ZX: {
    const CVector3f center = bounds.GetCenterPoint();
    const int a = skAxes[flags][0];
    const int b = skAxes[flags][1];
    switch (childIndex) {
    case 0:
      max[a] = center[a];
      max[b] = center[b];
      break;
    case 1:
      min[a] = center[a];
      max[b] = center[b];
      break;
    case 2:
      min[b] = center[b];
      max[a] = center[a];
      break;
    case 3:
      min[a] = center[a];
      min[b] = center[b];
      break;
    }
    break;
  }
  case kS_XYZ: {
    const CVector3f center = bounds.GetCenterPoint();
    for (int i = 0; i < 3; ++i) {
      if (childIndex & (1 << i)) {
        min[i] = center[i];
      } else {
        max[i] = center[i];
      }
    }
    break;
  }
  }

  return CAABox(min, max);
}

void CAreaRenderOctTree::FindOverlappingModels(rstl::vector< uint >& bitmap,
                                               const CAABox& bounds) const {
  bitmap.resize(mBitmapWordCount, 0);
  GetNode(0)->RecursiveBuildOverlaps(bitmap.data(), *this, mBounds, bounds);
}

void CAreaRenderOctTree::FindOverlappingModels(uint* bitmap, const CAABox& bounds) const {
  GetNode(0)->RecursiveBuildOverlaps(bitmap, *this, mBounds, bounds);
}

void CAreaRenderOctTree::Node::RecursiveBuildOverlaps(uint* bitmap, const CAreaRenderOctTree& tree,
                                                      const CAABox& bounds,
                                                      const CAABox& testBounds) const {
  if (testBounds.DoBoundsOverlap(bounds)) {
    if (mFlags == kS_Leaf || bounds.Inside(testBounds)) {
      const ushort bitmapIndex = CBasics::SwapBytes(mBitmapIndex);
      const uint* nodeBitmap = &tree.mBitmaps[bitmapIndex * tree.mBitmapWordCount];
      for (uint i = 0; i < tree.mBitmapWordCount; ++i) {
        bitmap[i] |= CBasics::SwapBytes(nodeBitmap[i]);
      }
    } else {
      const int childCount = GetChildCount();
      for (int i = 0; i < childCount; ++i) {
        const Node* child = tree.GetNode(CBasics::SwapBytes(mChildren[i]));
        child->RecursiveBuildOverlaps(bitmap, tree, GetNodeBounds(bounds, i), testBounds);
      }
    }
  }
}

bool CAreaRenderOctTree::TestBit(const uint* bitmap, int bitIndex) {
  return (bitmap[bitIndex >> 5] & (1 << (bitIndex & 31))) != 0;
}
