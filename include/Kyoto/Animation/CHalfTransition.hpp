#ifndef _CHALFTRANSITION
#define _CHALFTRANSITION

#include "Kyoto/CAssetId.hpp"

#include "rstl/rc_ptr.hpp"

class IMetaTrans;
class CInputStream;

// G2MEAB keys half transitions by the destination animation's asset id
// (CHalfTransition.cpp 0x8055DE78..0x8055DF20).
class CHalfTransition {
public:
  explicit CHalfTransition(CInputStream& in);

  // Emitted out of line in CHalfTransition.cpp.
  const rstl::rc_ptr< IMetaTrans >& GetMetaTrans() const; // 0x8055DE78, Prime's name
  const CAssetId& GetToAnimId() const;                    // 0x8055DE80, Guessed name

private:
  uchar mVersion; // Guessed name: the first byte of the stream record
  CAssetId mToAnim;
  rstl::rc_ptr< IMetaTrans > mMetaTrans;
};
CHECK_SIZEOF(CHalfTransition, 0x18)

#endif // _CHALFTRANSITION
