#ifndef _TFUNCTOR_HPP
#define _TFUNCTOR_HPP

#include "types.h"
#include <string.h>

// Prime-correlated names. Native callbacks store a bridge, borrowed object, and 16 method bytes.
class CMethodPtrStore {
public:
  typedef void (*DummyFunctor)();

  CMethodPtrStore() { memset(mFuncStorage, 0, sizeof(mFuncStorage)); }

  CMethodPtrStore(const void* method, int size) { memcpy(mFuncStorage, method, size); }

  bool IsNull() const {
    for (int i = 0; i < ARRAY_SIZE(mFuncStorage); ++i) {
      if (mFuncStorage[i] != 0) {
        return false;
      }
    }
    return true;
  }

  const void* GetMethodPointer() const { return mFuncStorage; }

private:
  union {
    DummyFunctor mFunc;
    char mFuncStorage[(sizeof(DummyFunctor) + 15) & ~15];
  };
};

// Guessed name, following TFunctor1/TFunctor2: the no-argument callback stored by TSignal0.
class TFunctor0 {
public:
  typedef void (*Functor)(const void* object, const void* method);

  TFunctor0(Functor functor, const void* object, const void* method, int size)
  : mFunctor(functor), mObject(object), mMethod(method, size) {}

  void operator()() const { mFunctor(mObject, mMethod.GetMethodPointer()); }

private:
  Functor mFunctor;
  const void* mObject;
  CMethodPtrStore mMethod;
};

template < class Arg1 >
class TFunctor1 {
public:
  typedef void (*Functor)(const void* object, const void* method, Arg1 arg1);

  TFunctor1() : mFunctor(nullptr), mObject(nullptr) {}

  TFunctor1(Functor functor, const void* object, const void* method, int size)
  : mFunctor(functor), mObject(object), mMethod(method, size) {}

  void operator()(Arg1 arg1) const { mFunctor(mObject, mMethod.GetMethodPointer(), arg1); }

  operator bool() const { return !mMethod.IsNull(); }

private:
  Functor mFunctor;
  const void* mObject;
  CMethodPtrStore mMethod;
};

// Guessed names, following TNonStaticCallback1: the bridge of a functor that calls a free
// function, whose pointer is stored in place of the member pointer (ConsoleCommands.cpp).
class TStaticCallback0 {
public:
  typedef void (*FunctionPtr)();

  static void Function(const void* object, const void* method) {
    FunctionPtr callback;
    memcpy(&callback, method, sizeof(callback));
    callback();
  }
};

class TFunctor0FromFunction {
public:
  typedef void (*FunctionPtr)();

  static TFunctor0 Make(FunctionPtr function) {
    char functionData[sizeof(function)];
    memcpy(functionData, &function, sizeof(function));
    return TFunctor0(TStaticCallback0::Function, nullptr, functionData, sizeof(function));
  }
};

template < typename P1 >
class TStaticCallback1 {
public:
  typedef void (*FunctionPtr)(P1);

  static void Function(const void* object, const void* method, P1 p1) {
    FunctionPtr callback;
    memcpy(&callback, method, sizeof(callback));
    callback(p1);
  }
};

template < typename P1 >
class TFunctor1FromFunction {
public:
  typedef void (*FunctionPtr)(P1);

  static TFunctor1< P1 > Make(FunctionPtr function) {
    char functionData[sizeof(function)];
    memcpy(functionData, &function, sizeof(function));
    return TFunctor1< P1 >(TStaticCallback1< P1 >::Function, nullptr, functionData,
                           sizeof(function));
  }
};

// Prime-correlated names; the Echoes bridge forwards one argument through a member pointer.
template < class T, typename P1 >
class TNonStaticCallback1 {
public:
  typedef void (T::*MethodPtr)(P1);

  static void Function(const void* object, const void* method, P1 p1) {
    MethodPtr callback;
    memcpy(&callback, method, sizeof(callback));
    (static_cast< T* >(const_cast< void* >(object))->*callback)(p1);
  }
};

template < class T, typename P1 >
class TFunctor1FromMethod {
public:
  typedef void (T::*MethodPtr)(P1);

  static TFunctor1< P1 > Make(T& object, MethodPtr method) {
    char methodData[sizeof(method)];
    memcpy(methodData, &method, sizeof(method));
    return TFunctor1< P1 >(TNonStaticCallback1< T, P1 >::Function, &object, methodData,
                           sizeof(method));
  }
};

template < class Arg1, class Arg2 >
class TFunctor2 {
public:
  typedef void (*Functor)(const void* object, const void* method, Arg1 arg1, Arg2 arg2);

  TFunctor2() : mFunctor(nullptr), mObject(nullptr) {}

  TFunctor2(Functor functor, const void* object, const void* method, int size)
  : mFunctor(functor), mObject(object), mMethod(method, size) {}

  void operator()(Arg1 arg1, Arg2 arg2) const {
    mFunctor(mObject, mMethod.GetMethodPointer(), arg1, arg2);
  }

  operator bool() const { return !mMethod.IsNull(); }

private:
  Functor mFunctor;
  const void* mObject;
  CMethodPtrStore mMethod;
};

// Prime-correlated names; the Echoes bridge copies and invokes a native member pointer.
template < class T, typename P1, typename P2 >
class TNonStaticCallback2 {
public:
  typedef void (T::*MethodPtr)(P1, P2);

  static void Function(const void* object, const void* method, P1 p1, P2 p2) {
    MethodPtr callback;
    memcpy(&callback, method, sizeof(callback));
    (static_cast< T* >(const_cast< void* >(object))->*callback)(p1, p2);
  }
};

template < class T, typename P1, typename P2 >
class TFunctor2FromMethod {
public:
  typedef void (T::*MethodPtr)(P1, P2);

  static TFunctor2< P1, P2 > Make(T& object, MethodPtr method) {
    char methodData[sizeof(method)];
    memcpy(methodData, &method, sizeof(method));
    return TFunctor2< P1, P2 >(TNonStaticCallback2< T, P1, P2 >::Function, &object, methodData,
                               sizeof(method));
  }
};

// Guessed name, following TFunctor1/TFunctor2: the three-argument callback stored by TSignal3.
template < class Arg1, class Arg2, class Arg3 >
class TFunctor3 {
public:
  typedef void (*Functor)(const void* object, const void* method, Arg1 arg1, Arg2 arg2, Arg3 arg3);

  TFunctor3() : mFunctor(nullptr), mObject(nullptr) {}

  TFunctor3(Functor functor, const void* object, const void* method, int size)
  : mFunctor(functor), mObject(object), mMethod(method, size) {}

  void operator()(Arg1 arg1, Arg2 arg2, Arg3 arg3) const {
    mFunctor(mObject, mMethod.GetMethodPointer(), arg1, arg2, arg3);
  }

  operator bool() const { return !mMethod.IsNull(); }

private:
  Functor mFunctor;
  const void* mObject;
  CMethodPtrStore mMethod;
};

CHECK_SIZEOF(CMethodPtrStore, 0x10)

#endif // _TFUNCTOR_HPP
