#ifndef _CSCRIPTMSGQUEUE
#define _CSCRIPTMSGQUEUE

#include "MetroidPrime/CEntityInfo.hpp"

// Class name from CScriptMsgQueue.cpp's asserts, which also name mHead and mTail. This is
// Echoes' CStateManager::ScriptMsgArray: a ring of 0xC0 messages with a write index (mHead) and
// a read index (mTail). CStateManagerObject allocates it (0x1808 bytes) and keeps a pointer to it
// at 0x10D8, where Echoes embedded the array in CStateManager.
class CScriptMsgQueue {
public:
  enum { kCapacity = 0xC0 };

  void Push(const CScriptMsg& msg); // Guessed name; asserts "Overflow in ScriptMsgQueue"
  CScriptMsg Pop();                 // Named by its "Pop from empty queue" assert
  int Size() const;                 // Guessed name
  // Echoes' ScriptMsgArray::empty; the message pump (0x80298594) inlines it.
  bool empty() const { return mHead == mTail; }

private:
  CScriptMsg mMsgs[kCapacity];
  uint mHead;
  uint mTail;
};
CHECK_SIZEOF(CScriptMsgQueue, 0x1808)

#endif // _CSCRIPTMSGQUEUE
