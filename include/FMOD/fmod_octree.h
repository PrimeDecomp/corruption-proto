// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. OctreeNode (0x3C) and Octree (0x18) match the G2MEAB accesses in fmod_octree.

#ifndef _FMOD_OCTREE_H
#define _FMOD_OCTREE_H

#include "fmod.h"

struct FMOD_VECTOR;
namespace FMOD {
    struct FMOD_AABB;
    struct Octree;
    struct OctreeNode;
    struct RecursionData;
}

namespace FMOD {

struct FMOD_AABB
{
    float xMin; // offset 0x0
    float xMax; // offset 0x4
    float yMin; // offset 0x8
    float yMax; // offset 0xC
    float zMin; // offset 0x10
    float zMax; // offset 0x14
};

// OctreeNode::flags bits (all names guessed): the low two bits hold an internal node's split axis.
enum
{
    OCTREE_FLAG_AXIS_MASK = 0x3, // Guessed name; insertInternal 0x8060DC0C
    OCTREE_FLAG_ITEM = 0x4, // Guessed name; set by insertItem, tested by adjustAABBs 0x8060E034
    OCTREE_FLAG_AABB_VALID = 0x8, // Guessed name; set by adjustAABBs
    OCTREE_FLAG_LISTITEM = 0x10, // Guessed name; set by addListItem 0x8060E134
    OCTREE_FLAG_FREE = 0x20, // Guessed name; addToFreeList 0x8060DFC4 / getFreeNode 0x8060DFF8
    OCTREE_FLAG_INSERTED = 0x40, // Guessed name; insertItem 0x8060D508 / deleteItem 0x8060D68C
    OCTREE_FLAG_INTERNALNODE = 0x400 // Guessed name; addInternalNode 0x8060DB48
};

// Line query state: Octree::testLine 0x8060DB80 builds it on the stack; the recursive testLine
// 0x8060E248 calls the callback with the data word and sets the flag to stop.
struct RecursionData
{
    bool (* callback)(OctreeNode *, void *); // offset 0x0, Guessed name
    void * data; // offset 0x4, Guessed name
    bool stop; // offset 0x8, Guessed name
};

struct OctreeNode
{
    FMOD_AABB aabb; // offset 0x0
    int flags; // offset 0x18
    unsigned int splitLevel; // offset 0x1C
    unsigned int pos[3]; // offset 0x20
    OctreeNode * parent; // offset 0x2C
    OctreeNode * hi; // offset 0x30
    OctreeNode * lo; // offset 0x34
    OctreeNode * nextItem; // offset 0x38
};

struct Octree
{
    Octree(float worldSize);
    ~Octree();
    OctreeNode * mRoot; // offset 0x0
    FMOD_VECTOR mCenter; // offset 0x4
    float mScale; // offset 0x10
    OctreeNode * mFreeList; // offset 0x14
    void insertItem(OctreeNode * item);
    void deleteItem(OctreeNode * item);
    void updateItem(OctreeNode * item);
    bool needsReinsert(OctreeNode *);
    void updateItemAABB(OctreeNode *);
    void getAABB(FMOD_AABB * aabb);
    void addInternalNode(OctreeNode * item);
    void removeInternalNode(OctreeNode * item);
    void setMaxSize(float maxSize);
    bool testLine(bool (* octreeLineTestCallback)(OctreeNode *, void *), void * data, const FMOD_VECTOR & a, const FMOD_VECTOR & b);
    // Inline in G2MEAB (expanded in insertItem 0x8060D508 and updateItem 0x8060D8D8).
    unsigned int ftoint(float value)
    {
        return (int)value;
    }
    unsigned int xGetCenter(OctreeNode * node)
    {
        return ftoint(1073741824.0f + 1073741824.0f * (mScale * (0.5f * (node->aabb.xMin + node->aabb.xMax) - mCenter.x)));
    }
    unsigned int yGetCenter(OctreeNode * node)
    {
        return ftoint(1073741824.0f + 1073741824.0f * (mScale * (0.5f * (node->aabb.yMin + node->aabb.yMax) - mCenter.y)));
    }
    unsigned int zGetCenter(OctreeNode * node)
    {
        return ftoint(1073741824.0f + 1073741824.0f * (mScale * (0.5f * (node->aabb.zMin + node->aabb.zMax) - mCenter.z)));
    }
    void insertInternal(OctreeNode * node, OctreeNode * item);
    void addToFreeList(OctreeNode * item);
    OctreeNode * getFreeNode();
    void adjustAABBs(OctreeNode * node);
    void removeListItem(OctreeNode * node);
    void addListItem(OctreeNode * list, OctreeNode * node);
    static void testLine(OctreeNode * node, FMOD_VECTOR a, FMOD_VECTOR b, RecursionData * recursionData);
};

void aabbAdd(FMOD_AABB & a, FMOD_AABB & b, FMOD_AABB & dst);

} // namespace FMOD

#endif
