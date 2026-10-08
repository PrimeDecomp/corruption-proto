#ifndef _CGROUPEDFLOATDATA
#define _CGROUPEDFLOATDATA

#include "types.h"

#include "rstl/pair.hpp"
#include "rstl/vector.hpp"

class CInputStream;

// Provisional name: stream-loaded groups (ushort id + records of two floats and two words)
// consumed by CScriptSequenceTimer.
class CGroupedFloatData {
public:
  struct SRecord {
    SRecord(CInputStream& in);
    float x0_;
    float x4_;
    uint x8_;
    uint xc_;
  };
  struct SGroup {
    SGroup(CInputStream& in);
    ushort mId;
    rstl::vector< SRecord > mRecords;
  };

  CGroupedFloatData();
  CGroupedFloatData(CInputStream& in);
  rstl::pair< float, float > GetFirstFloatRange() const;

private:
  rstl::vector< SGroup > mGroups;
};
CHECK_SIZEOF(CGroupedFloatData, 0x10)

#endif // _CGROUPEDFLOATDATA
