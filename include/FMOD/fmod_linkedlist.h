// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. LinkedListNode is the G2MEAB layout.
// G2MEAB has one 0x14-byte polymorphic node type where 4.06 has a 0xC LinkedListNode and a 0x10
// SortedLinkedListNode. Offset comments in headers that embed or derive from a node are still the
// 4.06 values unless a header says otherwise.

#ifndef _FMOD_LINKEDLIST_H
#define _FMOD_LINKEDLIST_H

namespace FMOD {
    class LinkedListNode;
}

namespace FMOD {

// G2MEAB: constructors (e.g. ChannelI fn_805BDCB8) self-link the node, store -1 at +0xC and the
// vtable 0x806E25B4 at +0x10; sorted and plain channel nodes share that vtable, so the 4.06
// SortedLinkedListNode is not a separate dynamic type here. The vtable holds only the destructor
// 0x805B6184.
class LinkedListNode
{
    LinkedListNode * mNodeNext; // offset 0x0
    LinkedListNode * mNodePrev; // offset 0x4
    void * mNodeData; // offset 0x8
    unsigned int mNodePriority; // offset 0xC, Guessed name (4.06 SortedLinkedListNode member)
public:
    LinkedListNode();
    virtual ~LinkedListNode(); // vptr offset 0x10
    LinkedListNode * getNext();
    LinkedListNode * getPrev();
    void setData(void * data);
    void * getData();
    void initNode();
    void removeNode();
    void addAfter(LinkedListNode * node);
    void addBefore(LinkedListNode * node);
    void addAt(LinkedListNode * head, unsigned int priority);
    bool exists(LinkedListNode * node);
    bool isEmpty();
    int count();
    LinkedListNode * getNodeByIndex(int i);
    int getNodeIndex(LinkedListNode * node);
};

} // namespace FMOD

#endif
