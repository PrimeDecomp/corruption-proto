/*
 * G2MEAB Kyoto/Animation/CParticleResData.cpp (NonMatching).
 * .text: 0x805610AC..0x80561278 (end exclusive; 2 native functions).
 * Prime and Echoes references inspected; original source placement remains inferred.
 * Reference constructor retains version-gated asset-ID vectors. The native constructor
 * reads all six vectors unconditionally; its serialization remains to be reconstructed.
 */

#include "Kyoto/Animation/CCECharacterInfo.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

CCECharacterInfo::CParticleResData::CParticleResData(CInputStream& in, ushort tableCount)
: mPart(in), mSwhc(in), mElscB(in) {
  if (tableCount > 5) {
    const rstl::vector< CAssetId > resources(in);
    mElscA = rstl::vector< CAssetId >(resources.begin(), resources.end());
  }

  if (tableCount > 8) {
    const rstl::vector< CAssetId > spawnResources(in);
    mSpsc = rstl::vector< CAssetId >(spawnResources.begin(), spawnResources.end());
    const rstl::vector< CAssetId > sortedResources(in);
    mSrsc = rstl::vector< CAssetId >(sortedResources.begin(), sortedResources.end());
  }
}
