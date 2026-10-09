#ifndef _TSIGNAL
#define _TSIGNAL

#include "types.h"

#include "Kyoto/TFunctor.hpp"

#include "rstl/list.hpp"

// The prototype's signal/slot helpers. Asserts name the original headers TSignal1.h ..
// TSignal5.h and TSignal4R.h; the class names follow those file names. Everything else here is
// a guessed name. Only the parts main.cpp needs are modelled.
//
// Connecting allocates a connection object (deleted through its vtable) that the subscriber
// owns; destroying the connection removes its slot from the signal. Destroying the signal
// first tells every live connection to forget the signal (second virtual, e.g. 0x8004BC9C,
// which clears the connection's signal pointer).
class CSignalConnection {
public:
  virtual ~CSignalConnection() = 0;
  virtual void DetachFromSignal() = 0;
};

class TSignal0 {
public:
  struct SSlot {
    CSignalConnection* mConnection;
    TFunctor0 mFunctor;
  };
  typedef rstl::list< SSlot >::const_iterator const_iterator;

  ~TSignal0() {
    for (rstl::list< SSlot >::iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->DetachFromSignal();
    }
  }

  // Calls every slot; the next node is fetched before the call so a slot may disconnect itself.
  void operator()() const {
    const_iterator end = mSlots.end();
    const_iterator it = mSlots.begin();
    while (it != end) {
      const_iterator slot = it;
      ++it;
      slot->mFunctor();
    }
  }

private:
  rstl::list< SSlot > mSlots;
};

template < typename P1 >
class TSignal1 {
public:
  struct SSlot {
    CSignalConnection* mConnection;
    TFunctor1< P1 > mFunctor;
  };
  typedef typename rstl::list< SSlot >::const_iterator const_iterator;

  ~TSignal1() {
    for (typename rstl::list< SSlot >::iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->DetachFromSignal();
    }
  }

  void operator()(P1 p1) const {
    const_iterator end = mSlots.end();
    const_iterator it = mSlots.begin();
    while (it != end) {
      const_iterator slot = it;
      ++it;
      slot->mFunctor(p1);
    }
  }

private:
  rstl::list< SSlot > mSlots;
};

#endif // _TSIGNAL
