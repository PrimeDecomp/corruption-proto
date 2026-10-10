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
    // Inline bodies (group D): DSPI ctor 0x806070B8 and the connection pool 0x805F3F7C..0x805F44A0
    // inline every one of these; store orders follow those expansions.
    LinkedListNode()
    {
        initNode();
    }
    virtual ~LinkedListNode() {} // vptr offset 0x10; inlined into derived dtors (0x805F44A0), weak copy 0x805B6184
    LinkedListNode * getNext() { return mNodeNext; }
    LinkedListNode * getPrev() { return mNodePrev; }
    void setData(void * data) { mNodeData = data; }
    void * getData() { return mNodeData; }
    void initNode()
    {
        mNodePriority = -1;
        mNodeNext = this;
        mNodePrev = this;
        mNodeData = 0;
    }
    void removeNode()
    {
        mNodePrev->mNodeNext = mNodeNext;
        mNodeNext->mNodePrev = mNodePrev;
        mNodeNext = mNodePrev = this;
        mNodePriority = -1;
        mNodeData = 0;
    }
    void addAfter(LinkedListNode * node)
    {
        mNodeNext = node->mNodeNext;
        mNodePrev = node;
        mNodeNext->mNodePrev = this;
        mNodePrev->mNodeNext = this;
    }
    void addBefore(LinkedListNode * node)
    {
        mNodePrev = node->mNodePrev;
        mNodeNext = node;
        mNodeNext->mNodePrev = this;
        mNodePrev->mNodeNext = this;
    }
    // Inline in G2MEAB (expanded in ChannelI::updatePosition 0x805BD08C and SoundI::addSyncPoint
    // 0x80618D88); 4.06 SortedLinkedListNode::addAt.
    void addAt(LinkedListNode * head, unsigned int priority)
    {
        LinkedListNode * current = head->mNodeNext;

        do
        {
            if (current->mNodePriority > priority)
            {
                mNodePriority = priority;
                addBefore(current);
                return;
            }

            current = current->mNodeNext;
        } while (current->mNodePrev != head);
    }
    // Inline in G2MEAB (group A: expanded in SystemI::validate 0x8061A870).
    bool exists(LinkedListNode * node)
    {
        LinkedListNode * current = mNodeNext;

        do
        {
            if (current == node)
            {
                return true;
            }
            current = current->mNodeNext;
        } while (current != this);

        return false;
    }
    bool isEmpty() { return mNodeNext == this && mNodePrev == this; }
    int count();
    // Inline in G2MEAB (group A: expanded in Metadata::getTag 0x8060C614).
    LinkedListNode * getNodeByIndex(int i)
    {
        LinkedListNode * current = mNodeNext;
        int count;

        if (current == this)
        {
            return 0;
        }

        for (count = 0; count < i; count++)
        {
            current = current->mNodeNext;
            if (current == this)
            {
                return 0;
            }
        }

        return current;
    }
    // Inline in G2MEAB (group A: expanded in PluginFactory::registerCodec 0x80610F2C).
    int getNodeIndex(LinkedListNode * node)
    {
        LinkedListNode * current = mNodeNext;
        int index = 0;

        while (current != this)
        {
            if (current == node)
            {
                return index;
            }
            index++;
            current = current->mNodeNext;
        }

        return -1;
    }
};

} // namespace FMOD

#endif
