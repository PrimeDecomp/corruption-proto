#ifndef _CTRANSITION
#define _CTRANSITION

#include "Kyoto/CAssetId.hpp"

#include "rstl/rc_ptr.hpp"

class IMetaTrans;
class CInputStream;

// G2MEAB keys transitions by animation asset id (CTransition.cpp 0x805608C0..0x8056098C).
class CTransition {
public:
  explicit CTransition(CInputStream& in);

  // Guessed names. Emitted out of line in CTransition.cpp.
  const CAssetId& GetToAnimId() const;   // 0x805608C0
  const CAssetId& GetFromAnimId() const; // 0x805608C8
  const rstl::rc_ptr< IMetaTrans >& GetMetaTrans() const; // 0x805608D0, Prime's name

private:
  uchar mVersion; // Guessed name: the first byte of the stream record
  CAssetId mFromAnim;
  CAssetId mToAnim;
  rstl::rc_ptr< IMetaTrans > mMetaTrans;
};
CHECK_SIZEOF(CTransition, 0x20)

#endif // _CTRANSITION
