// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_MEMORY_H
#define _FMOD_MEMORY_H

#include "fmod.h"
#include "fmod_os_misc.h"

namespace FMOD {
    struct LocalMemoryCounter;
    class MemPool;
    class MemSingleton;
}

namespace FMOD {

struct MemBlockHeader
{
    int mSize; // offset 0x0
    int mNumBlocks; // offset 0x4
    int mBlockOffset; // offset 0x8
    int mAlloced; // offset 0xC
};

class MemPool
{
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
    int mFirstFreeBlock; // offset 0x2C
    unsigned int mCounterThread[32]; // offset 0x30
    unsigned int * mCounterPtr[32]; // offset 0xB0
    bool mCounterPause[32]; // offset 0x130
    FMOD_MEMORY_ALLOCCALLBACK mAlloc; // offset 0x150
    FMOD_MEMORY_REALLOCCALLBACK mRealloc; // offset 0x154
    FMOD_MEMORY_FREECALLBACK mFree; // offset 0x158
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x15C
public:
    int findFreeBlocks(int, int, int);
    void updateCounters(int);
    void set(int blockoffset, int value, int numblocks);
    int mBlockSize; // offset 0x160
    MemPool();
    FMOD_RESULT init(void * poolmem, int poolsize, int blocksize);
    FMOD_RESULT initCustom(void * poolmem, int poolsize, int blocksize);
    void close();
    void * alloc(int, const char *, int, FMOD_MEMORY_TYPE);
    void * calloc(int, const char *, int, FMOD_MEMORY_TYPE);
    void * realloc(void *, int, const char *, int, FMOD_MEMORY_TYPE);
    void free(void *, const char *, int, FMOD_MEMORY_TYPE);
    int getSize(void * ptr, const char * file, int line);
    void addCounter(unsigned int *, const char *, int);
    void removeCounter(unsigned int *, const char *, int);
    void pauseCounter(unsigned int *, bool, const char *, int);
    FMOD_RESULT setCallbacks(FMOD_MEMORY_ALLOCCALLBACK, FMOD_MEMORY_REALLOCCALLBACK, FMOD_MEMORY_FREECALLBACK);
    unsigned int getAvailable();
    unsigned int getCurrentAllocated();
    unsigned int getMaxAllocated();
    unsigned char * getData();
    int getSizeBytes();
};

class MemSingleton
{
    void * mBuffer; // offset 0x0
    int mRefCount; // offset 0x4
public:
    MemSingleton();
    ~MemSingleton();
    void * alloc(int len, const char * file, int line);
    void free(const char * file, int line);
    void * getData();
};

struct LocalMemoryCounter
{
    unsigned int mMemoryUsed; // offset 0x0
    LocalMemoryCounter();
    ~LocalMemoryCounter();
    void remove();
};

} // namespace FMOD

#ifdef __cplusplus
extern "C" {
#endif

void FMOD_Memory_freeC(void * ptr, const char * file, const int line);
void * FMOD_Memory_reallocC(void * ptr, int len, const char * file, const int line);
void * FMOD_Memory_callocC(int len, const char * file, const int line);
void * FMOD_Memory_allocC(int len, const char * file, const int line);

#ifdef __cplusplus
}
#endif

namespace FMOD {

void * Memory_DefaultMalloc(unsigned int size, FMOD_MEMORY_TYPE type);
void * Memory_DefaultRealloc(void * data, unsigned int size, FMOD_MEMORY_TYPE type);
void Memory_DefaultFree(void * ptr, FMOD_MEMORY_TYPE type);

} // namespace FMOD

#endif
