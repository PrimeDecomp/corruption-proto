#ifndef _TSIGNAL3_HPP
#define _TSIGNAL3_HPP

#include "types.h"

#include "Kyoto/TFunctor.hpp"
#include "Kyoto/TSignal1.hpp"

#include "rstl/list.hpp"

// The three-argument signal, shaped like TSignal1 and TSignal2 (the asserts name TSignal1.h ..
// TSignal5.h). CStateManager's weapon-added signal (0x1E0) is the instance modelled so far: its
// Emit (0x8028FDC4) and destructor (0x80296C40) have the same code as TSignal2's.
template < class A1, class A2, class A3 >
class TSignal3 {
public:
  struct SSlot {
    IConnection* mConnection;
    TFunctor3< A1, A2, A3 > mFunctor;
  };

  typedef rstl::list< SSlot > SlotList;
  typedef typename SlotList::iterator iterator;
  typedef typename SlotList::const_iterator const_iterator;

  // Every connection forgets the signal before the slots go away.
  ~TSignal3() {
    for (iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->Detach();
    }
  }

  void Emit(A1 a1, A2 a2, A3 a3) const; // Name confirmed by Metroid Prime Remastered symbols

private:
  SlotList mSlots;
};

// Each slot's iterator is advanced before its functor runs, so a slot may disconnect itself.
template < class A1, class A2, class A3 >
void TSignal3< A1, A2, A3 >::Emit(A1 a1, A2 a2, A3 a3) const {
  const_iterator end = mSlots.end();
  const_iterator it = mSlots.begin();
  while (it != end) {
    const_iterator slot = it;
    ++it;
    slot->mFunctor(a1, a2, a3);
  }
}

#endif // _TSIGNAL3_HPP
