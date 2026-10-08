/*
 * G2MEAB WorldFormat/CMetroidModelInstance.cpp translation-unit scaffold.
 * .text: 0x8059CC34..0x8059CE78 (6 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Complete six-native sequence: SAreaSurface stream constructor59CC34, group
 * count59CCC0 and indices59CCE8, model instance constructor59CD24, bounding-box helper59CE20 and
 * transform pointer helper59CE74. Constructor copies header transform and bounds,
 * material/token/surface data, vertex channels and two extra section pointers as in Echoes source,
 * with prototype ownership changes retained. Both complete reference inventories/header support TU.
 * Tiny pointer-return helper identified by actual constructor caller rather than fingerprint;
 * next59CE78 asserts DolphinCAreaOctTree.cpp.
 */

#include "WorldFormat/CMetroidModelInstance.hpp"

#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Streams/CInputStream.hpp"
#include "MetaRender/IRenderer.hpp"

SAreaSurface::SAreaSurface(CInputStream& in)
: mBounds(in)
, mModelIndex(in.ReadInt16())
, mSurfaceGroupIndex(in.ReadInt16())
, x1c_(in.ReadInt16())
, x1e_(in.ReadInt16()) {}

ushort CMetroidModelInstance::CSurfaceGroups::GetSurfaceCount(int group) const {
  const ushort count = mData[group + 1];
  if (group == 0)
    return count;
  return count - mData[group];
}

const ushort* CMetroidModelInstance::CSurfaceGroups::GetSurfaceIndices(int group) const {
  const ushort start = group == 0 ? 0 : mData[group];
  return mData + (mData[0] + 1) + start;
}

static const CTransform4f& TransformFromData(const void* data);
static CAABox BoundingBoxFromData(const void* data);

CMetroidModelInstance::CMetroidModelInstance(
    const void* header, const void* materials, const void* positions, const void* normals,
    const void* colors, const void* texCoords, const void* packedTexCoords,
    const rstl::vector< void* >& surfaces, const void* const& section1, const void* const& section2)
: mVisorFlags(*static_cast< const uint* >(header))
, mWorldTransform(TransformFromData(static_cast< const uchar* >(header) + sizeof(uint)))
, mWorldBounds(BoundingBoxFromData(static_cast< const uchar* >(header) + sizeof(CTransform4f) +
                                   sizeof(uint)))
, mMaterialData(materials)
, mSurfaces(surfaces)
, mPositions(positions)
, mNormals(normals)
, mColors(colors)
, mTexCoords(texCoords)
, mPackedTexCoords(packedTexCoords)
, x74_(section1)
, mSurfaceGroups(section2) {}

static CAABox BoundingBoxFromData(const void* data) {
  float values[6];
  const float* source = static_cast< const float* >(data);
  for (int i = 0; i < 6; ++i) {
    values[i] = CBasics::SwapBytes(source[i]);
  }
  return *reinterpret_cast< const CAABox* >(values);
}

static const CTransform4f& TransformFromData(const void* data) {
  return *static_cast< const CTransform4f* >(data);
}
