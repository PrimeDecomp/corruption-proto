#include "Kyoto/Animation/CTransition.hpp"

#include "Kyoto/Animation/CMetaTransFactory.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

CTransition::CTransition(CInputStream& in)
: mVersion(in.Get< uchar >())
, mFromAnim(in)
, mToAnim(in)
, mMetaTrans(CMetaTransFactory::CreateMetaTrans(in)) {}

const rstl::rc_ptr< IMetaTrans >& CTransition::GetMetaTrans() const { return mMetaTrans; }

const CAssetId& CTransition::GetFromAnimId() const { return mFromAnim; }

const CAssetId& CTransition::GetToAnimId() const { return mToAnim; }
