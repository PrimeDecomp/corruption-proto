// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. GeometryI and FMOD_POLYGON are the G2MEAB layout.

#ifndef _FMOD_GEOMETRYI_H
#define _FMOD_GEOMETRYI_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_octree.h"

struct FMOD_VECTOR;
namespace FMOD {
    struct Geometry;
    class GeometryI;
    class GeometryMgr;
    struct LineTestData;
    struct OctreeNode;
    struct SpaicalData;
}

namespace FMOD {

// FMOD_POLYGON::flags: the low 16 bits hold the vertex count; octreeLineTestCallback 0x8060A808 tests
// 0x10000 for double-sided polygons and flush 0x8060A9FC sets 0x20000 for a zero-length normal.
enum
{
    FMOD_POLYGON_NUM_VERTICES_MASK = 0xFFFF, // Guessed name
    FMOD_POLYGON_FLAG_DOUBLESIDED = 0x10000, // Guessed name
    FMOD_POLYGON_FLAG_DEGENERATE = 0x20000 // Guessed name
};

struct FMOD_POLYGON
{
    OctreeNode node; // offset 0x0
    OctreeNode nodeInternal; // offset 0x3C
    float distance; // offset 0x78
    FMOD_VECTOR normal; // offset 0x7C
    float directOcclusion; // offset 0x88
    float reverbOcclusion; // offset 0x8C
    int flags; // offset 0x90
    FMOD_VECTOR vertices; // offset 0x94
};

// Octree item of a whole geometry in the main octree: GeometryMgr::mainOctreeLineTestCallback 0x8060A42C
// reads the owner at +0x78; updateSpacialData writes the AABB of the first node.
struct SpaicalData
{
    OctreeNode octreeNode; // offset 0x0, Guessed name
    OctreeNode octreeInternalNode; // offset 0x3C, Guessed name
    GeometryI * geometryI; // offset 0x78, Guessed name
};

// Built on the stack by GeometryMgr::lineTestAll 0x8060A458 (transmissions start at 1.0, geometry 0).
struct LineTestData
{
    FMOD_VECTOR start; // offset 0x0, Guessed name
    FMOD_VECTOR end; // offset 0xC, Guessed name
    float directTransmission; // offset 0x18, Guessed name
    float reverbTransmission; // offset 0x1C, Guessed name
    GeometryI * geometryI; // offset 0x20, Guessed name
};

// G2MEAB layout: the 4.06 members shifted by the 0x14 LinkedListNode base (vptr at +0x10). Evidence:
// updateSpacialData 0x8060A5F8 (mAABB +0x3C, mActive +0x54, mPosition +0x70, mMatrix +0x88,
// mSpatialData +0xE8, mGeometryMgr +0x14), flush 0x8060A9FC (mPolygonUpdateList +0x38, mOctree +0xEC),
// lineTest 0x8060AF3C (mInvMatrix +0xB8) and GeometryMgr::flushAll 0x8060A51C (mNextUpdateItem +0x104,
// mToBeUpdated +0x108).
class GeometryI : public LinkedListNode
{
    friend class GeometryMgr;

    GeometryMgr * mGeometryMgr; // offset 0x14
    int mMaxNumVertices; // offset 0x18
    int mNumVertices; // offset 0x1C
    int mMaxNumPolygons; // offset 0x20
    int mNumPolygons; // offset 0x24
    int * mPolygonOffsets; // offset 0x28
    int mPolygonDataPos; // offset 0x2C
    unsigned char * mPolygonData; // offset 0x30
    void * mUserData; // offset 0x34
    OctreeNode * mPolygonUpdateList; // offset 0x38
    FMOD_AABB mAABB; // offset 0x3C
    bool mActive; // offset 0x54
    FMOD_VECTOR mForward; // offset 0x58
    FMOD_VECTOR mUp; // offset 0x64
    FMOD_VECTOR mPosition; // offset 0x70
    FMOD_VECTOR mScale; // offset 0x7C
    float mMatrix[3][4]; // offset 0x88
    float mInvMatrix[3][4]; // offset 0xB8
    SpaicalData * mSpatialData; // offset 0xE8
    Octree mOctree; // offset 0xEC
    GeometryI * mNextUpdateItem; // offset 0x104
    bool mToBeUpdated; // offset 0x108
public:
    void calculateMatrix();
    void updateSpacialData();
    void setToBeUpdated();
    GeometryI(GeometryMgr * geometryMgr);
    FMOD_RESULT release();
    FMOD_RESULT alloc(int maxNumPolygons, int maxNumVertices);
    FMOD_RESULT addPolygon(float directOcclusion, float reverbOcclusion, bool doubleSided, int numVertices, const FMOD_VECTOR * vertices, int * polygonIndex);
    FMOD_RESULT getNumPolygons(int * numPolygons);
    FMOD_RESULT getMaxPolygons(int * maxNumPolygons, int * maxVertices);
    FMOD_RESULT getPolygonNumVertices(int polygonIndex, int * numVertices);
    FMOD_RESULT setPolygonVertex(int polygonIndex, int vertexIndex, const FMOD_VECTOR * vertex);
    FMOD_RESULT getPolygonVertex(int polygonIndex, int vertexIndex, FMOD_VECTOR * vertex);
    FMOD_RESULT setPolygonAttributes(int polygonIndex, float directOcclusion, float reverbOcclusion, bool doubleSided);
    FMOD_RESULT getPolygonAttributes(int polygonIndex, float * directOcclusion, float * reverbOcclusion, bool * doubleSided);
    FMOD_RESULT flush();
    FMOD_RESULT setActive(bool active);
    FMOD_RESULT getActive(bool * active);
    FMOD_RESULT setRotation(const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
    FMOD_RESULT getRotation(FMOD_VECTOR * forward, FMOD_VECTOR * up);
    FMOD_RESULT setPosition(const FMOD_VECTOR * position);
    FMOD_RESULT getPosition(FMOD_VECTOR * position);
    FMOD_RESULT setScale(const FMOD_VECTOR * scale);
    FMOD_RESULT getScale(FMOD_VECTOR * scale);
    FMOD_RESULT save(void * data, int * dataSize);
    FMOD_RESULT load(const void * data, int dataSize);
    static FMOD_RESULT saveData(void * fileData, int dataSize, int * fileDataIndex, void * liveData, int liveDataSize);
    static FMOD_RESULT loadData(void * fileData, int dataSize, int * fileDataIndex, void * liveData, int liveDataSize);
    static FMOD_RESULT countData(void * fileData, int dataSize, int * fileDataIndex, void * liveData, int liveDataSize);
    FMOD_RESULT serialiser(void * data, int * dataSize, bool bWrite, bool bRead, FMOD_RESULT (* serialiseData)(void *, int, int *, void *, int));
    FMOD_RESULT setUserData(void * userdata);
    FMOD_RESULT getUserData(void * * userdata);
    bool lineTest(LineTestData * lineTestData);
    FMOD_RESULT setWorldSize(float worldSize);
    void removeFromTree();
    static FMOD_RESULT validate(Geometry * geometry, GeometryI * * geometryi);
    static bool octreeLineTestCallback(OctreeNode * item, void * data);
};

} // namespace FMOD

#endif
