// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    unsigned int ftoint(float);
    unsigned int xGetCenter(OctreeNode * node);
    unsigned int yGetCenter(OctreeNode * node);
    unsigned int zGetCenter(OctreeNode * node);
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
