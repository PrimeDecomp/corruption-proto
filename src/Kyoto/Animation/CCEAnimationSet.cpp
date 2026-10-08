/*
 * G2MEAB Kyoto/Animation/CCEAnimationSet.cpp (NonMatching).
 * .text: 0x80563AF4..0x80564A30 (28 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Factory63AF4 allocates0x5C with CCEAnimationSet.cpp(35) and calls constructor640C0. Constructor reads table version, animation/transition vectors, MetaTransFactory default, additive pairs/floats and extra evaluator data. Preserve object wrappers/destructors, stream vector/assert helpers, transition reserve/copy64724/647E4, half-transition reserve/copy64858/64918 and additive-pair reserve6497C. Next64A30 is unrelated CRandom16-driven random-value update. Both CAnimationSet source families and full native inventories explain changed CE data, without importing retail table counts or source name.
 */

// NonMatching Echoes reference import: retained reference serialization/layout.
// Native allocation/layout differs; boundary evidence above remains authoritative.

#include "Kyoto/Animation/CCEAnimationSet.hpp"

#include "Kyoto/Animation/CMetaTransFactory.hpp"

CCEAnimationSet::CCEAnimationSet(CInputStream& in)
: mTableCount(in.Get< ushort >())
, mAnimations(in)
, mTransitions(in)
, mDefaultTransition(CMetaTransFactory::CreateMetaTrans(in))
, mAdditiveAnimations(StreamAdditiveAnimInfoList(mTableCount, in))
, mDefaultAdditiveAnimation(StreamDefaultAdditiveAnimInfo(mTableCount, in))
, mHalfTransitions(StreamHalfTransitions(mTableCount, in))
, mEventSets(StreamEventSetList(mTableCount, in)) {}

CCEAnimationSet::AdditiveAnimationList CCEAnimationSet::StreamAdditiveAnimInfoList(ushort tableCount,
                                                                               CInputStream& in) {
  if (tableCount > 1) {
    return AdditiveAnimationList(in);
  }

  return AdditiveAnimationList();
}

CAdditiveAnimationInfo CCEAnimationSet::StreamDefaultAdditiveAnimInfo(ushort tableCount,
                                                                    CInputStream& in) {
  if (tableCount > 1) {
    return CAdditiveAnimationInfo(in);
  }

  return CAdditiveAnimationInfo(0.f, 0.f);
}

CCEAnimationSet::HalfTransitionList CCEAnimationSet::StreamHalfTransitions(ushort tableCount,
                                                                       CInputStream& in) {
  if (tableCount > 2) {
    return HalfTransitionList(in);
  }

  return HalfTransitionList();
}

// Guessed name.
CCEAnimationSet::EventSetList CCEAnimationSet::StreamEventSetList(ushort tableCount, CInputStream& in) {
  if (tableCount > 3) {
    return EventSetList(in);
  }

  return EventSetList();
}
