#ifndef _CARCHITECTUREQUEUE
#define _CARCHITECTUREQUEUE

#include "types.h"

#include "MetroidPrime/CArchitectureMessage.hpp"

#include "rstl/list.hpp"

// Echoes layout: a list of messages. Unlike Echoes, where Push is out of line, the prototype
// inlines Push, so main.cpp emits the list's push_back chain (0x8000B55C..0x8000B614).
class CArchitectureQueue {
public:
  void Push(const CArchitectureMessage& msg) { mQueue.push_back(msg); }

private:
  rstl::list< CArchitectureMessage > mQueue;
};

#endif // _CARCHITECTUREQUEUE
