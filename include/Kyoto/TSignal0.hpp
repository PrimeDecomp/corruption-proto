#ifndef _TSIGNAL0
#define _TSIGNAL0

#include "types.h"

#include "Kyoto/TFunctor.hpp"
#include "Kyoto/TSignal1.hpp"

#include "rstl/list.hpp"

// The no-argument signal, following TSignal1. Asserts name the original headers TSignal1.h ..
// TSignal5.h and TSignal4R.h; TSignal0 is a guessed name. CMain's frame callbacks are the only
// instances modelled so far.
class TSignal0 {
public:
  struct SSlot {
    IConnection* mConnection;
    TFunctor0 mFunctor;
  };
  typedef rstl::list< SSlot >::iterator iterator;
  typedef rstl::list< SSlot >::const_iterator const_iterator;

  ~TSignal0() {
    for (iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->Detach();
    }
  }

  // Guessed name, after TSignal1. 0x8009E9CC, emitted in CAi.cpp. The allocation string of the
  // connection names the original header "CSignal0.h" (line 52).
  rstl::auto_ptr< IConnection > Connect(TFunctor0 functor);
  void Emit() const; // Name confirmed by Metroid Prime Remastered symbols

private:
  rstl::list< SSlot > mSlots;
};

// The iterator steps past each listener before calling it, so a listener may disconnect itself.
inline void TSignal0::Emit() const {
  const_iterator end = mSlots.end();
  const_iterator it = mSlots.begin();
  while (it != end) {
    const_iterator slot = it;
    ++it;
    slot->mFunctor();
  }
}

#endif // _TSIGNAL0
