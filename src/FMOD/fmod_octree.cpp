// G2MEAB prototype translation unit; reconstruction of all 14 retained native functions.
// G2MEAB .text: 0x8060D428..0x8060E934, in this order: HighestBit 0x8060D428, aabbAdd 0x8060D45C,
// insertItem 0x8060D508, deleteItem 0x8060D68C, updateItem 0x8060D8D8, getAABB 0x8060DA94,
// addInternalNode 0x8060DB48, testLine 0x8060DB80, insertInternal 0x8060DC0C, addToFreeList 0x8060DFC4,
// getFreeNode 0x8060DFF8, adjustAABBs 0x8060E034, addListItem 0x8060E134 and the recursive testLine
// 0x8060E248. The constructor, destructor, setMaxSize, removeInternalNode and removeListItem are not
// retained and stay empty placeholders.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_octree.h"
#include "fmod.h"

namespace FMOD {

static inline float MAX(float x, float y)
{
    return x > y ? x : y;
}

static inline float MIN(float x, float y)
{
    return x < y ? x : y;
}

static unsigned int HighestBit(unsigned int value)
{
    unsigned int mask = value >> 1;

    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;
    return value & ~mask;
}

void aabbAdd(FMOD_AABB & a, FMOD_AABB & b, FMOD_AABB & dst)
{
    dst.xMin = MIN(a.xMin, b.xMin);
    dst.xMax = MAX(a.xMax, b.xMax);
    dst.yMin = MIN(a.yMin, b.yMin);
    dst.yMax = MAX(a.yMax, b.yMax);
    dst.zMin = MIN(a.zMin, b.zMin);
    dst.zMax = MAX(a.zMax, b.zMax);
}

void Octree::insertItem(OctreeNode * item)
{
    float size;

    if (item->flags & OCTREE_FLAG_INSERTED)
    {
        return;
    }

    item->flags |= OCTREE_FLAG_INSERTED | OCTREE_FLAG_ITEM;

    size = MAX(MAX(item->aabb.xMax - item->aabb.xMin, item->aabb.yMax - item->aabb.yMin), item->aabb.zMax - item->aabb.zMin);
    item->splitLevel = HighestBit(ftoint(size * (1073741824.0f * mScale)));
    item->pos[0] = xGetCenter(item);
    item->pos[1] = yGetCenter(item);
    item->pos[2] = zGetCenter(item);

    if (!mRoot)
    {
        mRoot = item;
    }
    else
    {
        insertInternal(mRoot, item);
    }
}

void Octree::deleteItem(OctreeNode * item)
{
    OctreeNode * sibling;

    if (!(item->flags & OCTREE_FLAG_INSERTED))
    {
        return;
    }

    if (!item->parent)
    {
        mRoot = item->nextItem;
        if (mRoot)
        {
            mRoot->parent = 0;
            mRoot->flags &= ~OCTREE_FLAG_LISTITEM;
        }
        item->nextItem = 0;
        item->flags &= ~(OCTREE_FLAG_FREE | OCTREE_FLAG_INSERTED);
        return;
    }

    sibling = 0;
    if (item->parent->nextItem == item)
    {
        item->parent->nextItem = item->nextItem;
        if (item->nextItem)
        {
            item->nextItem->parent = item->parent;
        }
    }
    else if (item->parent->hi == item)
    {
        item->parent->hi = item->nextItem;
        if (item->nextItem)
        {
            item->nextItem->flags &= ~OCTREE_FLAG_LISTITEM;
            item->nextItem->parent = item->parent;
        }
        else
        {
            sibling = item->parent->lo;
        }
    }
    else
    {
        item->parent->lo = item->nextItem;
        if (item->nextItem)
        {
            item->nextItem->flags &= ~OCTREE_FLAG_LISTITEM;
            item->nextItem->parent = item->parent;
        }
        else
        {
            sibling = item->parent->hi;
        }
    }

    if (sibling)
    {
        OctreeNode * node = item->parent;
        OctreeNode * list = node->nextItem;
        OctreeNode * newParent;

        if (!node->parent)
        {
            mRoot = sibling;
            sibling->parent = 0;
            newParent = mRoot;
        }
        else
        {
            if (node->parent->hi == node)
            {
                node->parent->hi = sibling;
            }
            else
            {
                node->parent->lo = sibling;
            }
            sibling->parent = node->parent;
            newParent = node->parent;
        }

        while (list)
        {
            OctreeNode * next = list->nextItem;

            list->nextItem = 0;
            list->parent = 0;
            list->flags &= ~OCTREE_FLAG_LISTITEM;
            insertInternal(newParent, list);
            list = next;
        }

        if (node->parent)
        {
            adjustAABBs(node->parent);
        }

        node->parent = 0;
        node->hi = 0;
        node->lo = 0;
        node->nextItem = 0;
        node->flags &= ~(OCTREE_FLAG_ITEM | OCTREE_FLAG_AABB_VALID);
        addToFreeList(node);
    }
    else if (!(item->parent->flags & OCTREE_FLAG_ITEM))
    {
        adjustAABBs(item->parent);
    }

    item->parent = 0;
    item->nextItem = 0;
    item->flags &= ~(OCTREE_FLAG_FREE | OCTREE_FLAG_INSERTED);
}

void Octree::updateItem(OctreeNode * item)
{
    if (item->flags & OCTREE_FLAG_INSERTED)
    {
        float size;
        unsigned int splitLevel;
        unsigned int x;
        unsigned int y;
        unsigned int z;
        unsigned int mask;

        size = MAX(MAX(item->aabb.xMax - item->aabb.xMin, item->aabb.yMax - item->aabb.yMin), item->aabb.zMax - item->aabb.zMin);
        splitLevel = HighestBit(ftoint(size * (1073741824.0f * mScale)));
        x = xGetCenter(item);
        y = yGetCenter(item);
        z = zGetCenter(item);
        mask = ~(item->splitLevel - 1);

        if (splitLevel == item->splitLevel && (x & mask) == (item->pos[0] & mask) && (y & mask) == (item->pos[1] & mask) && (z & mask) == (item->pos[2] & mask))
        {
            adjustAABBs(item);
            return;
        }

        deleteItem(item);
    }

    insertItem(item);
}

void Octree::getAABB(FMOD_AABB * aabb)
{
    if (mRoot)
    {
        OctreeNode * item;

        aabb->xMin = mRoot->aabb.xMin;
        aabb->xMax = mRoot->aabb.xMax;
        aabb->yMin = mRoot->aabb.yMin;
        aabb->yMax = mRoot->aabb.yMax;
        aabb->zMin = mRoot->aabb.zMin;
        aabb->zMax = mRoot->aabb.zMax;

        for (item = mRoot->nextItem; item; item = item->nextItem)
        {
            aabbAdd(item->aabb, *aabb, *aabb);
        }
    }
    else
    {
        aabb->xMin = 0.0f;
        aabb->xMax = 0.0f;
        aabb->yMin = 0.0f;
        aabb->yMax = 0.0f;
        aabb->zMin = 0.0f;
        aabb->zMax = 0.0f;
    }
}

void Octree::addInternalNode(OctreeNode * item)
{
    if (!(item->flags & OCTREE_FLAG_INTERNALNODE))
    {
        item->flags |= OCTREE_FLAG_INTERNALNODE;
        addToFreeList(item);
    }
}

bool Octree::testLine(bool (* octreeLineTestCallback)(OctreeNode *, void *), void * data, const FMOD_VECTOR & a, const FMOD_VECTOR & b)
{
    RecursionData recursionData;

    if (!mRoot)
    {
        return true;
    }

    recursionData.callback = octreeLineTestCallback;
    recursionData.data = data;
    recursionData.stop = false;
    testLine(mRoot, a, b, &recursionData);

    return !recursionData.stop;
}

void Octree::insertInternal(OctreeNode * node, OctreeNode * item)
{
    for (;;)
    {
        int axis = 0;
        unsigned int highest = 0;
        int i;

        if (node->flags & OCTREE_FLAG_ITEM)
        {
            for (i = 0; i < 3; i++)
            {
                unsigned int bit = HighestBit(node->pos[i] ^ item->pos[i]);

                if (bit > highest && bit > item->splitLevel && bit > node->splitLevel)
                {
                    highest = bit;
                    axis = i;
                }
            }
        }
        else
        {
            for (i = 0; i < 3; i++)
            {
                unsigned int bit = HighestBit((node->pos[i] ^ item->pos[i]) & ~(node->splitLevel - 1));

                if (bit > highest && bit > item->splitLevel &&
                    (bit > node->splitLevel || (bit == node->splitLevel && i < (node->flags & OCTREE_FLAG_AXIS_MASK))))
                {
                    highest = bit;
                    axis = i;
                }
            }
        }

        if (highest)
        {
            OctreeNode * newNode = getFreeNode();
            OctreeNode * list;

            newNode->flags |= (newNode->flags & ~OCTREE_FLAG_AXIS_MASK) | axis;
            newNode->splitLevel = highest;
            if (item->pos[axis] & highest)
            {
                newNode->lo = node;
                newNode->hi = item;
            }
            else
            {
                newNode->hi = node;
                newNode->lo = item;
            }

            newNode->parent = node->parent;
            newNode->hi->parent = newNode;
            newNode->lo->parent = newNode;
            if (newNode->parent)
            {
                if (newNode->parent->lo == node)
                {
                    newNode->parent->lo = newNode;
                }
                else
                {
                    newNode->parent->hi = newNode;
                }
            }
            else
            {
                mRoot = newNode;
            }

            if (axis == 0)
            {
                newNode->pos[0] = (item->pos[0] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
                newNode->pos[1] = (item->pos[1] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
                newNode->pos[2] = (item->pos[2] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
            }
            else if (axis == 1)
            {
                newNode->pos[0] = (item->pos[0] & ~((newNode->splitLevel >> 1) - 1)) | (newNode->splitLevel >> 1);
                newNode->pos[1] = (item->pos[1] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
                newNode->pos[2] = (item->pos[2] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
            }
            else
            {
                newNode->pos[0] = (item->pos[0] & ~((newNode->splitLevel >> 1) - 1)) | (newNode->splitLevel >> 1);
                newNode->pos[1] = (item->pos[1] & ~((newNode->splitLevel >> 1) - 1)) | (newNode->splitLevel >> 1);
                newNode->pos[2] = (item->pos[2] & ~(newNode->splitLevel - 1)) | newNode->splitLevel;
            }

            list = node->nextItem;
            node->nextItem = 0;
            adjustAABBs(newNode);

            while (list)
            {
                OctreeNode * next = list->nextItem;

                list->parent = 0;
                list->nextItem = 0;
                list->flags &= ~OCTREE_FLAG_LISTITEM;
                if (newNode->parent)
                {
                    insertInternal(newNode->parent, list);
                }
                else
                {
                    insertInternal(mRoot, list);
                }
                list = next;
            }
            return;
        }

        if (item->splitLevel >= node->splitLevel)
        {
            addListItem(node, item);
            adjustAABBs(node);
            return;
        }

        if (node->flags & OCTREE_FLAG_ITEM)
        {
            OctreeNode * parent = node->parent;

            addListItem(node, item);
            adjustAABBs(parent);
            return;
        }

        if (item->pos[node->flags & OCTREE_FLAG_AXIS_MASK] >= node->pos[node->flags & OCTREE_FLAG_AXIS_MASK])
        {
            node = node->hi;
        }
        else
        {
            node = node->lo;
        }
    }
}

void Octree::addToFreeList(OctreeNode * item)
{
    item->nextItem = mFreeList;
    mFreeList = item;
    if (item->nextItem)
    {
        item->nextItem->parent = item;
    }
    item->parent = 0;
    item->flags |= OCTREE_FLAG_FREE;
}

OctreeNode * Octree::getFreeNode()
{
    OctreeNode * node = mFreeList;

    mFreeList = node->nextItem;
    if (mFreeList)
    {
        mFreeList->parent = 0;
    }
    node->nextItem = 0;
    node->flags &= ~OCTREE_FLAG_FREE;
    return node;
}

void Octree::adjustAABBs(OctreeNode * node)
{
    while (node)
    {
        if (!(node->flags & OCTREE_FLAG_ITEM))
        {
            OctreeNode * item;

            aabbAdd(node->hi->aabb, node->lo->aabb, node->aabb);
            for (item = node->nextItem; item; item = item->nextItem)
            {
                aabbAdd(item->aabb, node->aabb, node->aabb);
            }
            node->flags |= OCTREE_FLAG_AABB_VALID;

            if (node->hi->flags & OCTREE_FLAG_ITEM)
            {
                for (item = node->hi->nextItem; item; item = item->nextItem)
                {
                    aabbAdd(item->aabb, node->aabb, node->aabb);
                }
            }
            if (node->lo->flags & OCTREE_FLAG_ITEM)
            {
                for (item = node->lo->nextItem; item; item = item->nextItem)
                {
                    aabbAdd(item->aabb, node->aabb, node->aabb);
                }
            }
        }
        node = node->parent;
    }
}

void Octree::addListItem(OctreeNode * list, OctreeNode * node)
{
    if (list->flags & OCTREE_FLAG_ITEM)
    {
        if (!list->nextItem)
        {
            list->nextItem = node;
            node->parent = list;
            node->flags |= OCTREE_FLAG_LISTITEM;
            return;
        }
        list = list->nextItem;
    }

    while (node->splitLevel > list->splitLevel && list->nextItem)
    {
        list = list->nextItem;
    }

    if (!list->nextItem && node->splitLevel > list->splitLevel)
    {
        list->nextItem = node;
        node->parent = list;
        node->flags |= OCTREE_FLAG_LISTITEM;
        return;
    }

    if (!list->parent)
    {
        mRoot = node;
    }
    else if (list->parent->nextItem == list)
    {
        list->parent->nextItem = node;
        if (list->parent->flags & list->flags & OCTREE_FLAG_ITEM)
        {
            node->flags |= OCTREE_FLAG_LISTITEM;
        }
    }
    else if (list->parent->hi == list)
    {
        list->parent->hi = node;
    }
    else
    {
        list->parent->lo = node;
    }

    node->parent = list->parent;
    node->nextItem = list;
    list->parent = node;
    list->flags |= OCTREE_FLAG_LISTITEM;
}

static inline bool clipLine(float da, float db, FMOD_VECTOR & a, FMOD_VECTOR & b)
{
    if (da < 0.0f && db > 0.0f)
    {
        float t = da / (da - db);

        a.x = a.x + t * (b.x - a.x);
        a.y = a.y + t * (b.y - a.y);
        a.z = a.z + t * (b.z - a.z);
    }
    else if (da > 0.0f && db < 0.0f)
    {
        float t = db / (db - da);

        b.x = b.x + t * (a.x - b.x);
        b.y = b.y + t * (a.y - b.y);
        b.z = b.z + t * (a.z - b.z);
    }
    else if (da < 0.0f && db < 0.0f)
    {
        return false;
    }

    return true;
}

void Octree::testLine(OctreeNode * node, FMOD_VECTOR a, FMOD_VECTOR b, RecursionData * recursionData)
{
    OctreeNode * item;

    for (item = node->nextItem; item; item = item->nextItem)
    {
        if (!recursionData->callback(item, recursionData->data))
        {
            recursionData->stop = true;
            return;
        }
    }

    if (!clipLine(a.x - node->aabb.xMin, b.x - node->aabb.xMin, a, b))
    {
        return;
    }
    if (!clipLine(node->aabb.xMax - a.x, node->aabb.xMax - b.x, a, b))
    {
        return;
    }
    if (!clipLine(a.y - node->aabb.yMin, b.y - node->aabb.yMin, a, b))
    {
        return;
    }
    if (!clipLine(node->aabb.yMax - a.y, node->aabb.yMax - b.y, a, b))
    {
        return;
    }
    if (!clipLine(a.z - node->aabb.zMin, b.z - node->aabb.zMin, a, b))
    {
        return;
    }
    if (!clipLine(node->aabb.zMax - a.z, node->aabb.zMax - b.z, a, b))
    {
        return;
    }

    if (node->flags & OCTREE_FLAG_ITEM)
    {
        if (!recursionData->callback(node, recursionData->data))
        {
            recursionData->stop = true;
        }
        return;
    }

    if (node->hi)
    {
        testLine(node->hi, a, b, recursionData);
        if (recursionData->stop)
        {
            return;
        }
    }

    if (node->lo)
    {
        testLine(node->lo, a, b, recursionData);
        if (recursionData->stop)
        {
            return;
        }
    }
}

Octree::Octree(float worldSize)
{
}

Octree::~Octree()
{
}

void Octree::removeInternalNode(OctreeNode * item)
{
}

void Octree::setMaxSize(float maxSize)
{
}

void Octree::removeListItem(OctreeNode * node)
{
}

} // namespace FMOD
