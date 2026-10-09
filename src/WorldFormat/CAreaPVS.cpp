/*
 * G2MEAB WorldFormat/CAreaPVS.cpp translation-unit scaffold.
 * .text: 0x805AE030..0x805AE338 (6 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Six-natives: entityID lookup5AE030, GetLightSet5AE040, visoctree getter5AE0F8,
 * MakeAreaSet5AE100 (allocation asserts CAreaPVS.cpp65), holder constructor5AE21C and emitted
 * visoctree copy5AE2A4+94. Stream factory reads six counts and forms entity/light/octree regions
 * before0x64 allocation; light getter uses visibility-reset out-of-bounds path. Both complete
 * CPVSAreaSet reference inventories/source/header corroborate this coherent family. Target
 * assertion filename takes precedence over retail source name. Next5AE338 is independently
 * inspected bitmap test, retained with following rendering octree.
 */

#include "WorldFormat/CPVSAreaSet.hpp"

#include "Kyoto/Basics/CBasics.hpp"
#include "Kyoto/Streams/CMemoryInStream.hpp"

CPVSAreaSet::CPVSAreaSet(int numFeatures, int numLights, int num2ndLights, int numActors,
                         int leafSize, int lightIndexCount, const char* entityIds,
                         const char* lightLeaves, const char* octreeData)
: mNumFeatures(numFeatures)
, mNumLights(numLights)
, mNum2ndLights(num2ndLights)
, mNumActors(numActors)
, mLeafSize(leafSize)
, mLightIndexCount(lightIndexCount)
, mEntityIds(entityIds)
, mLightLeaves(lightLeaves)
, mOctree(CPVSVisOctree::MakePVSVisOctree(octreeData, 68)) {}

rstl::auto_ptr< CPVSAreaSet > CPVSAreaSet::MakeAreaSet(const char* data, int length) {
  CMemoryInStream in(data, length);
  const int numFeatures = in.ReadInt32();
  const int numLights = in.ReadInt32();
  const int num2ndLights = in.ReadInt32();
  const int numActors = in.ReadInt32();
  const int leafSize = in.ReadInt32();
  const int lightIndexCount = in.ReadInt32();

  data += in.GetReadPosition();
  const char* const lightLeaves = data + numActors * 4;
  const char* const octreeData = lightLeaves + lightIndexCount * leafSize;
  return rstl::auto_ptr< CPVSAreaSet >(rs_new CPVSAreaSet(numFeatures, numLights, num2ndLights,
                                                          numActors, leafSize, lightIndexCount,
                                                          data, lightLeaves, octreeData));
}

CPVSVisOctree& CPVSAreaSet::GetVisOctree() const { return mOctree; }

CPVSVisSet CPVSAreaSet::GetLightSet(int lightIndex) const {
  if (lightIndex >= mLightIndexCount) {
    return CPVSVisSet(kVSS_OutOfBounds);
  }

  rstl::auto_ptr< const char > leaf(mLightLeaves + mLeafSize * lightIndex);
  leaf.release();
  return CPVSVisSet(mOctree.GetNumObjects(), mOctree.GetNumLights(), leaf);
}

int CPVSAreaSet::GetEntityIdByIndex(uint index) const {
  return CBasics::SwapBytes(reinterpret_cast< const int* >(mEntityIds)[index]);
}
