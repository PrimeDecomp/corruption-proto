// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_GEOMETRY_MGR_H
#define _FMOD_GEOMETRY_MGR_H

#include "fmod.h"

struct FMOD_VECTOR;
namespace FMOD {
    class GeometryI;
    class GeometryMgr;
    struct Octree;
    struct OctreeNode;
    struct SystemI;
}

namespace FMOD {

class GeometryMgr
{
public:
    SystemI * mSystem; // offset 0x0
    bool mMoved; // offset 0x4
    GeometryMgr();
    ~GeometryMgr();
    FMOD_RESULT aquireMainOctree();
    void releaseMainOctree();
    Octree * mainOctree();
    FMOD_RESULT setWorldSize(float worldSize);
    float getWorldSize();
    FMOD_RESULT lineTestAll(FMOD_VECTOR * start, FMOD_VECTOR * end, float * directOcclusion, float * reverbOcclusion);
    FMOD_RESULT flushAll();
    static bool mainOctreeLineTestCallback(OctreeNode * item, void * data);
private:
    Octree * mMainOctree; // offset 0x8
    int mRefCount; // offset 0xC
    GeometryI * mFirstUpdateItem; // offset 0x10
    float mWorldSize; // offset 0x14
};

} // namespace FMOD

#endif
