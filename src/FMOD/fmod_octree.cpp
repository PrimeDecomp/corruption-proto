// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x8060D428..0x8060E934 (14 retained native functions).
// inferred descriptive source basename; original filename unverified.
// Evidence: Complete quantized spatial-tree family: highest-set-bit helperD428, six-float AABB
// unionD45C, insertD508/removeD68C/updateD8D8, boundsDA94, releaseDB48, ray queryDB80 and recursive
// hierarchy insertionDC0C. Geometry callers8060AEE4 and8060A4E4/8060B038 use insert/query,
// establishing independence from music and output objects. Preserve retained helpers, thunks and
// inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_octree.h"
#include "fmod.h"

namespace FMOD {

static float MAX(float x, float y)
{
}

static float MIN(float x, float y)
{
}

static unsigned int HighestBit(unsigned int value)
{
}

void aabbAdd(FMOD_AABB & a, FMOD_AABB & b, FMOD_AABB & dst)
{
}

void Octree::adjustAABBs(OctreeNode * node)
{
}

Octree::Octree(float worldSize)
{
}

Octree::~Octree()
{
}

void Octree::addToFreeList(OctreeNode * item)
{
}

void Octree::getAABB(FMOD_AABB * aabb)
{
}

void Octree::addInternalNode(OctreeNode * item)
{
}

OctreeNode * Octree::getFreeNode()
{
}

void Octree::removeInternalNode(OctreeNode * item)
{
}

void Octree::setMaxSize(float maxSize)
{
}

void Octree::testLine(OctreeNode * node, FMOD_VECTOR a, FMOD_VECTOR b, RecursionData * recursionData)
{
}

bool Octree::testLine(bool (* octreeLineTestCallback)(OctreeNode *, void *), void * data, const FMOD_VECTOR & a, const FMOD_VECTOR & b)
{
}

void Octree::addListItem(OctreeNode * list, OctreeNode * node)
{
}

void Octree::insertInternal(OctreeNode * node, OctreeNode * item)
{
}

void Octree::deleteItem(OctreeNode * item)
{
}

void Octree::insertItem(OctreeNode * item)
{
}

void Octree::updateItem(OctreeNode * item)
{
}

void Octree::removeListItem(OctreeNode * node)
{
}

} // namespace FMOD
