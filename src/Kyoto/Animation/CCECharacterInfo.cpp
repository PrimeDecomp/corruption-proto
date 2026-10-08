/*
 * G2MEAB Kyoto/Animation/CCECharacterInfo.cpp (NonMatching).
 * .text: 0x80561380..0x80563AF4 (92 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Leading getters61380..613A0 are referenced from vtable806E1EA0, also written
 * by destructor613A8 and constructor6265C. Factory61A38 allocates0x128 with
 * CCECharacterInfo.cpp(211). Target retains factory/object wrappers, getters/bounds, six
 * particle-resource vectors, PAS database, effect/string/AABB and animation vectors, with all
 * helpers through reserve63A5C; helper638FC is called by CharacterInfo animation-vector stream
 * routine630B8 and63A5C by bounding data stream62EDC. Ends at independently proven CCEAnimationSet
 * factory63AF4. Earlier external resource constructor610AC plus6113C are not absorbed across
 * unrelated quaternion/static helpers61278..61380. Both older CharacterInfo source families and
 * complete inventories consulted; target CE name follows actual filename.
 */

// NonMatching Echoes reference import: retained reference serialization/layout.
// Native allocation/layout differs; boundary evidence above remains authoritative.

#include "Kyoto/Animation/CCECharacterInfo.hpp"

#include "Kyoto/Streams/CInputStream.hpp"

CCECharacterInfo::CCECharacterInfo(CInputStream& in)
: mTableCount(in.Get< ushort >())
, mName(in)
, mCmdl(in.Get< CAssetId >())
, mCksr(in.Get< CAssetId >())
, mCinf(in.Get< CAssetId >())
, mAnimInfo(in)
, mPasDatabase(in.Get< CPASDatabase >())
, mPartRes(in, mTableCount)
, mDefaultAnimation(in.Get< uint >())
, mCmdlOverlay(kInvalidAssetId)
, mCksrOverlay(kInvalidAssetId)
, mSpatialPrimitiveId(kInvalidAssetId)
, mAnimatedScale(false) {
  if (mTableCount > 1) {
    mAabbs = rstl::vector< rstl::pair< rstl::string, CAABox > >(in);
  }
  if (mTableCount > 2) {
    mEffects = TEffectList(in);
  }
  if (mTableCount > 3) {
    mCmdlOverlay = in.Get< CAssetId >();
    mCksrOverlay = in.Get< CAssetId >();
  } else {
    mCmdlOverlay = CAssetId(0);
    mCksrOverlay = CAssetId(0);
  }
  if (mTableCount > 4) {
    mAnimIdxs = rstl::vector< uint >(in);
  }
  if (mTableCount > 6) {
    mSpatialPrimitiveId = in.Get< CAssetId >();
  }
  if (mTableCount > 7) {
    mAnimatedScale = in.Get< bool >();
  }
  if (mTableCount > 9) {
    mAnimBoundsById = rstl::vector< rstl::pair< uint, CAABox > >(in);
  }
}
