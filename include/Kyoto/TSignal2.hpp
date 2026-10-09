#ifndef _TSIGNAL2_HPP
#define _TSIGNAL2_HPP

#include "types.h"

#include "Kyoto/TFunctor.hpp"

#include "rstl/list.hpp"

// Guessed name. Each slot keeps the connection object that its subscriber holds; the signal's
// destructor disconnects every slot through that object's second virtual.
class CSignalConnection;

// Class name from the "TSignal2.h" allocation asserts (for example CScriptLUA's subscription at
// 0x802B5250). A signal is a list of slots (0x24-byte nodes), each one a connection pointer and
// a two-argument functor.
template < class A1, class A2 >
class TSignal2 {
public:
  struct SSlot {
    CSignalConnection* mConnection;
    TFunctor2< A1, A2 > mFunctor;
  };

  void Emit(A1 a1, A2 a2); // Guessed name

private:
  rstl::list< SSlot > mSlots;
};

// Each slot's iterator is advanced before its functor runs, so a slot may disconnect itself.
template < class A1, class A2 >
void TSignal2< A1, A2 >::Emit(A1 a1, A2 a2) {
  typename rstl::list< SSlot >::iterator end = mSlots.end();
  for (typename rstl::list< SSlot >::iterator it = mSlots.begin(); it != end;) {
    (it++)->mFunctor(a1, a2);
  }
}

#endif // _TSIGNAL2_HPP
