#include "Kyoto/Animation/CHalfTransition.hpp"

#include "Kyoto/Animation/CMetaTransFactory.hpp"
#include "Kyoto/Animation/IMetaTrans.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

CHalfTransition::CHalfTransition(CInputStream& in)
: mVersion(in.Get< uchar >()), mToAnim(in), mMetaTrans(CMetaTransFactory::CreateMetaTrans(in)) {}

const CAssetId& CHalfTransition::GetToAnimId() const { return mToAnim; }

const rstl::rc_ptr< IMetaTrans >& CHalfTransition::GetMetaTrans() const { return mMetaTrans; }
