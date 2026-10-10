// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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

class GeometryI : public LinkedListNode
{
    GeometryMgr * mGeometryMgr; // offset 0xC
    int mMaxNumVertices; // offset 0x10
    int mNumVertices; // offset 0x14
    int mMaxNumPolygons; // offset 0x18
    int mNumPolygons; // offset 0x1C
    int * mPolygonOffsets; // offset 0x20
    int mPolygonDataPos; // offset 0x24
    unsigned char * mPolygonData; // offset 0x28
    void * mUserData; // offset 0x2C
    OctreeNode * mPolygonUpdateList; // offset 0x30
    FMOD_AABB mAABB; // offset 0x34
    bool mActive; // offset 0x4C
    FMOD_VECTOR mForward; // offset 0x50
    FMOD_VECTOR mUp; // offset 0x5C
    FMOD_VECTOR mPosition; // offset 0x68
    FMOD_VECTOR mScale; // offset 0x74
    float mMatrix[3][4]; // offset 0x80
    float mInvMatrix[3][4]; // offset 0xB0
    SpaicalData * mSpatialData; // offset 0xE0
    Octree mOctree; // offset 0xE4
    GeometryI * mNextUpdateItem; // offset 0xFC
    bool mToBeUpdated; // offset 0x100
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
