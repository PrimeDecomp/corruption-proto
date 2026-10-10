// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_STACK_H
#define _FMOD_STACK_H

namespace FMOD {
    class Stack;
}

namespace FMOD {

class Stack
{
    int mLSPos; // offset 0x0
    bool mTempUsed; // offset 0x4
public:
    unsigned int mMramAddress; // offset 0x8
    Stack * mTop; // offset 0xC
    Stack * mNext; // offset 0x10
    Stack * mPrevious; // offset 0x14
    unsigned int mSize; // offset 0x18
    unsigned int mNextSize; // offset 0x1C
    unsigned int mPrevSize; // offset 0x20
    Stack();
    void push(Stack * object, int size);
    Stack * pop();
    Stack * peek();
    bool empty();
    void dump();
};

} // namespace FMOD

#endif
