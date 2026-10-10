// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_LINKEDLIST_H
#define _FMOD_LINKEDLIST_H

namespace FMOD {
    class LinkedListNode;
    class SortedLinkedListNode;
}

namespace FMOD {

// Open question (G2MEAB): FMOD_CODEC_DESCRIPTION_EX suggests the node may be 16 bytes, not the 4.06 12 bytes.
class LinkedListNode
{
    LinkedListNode * mNodeNext; // offset 0x0
    LinkedListNode * mNodePrev; // offset 0x4
    void * mNodeData; // offset 0x8
public:
    LinkedListNode();
    LinkedListNode * getNext();
    LinkedListNode * getPrev();
    void setData(void * data);
    void * getData();
    void initNode();
    void removeNode();
    void addAfter(LinkedListNode * node);
    void addBefore(LinkedListNode * node);
    bool exists(LinkedListNode * node);
    bool isEmpty();
    int count();
    LinkedListNode * getNodeByIndex(int i);
    int getNodeIndex(LinkedListNode * node);
};

class SortedLinkedListNode : public LinkedListNode
{
    unsigned int mNodePriority; // offset 0xC
public:
    SortedLinkedListNode();
    void initNode();
    void removeNode();
    void addAt(SortedLinkedListNode * head, unsigned int priority);
};

} // namespace FMOD

#endif
