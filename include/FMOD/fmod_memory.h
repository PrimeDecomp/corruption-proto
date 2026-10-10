// G2MEAB MemPool layout (0x40, the static pool object 0x80754290): the 4.06 members without
// mFirstFreeBlock and the per-thread counters. Evidence: MemPool() 0x8060B6A8 (+0x2C..+0x38), init
// 0x8060B6D8, initCustom 0x8060B7FC, close 0x8060B930, alloc 0x8060BB58; FMOD_Memory_GetStats
// 0x805B5F30 reads +0x1C/+0x20. Allocation entry points take no memory type (callers set r3..r6 only).

#ifndef _FMOD_MEMORY_H
#define _FMOD_MEMORY_H

#include "fmod.h"
#include "fmod_os_misc.h"

#include <stddef.h>

namespace FMOD {
    class MemPool;
}

namespace FMOD {

// Block header in front of every non-custom allocation; alloc/free/realloc step over 0x20 bytes.
struct MemBlockHeader
{
    int mSize; // offset 0x0
    int mNumBlocks; // offset 0x4
    int mBlockOffset; // offset 0x8
    int mUnkC[5]; // offset 0xC, not accessed; the header is 0x20 bytes
};

class MemPool
{
public:
    unsigned char * mBitmap; // offset 0x0
    unsigned char * mData; // offset 0x4
    bool mCustomPool; // offset 0x8
    int mSizeBytes; // offset 0xC
    int mSizeBlocks; // offset 0x10
    int mNumBlocks; // offset 0x14
    int mMaxBlocks; // offset 0x18
    int mCurrentAllocated; // offset 0x1C
    int mMaxAllocated; // offset 0x20
    int mActualMaxBytes; // offset 0x24
    int mWastage; // offset 0x28
    FMOD_MEMORY_ALLOCCALLBACK mAlloc; // offset 0x2C
    FMOD_MEMORY_REALLOCCALLBACK mRealloc; // offset 0x30
    FMOD_MEMORY_FREECALLBACK mFree; // offset 0x34
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x38
    int mBlockSize; // offset 0x3C

    inline int findFreeBlocks(int offset, int to, int numblocks);
    void set(int blockoffset, int value, int numblocks);
    MemPool();
    FMOD_RESULT init(void * poolmem, int poolsize, int blocksize);
    FMOD_RESULT initCustom(void * poolmem, int poolsize, int blocksize);
    void close();
    void * alloc(int len, const char * file, const int line);
    void * calloc(int len, const char * file, const int line);
    void * realloc(void * ptr, int len, const char * file, const int line);
    void free(void * ptr, const char * file, const int line);

    // Inline (FMOD_Memory_Initialize 0x805B5E74 stores all three through one pool load).
    FMOD_RESULT setCallbacks(FMOD_MEMORY_ALLOCCALLBACK useralloc, FMOD_MEMORY_REALLOCCALLBACK userrealloc, FMOD_MEMORY_FREECALLBACK userfree)
    {
        mAlloc = useralloc;
        mRealloc = userrealloc;
        mFree = userfree;
        return FMOD_OK;
    }
    unsigned int getCurrentAllocated() { return mCurrentAllocated; }
    unsigned int getMaxAllocated() { return mMaxAllocated; }
};

extern MemPool * gSystemPool;

// Reference-counted shared buffer (SystemI+0xE20). G2MEAB keeps no out-of-line alloc/free: the
// release in Sample::release 0x80615224 and in the SystemI destructor 0x8060B1BC (file "", line 0)
// are inline expansions.
struct MemSingleton
{
    void * mBuffer; // offset 0x0
    int mRefCount; // offset 0x4

    MemSingleton()
    {
        mBuffer = 0;
        mRefCount = 0;
    }
    ~MemSingleton()
    {
        free("", 0);
    }
    void * getData() { return mBuffer; }
    // Expanded in SoundI::loadSubSound 0x80616938 (file "", line 0).
    void * alloc(int size, const char * file, const int line)
    {
        if (!mRefCount)
        {
            mBuffer = gSystemPool->alloc(size, file, line);
        }
        mRefCount++;
        return mBuffer;
    }
    void free(const char * file, const int line)
    {
        if (mRefCount)
        {
            mRefCount--;
        }
        if (!mRefCount && mBuffer)
        {
            gSystemPool->free(mBuffer, file, line);
            mBuffer = 0;
        }
    }
};

} // namespace FMOD

#define FMOD_Memory_Alloc(_len) FMOD::gSystemPool->alloc((_len), __FILE__, __LINE__)
#define FMOD_Memory_Calloc(_len) FMOD::gSystemPool->calloc((_len), __FILE__, __LINE__)
#define FMOD_Memory_ReAlloc(_ptr, _len) FMOD::gSystemPool->realloc((_ptr), (_len), __FILE__, __LINE__)
#define FMOD_Memory_Free(_ptr) FMOD::gSystemPool->free((_ptr), __FILE__, __LINE__)

#ifndef FMOD_Object_Alloc
// Placement allocation as in 4.06 (MemPool::alloc, then operator new(unsigned int, void *) and the
// constructor behind a null check), e.g. OutputEmulated::init 0x8060EF40.
inline void * operator new(size_t, void * ptr) { return ptr; }
#define FMOD_Object_Alloc(_type) new (FMOD_Memory_Alloc(sizeof(_type))) _type
#endif
#ifndef FMOD_Object_Calloc
// e.g. ChannelGroupI::addGroup 0x805BCB38.
#define FMOD_Object_Calloc(_type) new (FMOD_Memory_Calloc(sizeof(_type))) _type
#endif
#ifndef FMOD_Object_CallocSize
// At least sizeof(_type) bytes, compared unsigned (PluginFactory::createCodec 0x806115D8 cmplwi 0x1F4).
#define FMOD_Object_CallocSize(_type, _size) new (FMOD_Memory_Calloc((_size) < sizeof(_type) ? sizeof(_type) : (_size))) _type
#endif

#ifdef __cplusplus
extern "C" {
#endif

void * FMOD_Memory_allocC(int len, const char * file, const int line);
void * FMOD_Memory_callocC(int len, const char * file, const int line);
void * FMOD_Memory_reallocC(void * ptr, int len, const char * file, const int line);
void FMOD_Memory_freeC(void * ptr, const char * file, const int line);

#ifdef __cplusplus
}
#endif

namespace FMOD {

void * Memory_DefaultMalloc(unsigned int size);
void * Memory_DefaultRealloc(void * data, unsigned int size);
void Memory_DefaultFree(void * ptr);

} // namespace FMOD

#endif
