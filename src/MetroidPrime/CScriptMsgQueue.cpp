// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x801E5D34..0x801E5F8C (5 native functions). Listed below are the ones not yet
// implemented.
// Source identity: asserted original basename.
// Complete proposed native interval retained; historical source arrangement inferred.
// 0x801E5E28 +0x54: script-message record copy helper (CScriptMsg's implicit copy constructor)
// 0x801E5F5C +0x30: registered initializer; .ctors 0x8065B7F8; native size 0x30

#include "MetroidPrime/CScriptMsgQueue.hpp"

#include "Kyoto/Alloc/Assert.hpp"

void CScriptMsgQueue::Push(const CScriptMsg& msg) {
  uint next = (mHead + 1) % kCapacity;
  RS_VERIFY_THROW(16, next != mTail, false, "Overflow in ScriptMsgQueue");
  mMsgs[mHead] = msg;
  mHead = next;
}

CScriptMsg CScriptMsgQueue::Pop() {
  RS_VERIFY_THROW(23, mHead != mTail, false, "Pop from empty queue in ScriptMsgQueue");
  uint idx = mTail;
  mTail = (mTail + 1) % kCapacity;
  return mMsgs[idx];
}

int CScriptMsgQueue::Size() const {
  if (mHead < mTail) {
    return kCapacity - mTail + mHead;
  }
  return mHead - mTail;
}
