#ifndef _CTRANSITIONDATABASEGAME
#define _CTRANSITIONDATABASEGAME

#include "Kyoto/Animation/CAdditiveAnimationInfo.hpp"
#include "Kyoto/Animation/CTransitionDatabase.hpp"
#include "Kyoto/Animation/IMetaTrans.hpp"
#include "Kyoto/CAssetId.hpp"

#include "rstl/pair.hpp"
#include "rstl/vector.hpp"

class CTransition;
class CHalfTransition;

// G2MEAB moves the additive animation table from the character factory into the transition
// database. CCECharacterFactory builds one (0x44 bytes) from its animation set.
class CTransitionDatabaseGame : public CTransitionDatabase {
public:
  typedef rstl::vector< rstl::pair< CAssetId, CAdditiveAnimationInfo > > AdditiveInfoList;

  CTransitionDatabaseGame(const rstl::vector< CTransition >& transitions,
                          const rstl::vector< CHalfTransition >& halfTransitions,
                          const AdditiveInfoList& additiveInfo,
                          rstl::rc_ptr< IMetaTrans > defaultTrans,
                          const CAdditiveAnimationInfo& defaultAdditiveInfo);
  ~CTransitionDatabaseGame() {}

  const rstl::rc_ptr< IMetaTrans >& GetMetaTrans(const CAssetId& from, const CAssetId& to) const override;
  bool UsesDefaultTransition(const CAssetId& from, const CAssetId& to) const override;
  const CAdditiveAnimationInfo& FindAdditiveInfo(const CAssetId& anim) const override;

private:
  // Guessed names. Each fills and sorts its table; the constructor calls them in this order.
  void AddTransitions(const rstl::vector< CTransition >& transitions);
  void AddHalfTransitions(const rstl::vector< CHalfTransition >& halfTransitions);

  rstl::rc_ptr< IMetaTrans > mDefaultTrans;
  rstl::vector< rstl::pair< rstl::pair< CAssetId, CAssetId >, rstl::rc_ptr< IMetaTrans > > >
      mTransitions;
  rstl::vector< rstl::pair< CAssetId, rstl::rc_ptr< IMetaTrans > > > mHalfTransitions;
  AdditiveInfoList mAdditiveInfo;
  CAdditiveAnimationInfo mDefaultAdditiveInfo;
};
CHECK_SIZEOF(CTransitionDatabaseGame, 0x44)

#endif // _CTRANSITIONDATABASEGAME
