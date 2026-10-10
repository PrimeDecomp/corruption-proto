#ifndef _CTRANSITIONDATABASE
#define _CTRANSITIONDATABASE

#include "rstl/rc_ptr.hpp"

class CAdditiveAnimationInfo;
class CAssetId;
class IMetaTrans;

// G2MEAB's base holds only the vtable (0x806CE5E0): its destructor (0x804918EC) restores the
// vtable and destroys nothing, unlike Prime's token-holding base. The animations are keyed by
// asset id.
class CTransitionDatabase {
public:
  virtual ~CTransitionDatabase() {}
  virtual const rstl::rc_ptr< IMetaTrans >& GetMetaTrans(const CAssetId& from, const CAssetId& to) const = 0;
  // Guessed name. CTransitionDatabaseGame answers true when neither a transition nor a half
  // transition covers the pair, i.e. when GetMetaTrans would return the default.
  virtual bool UsesDefaultTransition(const CAssetId& from, const CAssetId& to) const = 0;
  // Guessed name, after Metaforce's CCharacterFactory::FindAdditiveInfo, which this lookup
  // replaces; CAnimData passes the result to CAdditiveAnimPlayback.
  virtual const CAdditiveAnimationInfo& FindAdditiveInfo(const CAssetId& anim) const = 0;
};
CHECK_SIZEOF(CTransitionDatabase, 0x4)

#endif // _CTRANSITIONDATABASE
