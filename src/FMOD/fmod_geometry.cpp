// G2MEAB prototype translation unit; reconstruction of all 10 retained native functions.
// G2MEAB .text: 0x8060A3C8..0x8060B08C. The unit holds the retained GeometryMgr and GeometryI code (the
// 4.06 fmod_geometry_mgr.cpp and fmod_geometryi.cpp functions that survive dead stripping), in this order:
// GeometryMgr() 0x8060A3C8, ~GeometryMgr() 0x8060A3E8 (called by the SystemI destructor for +0xFC8),
// mainOctreeLineTestCallback 0x8060A42C, lineTestAll 0x8060A458, flushAll 0x8060A51C, matrixMult
// 0x8060A57C, GeometryI::updateSpacialData 0x8060A5F8, octreeLineTestCallback 0x8060A808, flush
// 0x8060A9FC and lineTest 0x8060AF3C. The Geometry API wrappers below them are not in G2MEAB and stay
// empty placeholders.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; GeometryI is the G2MEAB layout.

#include "fmod.h"
#include "fmod.hpp"
#include "fmod_geometry_mgr.h"
#include "fmod_geometryi.h"
#include "fmod_octree.h"

#include <math.h>

namespace FMOD {

static inline float MAX(float a, float b)
{
    return a > b ? a : b;
}

static inline float MIN(float a, float b)
{
    return a < b ? a : b;
}

static inline void cross(const FMOD_VECTOR & a, const FMOD_VECTOR & b, FMOD_VECTOR & c)
{
    c.x = a.y * b.z - a.z * b.y;
    c.y = a.z * b.x - a.x * b.z;
    c.z = a.x * b.y - a.y * b.x;
}

GeometryMgr::GeometryMgr()
{
    mSystem = 0;
    mMainOctree = 0;
    mRefCount = 0;
    mFirstUpdateItem = 0;
    mMoved = true;
}

GeometryMgr::~GeometryMgr()
{
}

bool GeometryMgr::mainOctreeLineTestCallback(OctreeNode * item, void * data)
{
    SpaicalData * spacialData = (SpaicalData *)item;
    LineTestData * lineTestData = (LineTestData *)data;

    lineTestData->geometryI = spacialData->geometryI;
    return spacialData->geometryI->lineTest(lineTestData);
}

FMOD_RESULT GeometryMgr::lineTestAll(FMOD_VECTOR * start, FMOD_VECTOR * end, float * directOcclusion, float * reverbOcclusion)
{
    LineTestData lineTestData;

    flushAll();

    lineTestData.start = *start;
    lineTestData.end = *end;
    lineTestData.directTransmission = 1.0f;
    lineTestData.reverbTransmission = 1.0f;
    lineTestData.geometryI = 0;

    if (mMainOctree)
    {
        mMainOctree->testLine(mainOctreeLineTestCallback, &lineTestData, *start, *end);
    }

    *directOcclusion = 1.0f - lineTestData.directTransmission;
    *reverbOcclusion = 1.0f - lineTestData.reverbTransmission;
    return FMOD_OK;
}

FMOD_RESULT GeometryMgr::flushAll()
{
    GeometryI * geometryI = mFirstUpdateItem;

    mFirstUpdateItem = 0;
    while (geometryI)
    {
        GeometryI * next = geometryI->mNextUpdateItem;

        geometryI->mNextUpdateItem = 0;
        geometryI->mToBeUpdated = false;
        geometryI->flush();
        geometryI = next;
    }

    return FMOD_OK;
}

void matrixMult(const float matrix[3][4], const FMOD_VECTOR * in, FMOD_VECTOR * out)
{
    out->x = matrix[0][0] * in->x + matrix[0][1] * in->y + matrix[0][2] * in->z;
    out->y = matrix[1][0] * in->x + matrix[1][1] * in->y + matrix[1][2] * in->z;
    out->z = matrix[2][0] * in->x + matrix[2][1] * in->y + matrix[2][2] * in->z;
}

void GeometryI::updateSpacialData()
{
    FMOD_VECTOR center;
    FMOD_VECTOR worldCenter;
    FMOD_VECTOR extent;
    FMOD_VECTOR worldExtent;

    center.x = 0.5f * (mAABB.xMax + mAABB.xMin);
    center.y = 0.5f * (mAABB.yMax + mAABB.yMin);
    center.z = 0.5f * (mAABB.zMax + mAABB.zMin);
    matrixMult(mMatrix, &center, &worldCenter);
    worldCenter.x += mPosition.x;
    worldCenter.y += mPosition.y;
    worldCenter.z += mPosition.z;

    extent.x = 0.5f * (mAABB.xMax - mAABB.xMin);
    extent.y = 0.5f * (mAABB.yMax - mAABB.yMin);
    extent.z = 0.5f * (mAABB.zMax - mAABB.zMin);
    worldExtent.x = extent.x * (float)fabs(mMatrix[0][0]) + extent.y * (float)fabs(mMatrix[1][0]) + extent.z * (float)fabs(mMatrix[2][0]);
    worldExtent.y = extent.x * (float)fabs(mMatrix[0][1]) + extent.y * (float)fabs(mMatrix[1][1]) + extent.z * (float)fabs(mMatrix[2][1]);
    worldExtent.z = extent.x * (float)fabs(mMatrix[0][2]) + extent.y * (float)fabs(mMatrix[1][2]) + extent.z * (float)fabs(mMatrix[2][2]);

    mSpatialData->octreeNode.aabb.xMax = worldCenter.x + worldExtent.x;
    mSpatialData->octreeNode.aabb.xMin = worldCenter.x - worldExtent.x;
    mSpatialData->octreeNode.aabb.yMax = worldCenter.y + worldExtent.y;
    mSpatialData->octreeNode.aabb.yMin = worldCenter.y - worldExtent.y;
    mSpatialData->octreeNode.aabb.zMax = worldCenter.z + worldExtent.z;
    mSpatialData->octreeNode.aabb.zMin = worldCenter.z - worldExtent.z;

    if (mActive)
    {
        mGeometryMgr->mainOctree()->updateItem(&mSpatialData->octreeNode);
    }
    else
    {
        mGeometryMgr->mainOctree()->deleteItem(&mSpatialData->octreeNode);
    }
}

bool GeometryI::octreeLineTestCallback(OctreeNode * item, void * data)
{
    FMOD_POLYGON * polygon = (FMOD_POLYGON *)item;
    LineTestData * lineTestData = (LineTestData *)data;
    float startDistance;
    float endDistance;
    float t;
    FMOD_VECTOR intersection;
    FMOD_VECTOR * vertices;
    int numVertices;
    int i;

    startDistance = polygon->normal.x * lineTestData->start.x + polygon->normal.y * lineTestData->start.y + polygon->normal.z * lineTestData->start.z - polygon->distance;
    endDistance = polygon->normal.x * lineTestData->end.x + polygon->normal.y * lineTestData->end.y + polygon->normal.z * lineTestData->end.z - polygon->distance;

    if ((startDistance >= 0.0f && endDistance >= 0.0f) || (startDistance <= 0.0f && endDistance <= 0.0f))
    {
        return true;
    }

    if (startDistance > 0.0f && !(polygon->flags & FMOD_POLYGON_FLAG_DOUBLESIDED))
    {
        return true;
    }

    t = startDistance / (startDistance - endDistance);
    intersection.x = lineTestData->start.x + t * (lineTestData->end.x - lineTestData->start.x);
    intersection.y = lineTestData->start.y + t * (lineTestData->end.y - lineTestData->start.y);
    intersection.z = lineTestData->start.z + t * (lineTestData->end.z - lineTestData->start.z);

    vertices = &polygon->vertices;
    numVertices = polygon->flags & FMOD_POLYGON_NUM_VERTICES_MASK;
    for (i = 0; i < numVertices; i++)
    {
        int next = i + 1;
        FMOD_VECTOR edge;
        FMOD_VECTOR edgeNormal;

        if (next >= numVertices)
        {
            next = 0;
        }

        edge.x = vertices[next].x - vertices[i].x;
        edge.y = vertices[next].y - vertices[i].y;
        edge.z = vertices[next].z - vertices[i].z;
        cross(edge, polygon->normal, edgeNormal);

        if (edgeNormal.x * (intersection.x - vertices[i].x) + edgeNormal.y * (intersection.y - vertices[i].y) + edgeNormal.z * (intersection.z - vertices[i].z) > 0.0f)
        {
            break;
        }
    }

    if (i == numVertices)
    {
        lineTestData->directTransmission *= 1.0f - polygon->directOcclusion;
        lineTestData->reverbTransmission *= 1.0f - polygon->reverbOcclusion;
    }

    return true;
}

FMOD_RESULT GeometryI::flush()
{
    OctreeNode * node = mPolygonUpdateList;

    mPolygonUpdateList = 0;
    while (node)
    {
        OctreeNode * next = node->nextItem;
        FMOD_POLYGON * polygon = (FMOD_POLYGON *)node;
        FMOD_VECTOR * vertices = &polygon->vertices;
        FMOD_VECTOR normal;
        float length;
        float size;
        int i;

        node->nextItem = 0;

        normal.x = 0.0f;
        normal.y = 0.0f;
        normal.z = 0.0f;
        for (i = 0; i < (polygon->flags & FMOD_POLYGON_NUM_VERTICES_MASK) - 2; i++)
        {
            FMOD_VECTOR a;
            FMOD_VECTOR b;
            FMOD_VECTOR c;

            a.x = vertices[i + 1].x - vertices[0].x;
            a.y = vertices[i + 1].y - vertices[0].y;
            a.z = vertices[i + 1].z - vertices[0].z;
            b.x = vertices[i + 2].x - vertices[0].x;
            b.y = vertices[i + 2].y - vertices[0].y;
            b.z = vertices[i + 2].z - vertices[0].z;
            cross(a, b, c);
            normal.x += c.x;
            normal.y += c.y;
            normal.z += c.z;
        }

        length = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (length > 0.0f)
        {
            polygon->flags &= ~FMOD_POLYGON_FLAG_DEGENERATE;
            normal.x /= length;
            normal.y /= length;
            normal.z /= length;
        }
        else
        {
            polygon->flags |= FMOD_POLYGON_FLAG_DEGENERATE;
        }

        polygon->normal = normal;
        polygon->distance = vertices[0].x * polygon->normal.x + vertices[0].y * polygon->normal.y + vertices[0].z * polygon->normal.z;

        mOctree.addInternalNode(&polygon->nodeInternal);

        polygon->node.aabb.xMin = polygon->node.aabb.xMax = vertices[0].x;
        polygon->node.aabb.yMin = polygon->node.aabb.yMax = vertices[0].y;
        polygon->node.aabb.zMin = polygon->node.aabb.zMax = vertices[0].z;
        for (i = 1; i < (polygon->flags & FMOD_POLYGON_NUM_VERTICES_MASK); i++)
        {
            polygon->node.aabb.xMax = MAX(polygon->node.aabb.xMax, vertices[i].x);
            polygon->node.aabb.xMin = MIN(polygon->node.aabb.xMin, vertices[i].x);
            polygon->node.aabb.yMax = MAX(polygon->node.aabb.yMax, vertices[i].y);
            polygon->node.aabb.yMin = MIN(polygon->node.aabb.yMin, vertices[i].y);
            polygon->node.aabb.zMax = MAX(polygon->node.aabb.zMax, vertices[i].z);
            polygon->node.aabb.zMin = MIN(polygon->node.aabb.zMin, vertices[i].z);
        }

        size = MAX(polygon->node.aabb.xMax - polygon->node.aabb.xMin, polygon->node.aabb.yMax - polygon->node.aabb.yMin);
        size = MAX(size, polygon->node.aabb.zMax - polygon->node.aabb.zMin);
        size *= 0.01f;
        polygon->node.aabb.xMin -= size;
        polygon->node.aabb.xMax += size;
        polygon->node.aabb.yMin -= size;
        polygon->node.aabb.yMax += size;
        polygon->node.aabb.zMin -= size;
        polygon->node.aabb.zMax += size;

        if (!(polygon->flags & FMOD_POLYGON_FLAG_DEGENERATE))
        {
            mOctree.insertItem(&polygon->node);
        }

        node = next;
    }

    mOctree.getAABB(&mAABB);
    updateSpacialData();
    return FMOD_OK;
}

bool GeometryI::lineTest(LineTestData * lineTestData)
{
    FMOD_VECTOR start = lineTestData->start;
    FMOD_VECTOR end = lineTestData->end;
    FMOD_VECTOR localStart;
    FMOD_VECTOR localEnd;
    bool result;

    localStart = lineTestData->start;
    localStart.x -= mPosition.x;
    localStart.y -= mPosition.y;
    localStart.z -= mPosition.z;
    localEnd = lineTestData->end;
    localEnd.x -= mPosition.x;
    localEnd.y -= mPosition.y;
    localEnd.z -= mPosition.z;
    matrixMult(mInvMatrix, &localStart, &lineTestData->start);
    matrixMult(mInvMatrix, &localEnd, &lineTestData->end);

    result = mOctree.testLine(octreeLineTestCallback, lineTestData, lineTestData->start, lineTestData->end);

    lineTestData->start = start;
    lineTestData->end = end;
    lineTestData->geometryI = 0;
    return result;
}

FMOD_RESULT Geometry::release()
{
}

FMOD_RESULT Geometry::addPolygon(float directocclusion, float reverbocclusion, bool doublesided, int numvertices, const FMOD_VECTOR * vertices, int * polygonindex)
{
}

FMOD_RESULT Geometry::getNumPolygons(int * numpolygons)
{
}

FMOD_RESULT Geometry::getMaxPolygons(int * maxpolygons, int * maxvertices)
{
}

FMOD_RESULT Geometry::getPolygonNumVertices(int index, int * numvertices)
{
}

FMOD_RESULT Geometry::setPolygonVertex(int index, int vertexindex, const FMOD_VECTOR * vertex)
{
}

FMOD_RESULT Geometry::getPolygonVertex(int index, int vertexindex, FMOD_VECTOR * vertex)
{
}

FMOD_RESULT Geometry::setPolygonAttributes(int index, float directocclusion, float reverbocclusion, bool doublesided)
{
}

FMOD_RESULT Geometry::getPolygonAttributes(int index, float * directocclusion, float * reverbocclusion, bool * doublesided)
{
}

FMOD_RESULT Geometry::setActive(bool active)
{
}

FMOD_RESULT Geometry::getActive(bool * active)
{
}

FMOD_RESULT Geometry::setRotation(const FMOD_VECTOR * forward, const FMOD_VECTOR * up)
{
}

FMOD_RESULT Geometry::getRotation(FMOD_VECTOR * forward, FMOD_VECTOR * up)
{
}

FMOD_RESULT Geometry::setPosition(const FMOD_VECTOR * position)
{
}

FMOD_RESULT Geometry::getPosition(FMOD_VECTOR * position)
{
}

FMOD_RESULT Geometry::setScale(const FMOD_VECTOR * scale)
{
}

FMOD_RESULT Geometry::getScale(FMOD_VECTOR * scale)
{
}

FMOD_RESULT Geometry::save(void * data, int * datasize)
{
}

FMOD_RESULT Geometry::setUserData(void * _userdata)
{
}

FMOD_RESULT Geometry::getUserData(void * * _userdata)
{
}

} // namespace FMOD
