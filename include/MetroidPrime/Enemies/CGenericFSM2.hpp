#ifndef _CGENERICFSM2
#define _CGENERICFSM2

#include "types.h"

// Minimal declaration. The name is from the asserts in CGenericFSM2.cpp
// ("CGenericFSM2State::SetState", "::ExecuteState", "::ExecuteSubflow"). It is the running
// state of a CGenericFSM2 machine that each CAi owns (0xBC bytes, cleared by 0x8028E684): the
// current node is at 0x44 ("mpCurrentNode != NULL"), and 0x58 is set once the machine has been
// built from its resource (0x8028BEE0) and cleared again on shutdown (0x8028BD90).
class CGenericFSM2State {
public:
  // Guessed name. 0x8028AED0.
  bool IsInitialized() const;

private:
  uchar x0_[0xBC];
};

#endif // _CGENERICFSM2
