#include "Kyoto/Animation/CMetaAnimFactory.hpp"
#include "Kyoto/Animation/CCharAnimTime.hpp"
#include "Kyoto/Animation/CMetaAnimBlend.hpp"
#include "Kyoto/Animation/CMetaAnimPhaseBlend.hpp"
#include "Kyoto/Animation/CMetaAnimPlay.hpp"
#include "Kyoto/Animation/CMetaAnimRandom.hpp"
#include "Kyoto/Animation/CMetaAnimSequence.hpp"
#include "Kyoto/Animation/IMetaAnim.hpp"
#include "Kyoto/Streams/CInputStream.hpp"

rstl::rc_ptr< IMetaAnim > CMetaAnimFactory::CreateMetaAnim(CInputStream& in) {
  EMetaAnimType type = static_cast< EMetaAnimType >(in.ReadInt32());
  switch (type) {
  case kMAT_Play:
    return NEW CMetaAnimPlay(in);
  case kMAT_Blend:
    return NEW CMetaAnimBlend(in);
  case kMAT_PhaseBlend:
    return NEW CMetaAnimPhaseBlend(in);
  case kMAT_Random:
    return NEW CMetaAnimRandom(in);
  case kMAT_Sequence:
    return NEW CMetaAnimSequence(in);
  default:
    return rstl::rc_ptr< IMetaAnim >();
  }
}
