// G2MEAB prototype translation unit; complete reconstruction.
// .text: 0x8060B560..0x8060C2B4 (16 native functions).
// Allocation/release paths name the file; MemPool::findFreeBlocks is inlined into alloc/realloc.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; MemPool uses the
// G2MEAB layout documented in fmod_memory.h.

#include "fmod_memory.h"
#include "fmod.h"
#include "fmod_string.h"

#include <string.h>

void * FMOD_Memory_allocC(int len, const char * file, const int line)
{
    return FMOD::gSystemPool->alloc(len, file, line);
}

void * FMOD_Memory_callocC(int len, const char * file, const int line)
{
    return FMOD::gSystemPool->calloc(len, file, line);
}

void * FMOD_Memory_reallocC(void * ptr, int len, const char * file, const int line)
{
    return FMOD::gSystemPool->realloc(ptr, len, file, line);
}

void FMOD_Memory_freeC(void * ptr, const char * file, const int line)
{
    FMOD::gSystemPool->free(ptr, file, line);
}

namespace FMOD {

void * Memory_DefaultMalloc(unsigned int size)
{
    return FMOD_OS_Memory_Alloc(size);
}

void * Memory_DefaultRealloc(void * data, unsigned int size)
{
    return FMOD_OS_Memory_Realloc(data, size);
}

void Memory_DefaultFree(void * ptr)
{
    FMOD_OS_Memory_Free(ptr);
}

MemPool::MemPool()
{
    mAlloc = Memory_DefaultMalloc;
    mRealloc = Memory_DefaultRealloc;
    mFree = Memory_DefaultFree;
    mCrit = 0;
}

FMOD_RESULT MemPool::init(void * poolmem, int poolsize, int blocksize)
{
    FMOD_RESULT result;
    void * oldpoolmem = poolmem;
    int bitmapsize;

    if (!poolmem || !poolsize)
    {
        return FMOD_ERR_MEMORY;
    }

    close();

    poolmem = (void *)(((unsigned int)poolmem + 255) & ~255);
    poolsize -= (unsigned int)poolmem - (unsigned int)oldpoolmem;
    poolsize &= ~(blocksize - 1);

    mBlockSize = blocksize;

    bitmapsize = ((poolsize + 7) / 8 + mBlockSize - 1) / mBlockSize;
    bitmapsize = (bitmapsize + blocksize - 1) & ~(blocksize - 1);

    mSizeBlocks = (poolsize - bitmapsize + mBlockSize - 1) / mBlockSize;
    mSizeBytes = mSizeBlocks * mBlockSize;
    mBitmap = (unsigned char *)poolmem;
    mData = mBitmap + bitmapsize;

    set(0, 0, mSizeBlocks);

    memset(mData, 0, mSizeBytes);

    mNumBlocks = 0;
    mMaxBlocks = 0;
    mCurrentAllocated = 0;
    mMaxAllocated = 0;
    mActualMaxBytes = 0;
    mWastage = 0;

    result = FMOD_OS_CriticalSection_Create(&mCrit, true);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT MemPool::initCustom(void * poolmem, int poolsize, int blocksize)
{
    FMOD_RESULT result;

    if (!poolsize)
    {
        return FMOD_ERR_MEMORY;
    }

    close();

    mBlockSize = blocksize;
    mSizeBlocks = (poolsize + mBlockSize - 1) / mBlockSize;
    mSizeBlocks &= ~3;
    mSizeBytes = mSizeBlocks * mBlockSize;

    mBitmap = (unsigned char *)FMOD_Memory_Alloc((mSizeBlocks + 7) / 8);
    if (!mBitmap)
    {
        return FMOD_ERR_MEMORY;
    }
    mData = (unsigned char *)poolmem;

    set(0, 0, mSizeBlocks);

    mNumBlocks = 0;
    mMaxBlocks = 0;
    mCurrentAllocated = 0;
    mMaxAllocated = 0;
    mActualMaxBytes = 0;
    mWastage = 0;
    mCustomPool = true;
    mAlloc = 0;
    mRealloc = 0;
    mFree = 0;

    result = FMOD_OS_CriticalSection_Create(&mCrit, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

void MemPool::close()
{
    if (mCustomPool && mBitmap)
    {
        FMOD_Memory_Free(mBitmap);
    }

    mBitmap = 0;
    mData = 0;
    mSizeBytes = 0;
    mSizeBlocks = 0;
    mNumBlocks = 0;
    mMaxBlocks = 0;
    mCurrentAllocated = 0;
    mMaxAllocated = 0;
    mAlloc = Memory_DefaultMalloc;
    mRealloc = Memory_DefaultRealloc;
    mFree = Memory_DefaultFree;
    mCustomPool = false;

    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit);
        mCrit = 0;
    }
}

void MemPool::set(int blockoffset, int value, int numblocks)
{
    int byteoffset = blockoffset / 8;
    int bitoffset = blockoffset & 7;
    int count;

    count = 0;
    if (blockoffset & 31)
    {
        count = 32 - (blockoffset & 31);
        if (count > numblocks)
        {
            count = numblocks;
        }
    }

    for (; count; count--)
    {
        if (value)
        {
            mBitmap[byteoffset] |= (1 << bitoffset);
        }
        else
        {
            mBitmap[byteoffset] &= ~(1 << bitoffset);
        }

        bitoffset++;
        if (bitoffset >= 8)
        {
            bitoffset = 0;
            byteoffset++;
        }
        numblocks--;
    }

    count = numblocks / 8;
    if (count)
    {
        memset(mBitmap + byteoffset, value ? 0xFF : 0, count);
        byteoffset += count;
        numblocks -= count * 8;
    }

    for (count = numblocks & 31; count; count--)
    {
        if (value)
        {
            mBitmap[byteoffset] |= (1 << bitoffset);
        }
        else
        {
            mBitmap[byteoffset] &= ~(1 << bitoffset);
        }

        bitoffset++;
        if (bitoffset >= 8)
        {
            bitoffset = 0;
            byteoffset++;
        }
    }
}

inline int MemPool::findFreeBlocks(int offset, int to, int numblocks)
{
    int found = 0;
    int bitoffset = 0;

    while (found < numblocks && offset * 8 + bitoffset < to && offset * 8 + bitoffset < mSizeBlocks)
    {
        if ((mBitmap[offset] & (1 << bitoffset)) || (!bitoffset && !(offset & 3) && *(unsigned int *)&mBitmap[offset] == 0xFFFFFFFF))
        {
            found = 0;
        }
        else
        {
            found++;
        }

        if (!bitoffset && !(offset & 3) && *(unsigned int *)&mBitmap[offset] == 0xFFFFFFFF)
        {
            offset += 4;
        }
        else
        {
            bitoffset++;
            if (bitoffset >= 8)
            {
                bitoffset = 0;
                offset++;
            }
        }
    }

    offset = offset * 8 + bitoffset;
    if (found == numblocks)
    {
        offset -= numblocks;
    }
    else
    {
        offset = -1;
    }

    return offset;
}

void * MemPool::alloc(int len, const char * file, const int line)
{
    MemBlockHeader * block = 0;
    int numblocks = 0;
    int reallen = len;

    FMOD_OS_CriticalSection_Enter(mCrit);

    if (!mCustomPool)
    {
        reallen += sizeof(MemBlockHeader);
    }

    if (mAlloc)
    {
        block = (MemBlockHeader *)mAlloc(reallen);
    }
    else
    {
        int offset;

        numblocks = (mBlockSize + reallen - 1) / mBlockSize;

        offset = findFreeBlocks(0, mSizeBlocks, numblocks);
        if (offset >= 0)
        {
            set(offset, 1, numblocks);

            if (mCustomPool)
            {
                block = (MemBlockHeader *)FMOD_Memory_Alloc(sizeof(MemBlockHeader));
            }
            else
            {
                block = (MemBlockHeader *)(mData + offset * mBlockSize);
            }

            block->mBlockOffset = offset;
        }
    }

    if (!block)
    {
        FMOD_OS_CriticalSection_Leave(mCrit);
        return 0;
    }

    block->mSize = len;
    block->mNumBlocks = numblocks;

    mCurrentAllocated += block->mSize;
    if (mCurrentAllocated > mMaxAllocated)
    {
        mMaxAllocated = mCurrentAllocated;
    }

    mNumBlocks += block->mNumBlocks;
    if (mNumBlocks > mMaxBlocks)
    {
        mMaxBlocks = mNumBlocks;
        mActualMaxBytes = mMaxBlocks * mBlockSize;
        mWastage = mActualMaxBytes - mMaxAllocated;
    }

    if (!mCustomPool)
    {
        block++;
    }

    FMOD_OS_CriticalSection_Leave(mCrit);

    return block;
}

void MemPool::free(void * ptr, const char * file, const int line)
{
    MemBlockHeader * block = (MemBlockHeader *)ptr;

    FMOD_OS_CriticalSection_Enter(mCrit);

    if (!mCustomPool)
    {
        block--;
    }

    mCurrentAllocated -= block->mSize;
    mNumBlocks -= block->mNumBlocks;

    if (mFree)
    {
        mFree(block);
    }
    else
    {
        set(block->mBlockOffset, 0, block->mNumBlocks);
    }

    FMOD_OS_CriticalSection_Leave(mCrit);

    if (mCustomPool)
    {
        FMOD_Memory_Free(block);
    }
}

void * MemPool::calloc(int len, const char * file, const int line)
{
    void * ptr = alloc(len, file, line);

    if (!mCustomPool && ptr)
    {
        memset(ptr, 0, len);
    }

    return ptr;
}

void * MemPool::realloc(void * ptr, int len, const char * file, const int line)
{
    MemBlockHeader * block = (MemBlockHeader *)ptr;
    int reallen = len;
    int numblocks = 0;

    if (!ptr)
    {
        return alloc(len, file, line);
    }

    FMOD_OS_CriticalSection_Enter(mCrit);

    if (!mCustomPool)
    {
        reallen += sizeof(MemBlockHeader);
        block--;
    }

    mCurrentAllocated -= block->mSize;
    mNumBlocks -= block->mNumBlocks;

    if (mRealloc)
    {
        block = (MemBlockHeader *)mRealloc(block, reallen);
    }
    else
    {
        int offset;

        numblocks = (mBlockSize + reallen - 1) / mBlockSize;

        set(block->mBlockOffset, 0, block->mNumBlocks);

        offset = findFreeBlocks(block->mBlockOffset, block->mBlockOffset + numblocks, numblocks);
        if (offset >= 0)
        {
            set(offset, 1, numblocks);

            block = (MemBlockHeader *)(mData + offset * mBlockSize);
            block->mBlockOffset = offset;
        }
        else
        {
            offset = findFreeBlocks(0, mSizeBlocks, numblocks);
            if (offset >= 0)
            {
                MemBlockHeader * newblock;

                set(offset, 1, numblocks);

                if (mCustomPool)
                {
                    newblock = block;
                }
                else
                {
                    newblock = (MemBlockHeader *)(mData + offset * mBlockSize);
                }

                newblock->mBlockOffset = offset;

                if (!mCustomPool)
                {
                    FMOD_memmove(newblock + 1, block + 1, block->mSize);
                }

                block = newblock;
            }
            else
            {
                block = 0;
            }
        }
    }

    if (!block)
    {
        FMOD_OS_CriticalSection_Leave(mCrit);
        return 0;
    }

    block->mSize = len;
    block->mNumBlocks = numblocks;

    mCurrentAllocated += block->mSize;
    if (mCurrentAllocated > mMaxAllocated)
    {
        mMaxAllocated = mCurrentAllocated;
    }

    mNumBlocks += block->mNumBlocks;
    if (mNumBlocks > mMaxBlocks)
    {
        mMaxBlocks = mNumBlocks;
        mActualMaxBytes = mMaxBlocks * mBlockSize;
        mWastage = mActualMaxBytes - mMaxAllocated;
    }

    if (!mCustomPool)
    {
        block++;
    }

    FMOD_OS_CriticalSection_Leave(mCrit);

    return block;
}

} // namespace FMOD
