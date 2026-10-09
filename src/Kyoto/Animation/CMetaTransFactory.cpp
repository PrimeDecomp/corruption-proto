#include "Kyoto/Animation/CMetaTransFactory.hpp"
#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/Animation/CMetaTransMetaAnim.hpp"
#include "Kyoto/Animation/CMetaTransPhaseTrans.hpp"
#include "Kyoto/Animation/CMetaTransSnap.hpp"
#include "Kyoto/Animation/CMetaTransTrans.hpp"
#include "Kyoto/Animation/IMetaTrans.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

rstl::rc_ptr< IMetaTrans > CMetaTransFactory::CreateMetaTrans(CInputStream& in) {
  EMetaTransType type = static_cast< EMetaTransType >(in.ReadInt32());
  switch (type) {
  case kMTT_MetaAnim:
    return NEW CMetaTransMetaAnim(in);
  case kMTT_Trans:
    return NEW CMetaTransTrans(in);
  case kMTT_PhaseTrans:
    return NEW CMetaTransPhaseTrans(in);
  case kMTT_Snap:
    return NEW CMetaTransSnap;
  default:
    return rstl::rc_ptr< IMetaTrans >();
  }
}
