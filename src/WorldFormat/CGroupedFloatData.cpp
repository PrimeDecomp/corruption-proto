/*
 * G2MEAB WorldFormat/CGroupedFloatData.cpp
 * .text: 0x805AE940..0x805AEF30. Nested-vector serialization family used by
 * CScriptSequenceTimer; the class name is provisional and does not assert the float meaning.
 */

#include "WorldFormat/CGroupedFloatData.hpp"

#include "Kyoto/Streams/CInputStream.hpp"

rstl::pair< float, float > CGroupedFloatData::GetFirstFloatRange() const {
  const SRecord* recEnd;
  float f;
  const SGroup* group = mGroups.data();
  float min = 3.4028235e38f;
  float max = 1.1754944e-38f;
  const SGroup* groupEnd = group + mGroups.size();
  while (group != groupEnd) {
    const SRecord* rec = group->mRecords.data();
    recEnd = rec + group->mRecords.size();
    while (rec != recEnd) {
      f = rec->x0_;
      if (f < min) {
        min = f;
      }
      if (max < f) {
        max = f;
      }
      ++rec;
    }
    ++group;
  }
  return rstl::pair< float, float >(min, max);
}

CGroupedFloatData::CGroupedFloatData(CInputStream& in) : mGroups(in) {}

CGroupedFloatData::CGroupedFloatData() {}

CGroupedFloatData::SGroup::SGroup(CInputStream& in)
: mId(in.Get< ushort >()), mRecords(in) {}

CGroupedFloatData::SRecord::SRecord(CInputStream& in)
: x0_(in.Get< float >()), x4_(in.Get< float >()), x8_(in.Get< uint >()), xc_(in.Get< uint >()) {}
