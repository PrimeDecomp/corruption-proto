#include "Kyoto/Animation/CTreeUtils.hpp"

#include "Kyoto/Animation/CAnimSysContext.hpp"
#include "Kyoto/Animation/CAnimTreeNode.hpp"
#include "Kyoto/Animation/CTransitionDatabase.hpp"
#include "Kyoto/Animation/IMetaTrans.hpp"
#include "Kyoto/CAssetId.hpp"

rstl::ncrc_ptr< CAnimTreeNode >
CTreeUtils::GetTransitionTree(const rstl::ncrc_ptr< CAnimTreeNode >& a,
                              const rstl::ncrc_ptr< CAnimTreeNode >& b,
                              const CAnimSysContext& animCtx) {
  rstl::rc_ptr< IMetaTrans > trans = GetMetaTrans(a, b, animCtx);
  return trans->GetTransitionTree(a, b, animCtx);
}

rstl::rc_ptr< IMetaTrans > CTreeUtils::GetMetaTrans(const rstl::ncrc_ptr< CAnimTreeNode >& a,
                                                    const rstl::ncrc_ptr< CAnimTreeNode >& b,
                                                    const CAnimSysContext& animCtx) {
  CAnimTreeEffectiveContribution contribA = a->GetContributionOfHighestInfluence();
  CAnimTreeEffectiveContribution contribB = b->GetContributionOfHighestInfluence();
  // G2MEAB keys transitions by asset id: the target passes the 8-byte id at +0x8 of each
  // contribution, which this Prime-layout CAnimTreeEffectiveContribution does not model yet.
  return animCtx.GetTransitionDatabase().NonConstCopy()->GetMetaTrans(
      CAssetId(contribA.GetAnimDatabaseIndex()), CAssetId(contribB.GetAnimDatabaseIndex()));
}
