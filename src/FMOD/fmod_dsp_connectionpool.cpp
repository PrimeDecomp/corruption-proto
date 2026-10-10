// G2MEAB fmod_dsp_connectionpool.cpp: complete reconstruction (group D).
// .text: 0x805F3F7C..0x805F4538 (5 native functions; the last is the weak DSPConnection deleting dtor).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; G2MEAB pool layout.

#include "fmod_dsp_connectionpool.h"
#include "fmod.h"
#include "fmod_dsp_connection.h"
#include "fmod_memory.h"

namespace FMOD {

FMOD_RESULT DSPConnectionPool::init(int numconnections, int numinputlevels)
{
    float * levelmemory;
    int count;

    if (numconnections < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mNumConnections = (numconnections + 128) / 128 * 128;

    mConnection = (DSPConnection *)FMOD_Memory_Calloc(mNumConnections * sizeof(DSPConnection));
    if (!mConnection)
    {
        return FMOD_ERR_MEMORY;
    }

    mNumInputLevels = numinputlevels;

    mLevelDataMemory = (float *)FMOD_Memory_Calloc(mNumConnections * (numinputlevels < 2 ? 2 : numinputlevels) * 8 * sizeof(float) * 3);
    if (!mLevelDataMemory)
    {
        return FMOD_ERR_MEMORY;
    }

    levelmemory = mLevelDataMemory;

    mFreeListHead.initNode();

    for (count = 0; count < mNumConnections; count++)
    {
        new (&mConnection[count]) DSPConnection;

        mConnection[count].init(levelmemory, numinputlevels);
        mConnection[count].addAfter(&mFreeListHead);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPConnectionPool::close()
{
    if (mConnection)
    {
        FMOD_Memory_Free(mConnection);
        mConnection = 0;
    }

    if (mLevelDataMemory)
    {
        FMOD_Memory_Free(mLevelDataMemory);
        mLevelDataMemory = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPConnectionPool::alloc(DSPConnection * * connection)
{
    DSPConnection * newconnection;

    if (!connection)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFreeListHead.getNext() == &mFreeListHead)
    {
        DSPConnection * connectionmem;
        float * levelmemory;
        int count;

        connectionmem = (DSPConnection *)FMOD_Memory_Alloc(mNumConnections * sizeof(DSPConnection));
        if (!connectionmem)
        {
            return FMOD_ERR_MEMORY;
        }

        levelmemory = (float *)FMOD_Memory_Alloc(mNumConnections * mNumInputLevels * 8 * sizeof(float) * 3);
        if (!levelmemory)
        {
            return FMOD_ERR_MEMORY;
        }

        for (count = 0; count < mNumConnections; count++)
        {
            new (connectionmem) DSPConnection;

            connectionmem->init(levelmemory, mNumInputLevels);
            connectionmem->addAfter(&mFreeListHead);
            connectionmem++;
        }
    }

    newconnection = (DSPConnection *)mFreeListHead.getNext();

    newconnection->mInputNode.removeNode();
    newconnection->mInputNode.setData(newconnection);
    newconnection->mOutputNode.removeNode();
    newconnection->mOutputNode.setData(newconnection);

    newconnection->removeNode();
    newconnection->addAfter(&mUsedListHead);

    *connection = newconnection;

    return FMOD_OK;
}

FMOD_RESULT DSPConnectionPool::free(DSPConnection * connection)
{
    if (!connection)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    connection->mInputNode.removeNode();
    connection->mOutputNode.removeNode();

    connection->removeNode();
    connection->addAfter(&mFreeListHead);

    return FMOD_OK;
}

} // namespace FMOD
