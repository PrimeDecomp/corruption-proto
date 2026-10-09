#ifndef _TSIGNAL2_HPP
#define _TSIGNAL2_HPP

#include "types.h"

#include "Kyoto/TFunctor.hpp"
#include "Kyoto/TSignal1.hpp"

#include "rstl/list.hpp"

// Class name from the "TSignal2.h" allocation asserts (for example CScriptLUA's subscription at
// 0x802B5250). A signal is a list of slots (0x24-byte nodes), each one a connection pointer and
// a two-argument functor.
template < class A1, class A2 >
class TSignal2 {
public:
  struct SSlot {
    IConnection* mConnection;
    TFunctor2< A1, A2 > mFunctor;
  };

  typedef TFunctor2< A1, A2 > Functor;
  typedef rstl::list< SSlot > SlotList;
  typedef typename SlotList::iterator iterator;

  // Same shape as TSignal1's: every connection forgets the signal before the slots go away
  // (CStateManagerCallbackLists' destructor, 0x802FFB1C, calls two instances).
  ~TSignal2() {
    for (iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->Detach();
    }
  }

  // Guessed name, as TSignal1's. The <CStateManager&, CEntity&> instance is emitted in
  // CGameArea.cpp (0x8005986C); the functor is passed by value.
  rstl::auto_ptr< IConnection > Connect(Functor functor);
  void Emit(A1 a1, A2 a2) const; // Name confirmed by Metroid Prime Remastered symbols

private:
  SlotList mSlots;
};

// Each slot's iterator is advanced before its functor runs, so a slot may disconnect itself.
template < class A1, class A2 >
void TSignal2< A1, A2 >::Emit(A1 a1, A2 a2) const {
  typename rstl::list< SSlot >::const_iterator end = mSlots.end();
  typename rstl::list< SSlot >::const_iterator it = mSlots.begin();
  while (it != end) {
    typename rstl::list< SSlot >::const_iterator slot = it;
    ++it;
    slot->mFunctor(a1, a2);
  }
}

#endif // _TSIGNAL2_HPP
