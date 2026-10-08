/*
 * G2MEAB Collision/CMaterialFilter.cpp translation-unit scaffold.
 * .text: 0x8047C2D4..0x8047C680 (3 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Three-natives: WithImplicitMaterials7C2D4 handles five filter cases with64-bit
 * include/exclude masks, Passes7C52C agrees with Echoes, static initializer7C658+28 agrees with
 * both refs. Source/header enum and mask semantics checked in both games. Next7C680 is a full
 * oriented-box separating-axis test.
 */
#include "Collision/CMaterialFilter.hpp"

const CMaterialFilter CMaterialFilter::skPassEverything;

bool CMaterialFilter::Passes(const CMaterialList& other) const {
  switch (mType) {
  case kFT_Always:
    return true;
  case kFT_Include:
    return other.SharesMaterials(mInclude);
  case kFT_Exclude:
    return !other.SharesMaterials(mExclude);
  case kFT_IncludeExclude:
    return other.SharesMaterials(mInclude) && !other.SharesMaterials(mExclude);
  case kFT_Never:
    return false;
  default:
    return true;
  }
}

CMaterialFilter CMaterialFilter::WithImplicitMaterials(const CMaterialList& materials) const {
  switch (mType) {
  case kFT_Always:
  case kFT_Never:
    return *this;
  case kFT_Include:
    return mInclude.SharesMaterials(materials) ? CMaterialFilter() : *this;
  case kFT_Exclude:
    if (mExclude.SharesMaterials(materials)) {
      return CMaterialFilter(CMaterialList(), CMaterialList(0x00000000FFFFFFFF), kFT_Never);
    }
    return *this;
  case kFT_IncludeExclude:
    if (mInclude.SharesMaterials(materials)) {
      return CMaterialFilter(CMaterialList(0x00000000FFFFFFFF), mExclude, kFT_Exclude);
    }
    if (mExclude.SharesMaterials(materials)) {
      return CMaterialFilter(CMaterialList(), CMaterialList(0x00000000FFFFFFFF), kFT_Never);
    }
    return *this;
  }
  return CMaterialFilter();
}
