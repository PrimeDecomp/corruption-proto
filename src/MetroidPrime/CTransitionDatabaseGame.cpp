#include "MetroidPrime/CTransitionDatabaseGame.hpp"

#include "Kyoto/Animation/CHalfTransition.hpp"
#include "Kyoto/Animation/CTransition.hpp"

// The additive table's __sort3 (0x80560388) swaps in place; the transition tables, whose pairs
// hold an rc_ptr, call swap out of line (0x8055FE54, 0x8056026C).
#define RSTL_INLINE_SWAP

#include "rstl/algorithm.hpp"

namespace {
// Guessed name, after Prime's uint_comparer: G2MEAB keys the tables by asset id.
struct asset_id_comparer {
  bool operator()(const CAssetId& a, const CAssetId& b) const { return a < b; }
  bool operator()(const rstl::pair< CAssetId, CAssetId >& a,
                  const rstl::pair< CAssetId, CAssetId >& b) const {
    const CAssetId& aTo = a.second;
    if (a.first == b.first) {
      return aTo < b.second;
    }
    return a.first < b.first;
  }
  bool operator()(const rstl::pair< CAssetId, CAdditiveAnimationInfo >& a,
                  const rstl::pair< CAssetId, CAdditiveAnimationInfo >& b) const {
    return a.first < b.first;
  }
};
} // namespace

CTransitionDatabaseGame::CTransitionDatabaseGame(
    const rstl::vector< CTransition >& transitions,
    const rstl::vector< CHalfTransition >& halfTransitions, const AdditiveInfoList& additiveInfo,
    rstl::rc_ptr< IMetaTrans > defaultTrans, const CAdditiveAnimationInfo& defaultAdditiveInfo)
: mDefaultTrans(defaultTrans)
, mAdditiveInfo(additiveInfo)
, mDefaultAdditiveInfo(defaultAdditiveInfo) {
  AddTransitions(transitions);
  AddHalfTransitions(halfTransitions);
  // The additive table is sorted with a named comparator on whole entries, not sort_by_key:
  // the comparator byte comes from the stack (0x8055EBF0) instead of a .sbss temporary.
  asset_id_comparer cmp;
  rstl::sort(mAdditiveInfo.begin(), mAdditiveInfo.end(), cmp);
}

const rstl::rc_ptr< IMetaTrans >& CTransitionDatabaseGame::GetMetaTrans(const CAssetId& from,
                                                                        const CAssetId& to) const {
  const CAssetId fromId(from);
  const CAssetId toId(to);
  const rstl::pair< CAssetId, CAssetId > key(fromId, toId);
  rstl::vector< rstl::pair< rstl::pair< CAssetId, CAssetId >,
                            rstl::rc_ptr< IMetaTrans > > >::const_iterator it =
      rstl::find_by_key(mTransitions, key, asset_id_comparer());
  if (it != mTransitions.end()) {
    return it->second;
  }
  rstl::vector< rstl::pair< CAssetId, rstl::rc_ptr< IMetaTrans > > >::const_iterator halfIt =
      rstl::find_by_key(mHalfTransitions, toId, asset_id_comparer());
  if (halfIt != mHalfTransitions.end()) {
    return halfIt->second;
  }
  return mDefaultTrans;
}

bool CTransitionDatabaseGame::UsesDefaultTransition(const CAssetId& from, const CAssetId& to) const {
  const CAssetId fromId(from);
  const CAssetId toId(to);
  const rstl::pair< CAssetId, CAssetId > key(fromId, toId);
  rstl::vector< rstl::pair< rstl::pair< CAssetId, CAssetId >,
                            rstl::rc_ptr< IMetaTrans > > >::const_iterator it =
      rstl::find_by_key(mTransitions, key, asset_id_comparer());
  if (it != mTransitions.end()) {
    return false;
  }
  rstl::vector< rstl::pair< CAssetId, rstl::rc_ptr< IMetaTrans > > >::const_iterator halfIt =
      rstl::find_by_key(mHalfTransitions, toId, asset_id_comparer());
  return halfIt == mHalfTransitions.end();
}

const CAdditiveAnimationInfo& CTransitionDatabaseGame::FindAdditiveInfo(const CAssetId& anim) const {
  const CAssetId animId(anim);
  AdditiveInfoList::const_iterator it = rstl::find_by_key(mAdditiveInfo, animId, asset_id_comparer());
  if (it != mAdditiveInfo.end()) {
    return it->second;
  }
  return mDefaultAdditiveInfo;
}

void CTransitionDatabaseGame::AddTransitions(const rstl::vector< CTransition >& transitions) {
  rstl::vector< CTransition >::const_iterator it = transitions.begin(), end = transitions.end();
  mTransitions.reserve(transitions.size());
  for (; it != end;) {
    const rstl::pair< CAssetId, CAssetId > ids(it->GetFromAnimId(), it->GetToAnimId());
    const rstl::pair< rstl::pair< CAssetId, CAssetId >, rstl::rc_ptr< IMetaTrans > > entry(
        ids, it->GetMetaTrans());
    mTransitions.push_back(entry);
    ++it;
  }
  rstl::sort_by_key(mTransitions, asset_id_comparer());
}

void CTransitionDatabaseGame::AddHalfTransitions(
    const rstl::vector< CHalfTransition >& halfTransitions) {
  rstl::vector< CHalfTransition >::const_iterator it = halfTransitions.begin(),
                                                  end = halfTransitions.end();
  mHalfTransitions.reserve(halfTransitions.size());
  for (; it != end;) {
    const rstl::pair< CAssetId, rstl::rc_ptr< IMetaTrans > > entry(it->GetToAnimId(),
                                                                   it->GetMetaTrans());
    mHalfTransitions.push_back(entry);
    ++it;
  }
  rstl::sort_by_key(mHalfTransitions, asset_id_comparer());
}
