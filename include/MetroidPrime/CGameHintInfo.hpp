#ifndef _CGAMEHINTINFO
#define _CGAMEHINTINFO

#include "types.h"

#include "Kyoto/SObjectTag.hpp"
#include "MetroidPrime/TGameTypes.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// Minimal declaration with Echoes' names. With 64-bit asset ids a hint is 0x38 bytes and a hint
// location 0x20; CStateManager::UpdateHintState reads them through the memory card.
class CGameHintInfo {
public:
  struct SHintLocation {
    CAssetId mMlvlId;
    CAssetId mMreaId;
    TAreaId mAreaId;
    CAssetId mStringId;
  };

  class CGameHint {
  public:
    CAssetId GetStringId() const { return mStringId; }
    float GetTextTime() const { return mTextTime; }
    const rstl::vector< SHintLocation >& GetLocations() const { return mLocations; }

  private:
    rstl::string mName;
    float mImmediateTime;
    float mNormalTime;
    CAssetId mStringId;
    float mTextTime;
    rstl::vector< SHintLocation > mLocations;
  };

  const rstl::vector< CGameHint >& GetHints() const { return mHints; }

private:
  rstl::vector< CGameHint > mHints;
};
CHECK_SIZEOF(CGameHintInfo, 0x10)
NESTED_CHECK_SIZEOF(CGameHintInfo, SHintLocation, 0x20)
NESTED_CHECK_SIZEOF(CGameHintInfo, CGameHint, 0x38)

#endif // _CGAMEHINTINFO
