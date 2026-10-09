#ifndef _TSIGNAL1
#define _TSIGNAL1

#include "types.h"

#include "Kyoto/Alloc/CMemory.hpp"
#include "Kyoto/TFunctor.hpp"

#include "rstl/auto_ptr.hpp"
#include "rstl/list.hpp"

// A one-argument signal. The template name comes from the "TSignal1.h(56) : " allocation string;
// everything else is inferred from its instantiation for CDebugOption (CDebugOption.cpp and
// CGameDebug). Neither Echoes nor Prime has an equivalent.
//
// Each listener is a TFunctor1 kept in an rstl::list together with the connection object handed
// back to the caller. Deleting the connection removes the listener. Destroying the signal first
// detaches every connection, so connections that outlive the signal do nothing when deleted.

// Guessed name. Non-template base of the connections (vtable 0x806B1CE4, both slots pure).
class IConnection {
public:
  virtual ~IConnection() = 0;
  // Guessed name. Called by the signal's destructor so the connection forgets the signal.
  virtual void Detach() = 0;
};

inline IConnection::~IConnection() {}

// Guessed name. Remembers where its listener sits in the owner and the owner's member that
// removes it (0x18 bytes).
template < typename Owner, typename Key >
class TConnection : public IConnection {
public:
  typedef void (Owner::*DisconnectFn)(Key);

  TConnection(Key key, Owner* owner, DisconnectFn disconnect)
  : mKey(key), mOwner(owner), mDisconnect(disconnect) {}
  ~TConnection() {
    if (mOwner != nullptr) {
      (mOwner->*mDisconnect)(mKey);
    }
  }

  void Detach() { mOwner = nullptr; }
  void SetKey(Key key) { mKey = key; }

private:
  Key mKey;
  Owner* mOwner;
  DisconnectFn mDisconnect;
};

template < typename A1 >
class TSignal1 {
public:
  typedef TFunctor1< A1 > Functor;

  // Guessed name. One listener (0x1C bytes).
  struct SSlot {
    SSlot(IConnection* connection, const Functor& functor)
    : mConnection(connection), mFunctor(functor) {}

    IConnection* mConnection;
    Functor mFunctor;
  };

  typedef rstl::list< SSlot > SlotList;
  typedef typename SlotList::iterator iterator;
  typedef typename SlotList::const_iterator const_iterator;
  typedef TConnection< TSignal1, iterator > Connection;

  ~TSignal1() {
    for (iterator it = mSlots.begin(); it != mSlots.end(); ++it) {
      it->mConnection->Detach();
    }
  }

  // Guessed names.
  rstl::auto_ptr< IConnection > Connect(Functor functor);
  void Disconnect(iterator it) { mSlots.erase(it); }
  void Emit(A1 arg) const;

private:
  SlotList mSlots;
};

template < typename A1 >
rstl::auto_ptr< IConnection > TSignal1< A1 >::Connect(Functor functor) {
  Connection* connection = new ("TSignal1.h(56) : ", (const char*)0)
      Connection(mSlots.end(), this, &TSignal1::Disconnect);
  connection->SetKey(mSlots.insert(mSlots.end(), SSlot(connection, functor)));
  return connection;
}

// The iterator steps past each listener before calling it.
template < typename A1 >
void TSignal1< A1 >::Emit(A1 arg) const {
  const_iterator end = mSlots.end();
  const_iterator it = mSlots.begin();
  while (it != end) {
    const_iterator slot = it;
    ++it;
    slot->mFunctor(arg);
  }
}

#endif // _TSIGNAL1
