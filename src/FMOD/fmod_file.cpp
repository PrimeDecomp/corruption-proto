// G2MEAB .text 0x806083B8..0x8060A3C8: all 55 native functions reconstructed (fmodcheck 99.7%).
// The split also holds the DiskFile, MemoryFile, NullFile and UserFile code after __sinit, so those
// were probably separate sources folded into this unit; here they follow File, which moves their
// weak destructors and the vtables ahead of __sinit. The remaining differences are relocation names,
// string-pool offsets (the subclass strings are separate pools natively) and __LINE__ arguments of the
// memory macros. Not retained natively: getByte(short *), getWord(short *), FMOD_File_Get/SetDiskBusy,
// gFileCrit, gFileBusy and gUsesUserCallbacks.

#include "fmod_file.h"
#include "fmod.h"
#include "fmod_file_disk.h"
#include "fmod_file_memory.h"
#include "fmod_file_null.h"
#include "fmod_file_user.h"
#include "fmod_linkedlist.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_string.h"
#include "fmod_systemi.h"
#include "fmod_time.h"

#include <stdio.h>
#include <string.h>

namespace FMOD {

unsigned int File::gBufferSize = 2048;
LinkedListNode File::gFileThreadHead;

void fileThreadFunc(void * data)
{
    FileThread * fileThread = (FileThread *)data;

    fileThread->threadFunc();
}

FMOD_RESULT FileThread::threadFunc()
{
    if (mThreadActive)
    {
        File * file;
        LinkedListNode * current;
        LinkedListNode * next;

        FMOD_OS_CriticalSection_Enter(mCrit);

        current = mHead.getNext();
        while (current != &mHead)
        {
            next = current->getNext();
            file = (File *)current;

            if (file->mNeedsFlip)
            {
                file->flip();
            }

            current = next;
        }

        FMOD_OS_CriticalSection_Leave(mCrit);
    }

    return FMOD_OK;
}

FileThread::FileThread()
{
    mCrit = 0;
    mThreadActive = false;
    mDeviceType = 0;
}

FMOD_RESULT FileThread::init(int devicetype, bool owned)
{
    FMOD_RESULT result;

    mDeviceType = devicetype;
    mOwned = owned;

    result = FMOD_OS_CriticalSection_Create(&mCrit, false);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = mThread.initThread("FMOD file thread", fileThreadFunc, this, Thread::PRIORITY_HIGH, 0, 16 * 1024, true, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mThreadActive = true;

    addAfter(&File::gFileThreadHead);

    return FMOD_OK;
}

FMOD_RESULT FileThread::release()
{
    removeNode();

    mThreadActive = false;
    mThread.closeThread();

    if (mCrit)
    {
        FMOD_OS_CriticalSection_Free(mCrit);
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT File::getFileThread()
{
    FMOD_RESULT result;
    LinkedListNode * current;
    int devicetype = 5; // Guessed: the only device type G2MEAB uses (0x80608678)
    FileThread * filethread = 0;
    bool found = false;

    for (current = gFileThreadHead.getNext(); current != &gFileThreadHead; current = current->getNext())
    {
        filethread = (FileThread *)current;

        if (filethread->mDeviceType == devicetype)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        filethread = FMOD_Object_Alloc(FileThread);
        if (!filethread)
        {
            return FMOD_ERR_MEMORY;
        }

        result = filethread->init(devicetype, false);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mFileThread = filethread;

    return FMOD_OK;
}

File::File()
{
    init(0, gBufferSize);
}

FMOD_RESULT File::init(unsigned int filesize, int blocksize)
{
    mBlockSize = blocksize;
    mBusy = false;
    mLength = 0;
    mNeedsFlip = false;
    mFileThread = 0;
    mRiderHandle = 0;
    mRiderUserData = 0;
    mUnk20 = true;
    mSeekable = true;
    mSystem = 0;

    mBigEndian = false;
    mStartOffset = 0;
    mBuffer = 0;
    mCurrentPosition = 0;
    mNextPosition = 0;
    mNextPositionDisplay = 0;
    mBlockOffset = 0;
    mBuffer = 0;
    mBufferPos = 0;
    mBufferSkip = 0;
    mAsyncError = FMOD_OK;
    mExit = false;
    mStarving = false;
    mEncryptionKeyLength = 0;
    mEncryptionKeyIndex = 0;

    mLengthOriginal = filesize;
    mLength = filesize;
    mFileSize = filesize;

    memset(mName, 0, 256);

    return FMOD_OK;
}

FMOD_RESULT File::shutDown()
{
    LinkedListNode * current;
    LinkedListNode * next;

    current = gFileThreadHead.getNext();
    while (current != &gFileThreadHead)
    {
        FileThread * filethread;

        next = current->getNext();
        filethread = (FileThread *)current;
        filethread->release();
        current = next;
    }

    return FMOD_OK;
}

FMOD_RESULT File::open(const char * name_or_data, unsigned int length, bool unicode, const char * encryptionkey)
{
    FMOD_RESULT result;

    mBigEndian = false;
    mStartOffset = 0;
    mBuffer = 0;
    mCurrentPosition = 0;
    mNextPosition = 0;
    mNextPositionDisplay = 0;
    mBlockOffset = 0;
    mBuffer = 0;
    mBufferPos = 0;
    mBufferSkip = 0;
    mAsyncError = FMOD_OK;
    mExit = false;
    mStarving = false;
    mEncryptionKeyLength = 0;
    mEncryptionKeyIndex = 0;

    mUnicode = unicode;
    mLength = length;
    mFileSize = length;

    if (encryptionkey)
    {
        mEncryptionKeyLength = FMOD_strlen(encryptionkey);
        if (mEncryptionKeyLength > 32)
        {
            mEncryptionKeyLength = 32;
        }

        memset(mEncryptionKey, 0, 32);
        FMOD_strncpy(mEncryptionKey, encryptionkey, mEncryptionKeyLength);
    }

    mBufferSize = mBlockSize;
    if (mBufferSize)
    {
        mBuffer = FMOD_Memory_Calloc(mBufferSize);
        if (!mBuffer)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    if (name_or_data && mUnk20)
    {
        FMOD_strncpy(mName, name_or_data, 256);
        mName[255] = 0;
    }

    result = reallyOpen(name_or_data, &mFileSize);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mSystem && mSystem->mOpenRiderCallback)
    {
        mSystem->mOpenRiderCallback(name_or_data, unicode, &mFileSize, &mRiderHandle, &mRiderUserData);
    }

    if (!mLength)
    {
        mLength = mFileSize;
    }
    mLengthOriginal = mLength;

    return result;
}

FMOD_RESULT File::close()
{
    FMOD_RESULT result;

    mExit = true;

    while (mBusy)
    {
        FMOD_Time_Sleep(10);
    }

    if (mFileThread)
    {
        FMOD_OS_CriticalSection_Enter(mFileThread->mCrit);
        removeNode();
        FMOD_OS_CriticalSection_Leave(mFileThread->mCrit);

        if (mFileThread->mOwned)
        {
            mFileThread->release();
        }

        mFileThread = 0;
    }

    result = reallyClose();

    if (mSystem && mSystem->mCloseRiderCallback)
    {
        mSystem->mCloseRiderCallback(mRiderHandle, mRiderUserData);
    }

    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
        mBuffer = 0;
    }

    return result;
}

FMOD_RESULT File::flip()
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int bytestoread = mBlockSize - mBufferSkip;
    char * destptr = (char *)mBuffer + mBlockOffset + mBufferSkip;

    while (bytestoread)
    {
        unsigned int read = 0;

        result = reallyRead(destptr, bytestoread, &read);
        if (result != FMOD_OK)
        {
            bytestoread = read;

            if ((int)mLength == -1 && result == FMOD_ERR_FILE_EOF)
            {
                mLength = mFileSize = mLengthOriginal = mNextPositionDisplay;
                result = FMOD_OK;
            }
        }

        if (mSystem && mSystem->mReadRiderCallback)
        {
            mSystem->mReadRiderCallback(mRiderHandle, destptr, read, 0, mRiderUserData);
        }

        bytestoread -= read;
        destptr += read;
        mNextPositionDisplay += read;

        mPercentBuffered = (int)(100.0f * (((float)mNextPositionDisplay - (float)mCurrentPosition) / (float)mBufferSize));
        if (mPercentBuffered < 0)
        {
            mPercentBuffered = 0;
        }

        if (mExit)
        {
            break;
        }
    }

    mBlockOffset += mBlockSize;
    if (mBlockOffset >= mBufferSize)
    {
        mBlockOffset = 0;
    }

    mBufferSkip = 0;
    mAsyncError = result;
    mNeedsFlip = false;
    mBusy = false;

    return result;
}

FMOD_RESULT File::seekAndReset()
{
    FMOD_RESULT result;
    unsigned int alignedpos;

    while (mBusy)
    {
        FMOD_Time_Sleep(10);
    }

    alignedpos = mCurrentPosition / mBufferSize * mBufferSize;

    mBufferPos = mCurrentPosition - alignedpos;
    mBlockOffset = 0;
    mNextPosition = alignedpos;
    mNextPositionDisplay = mNextPosition;
    mBufferSkip = 0;

    result = reallySeek(alignedpos);

    if (mSystem && mSystem->mSeekRiderCallback)
    {
        mSystem->mSeekRiderCallback(mRiderHandle, alignedpos, mRiderUserData);
    }

    return result;
}

FMOD_RESULT File::checkBufferedStatus()
{
    FMOD_RESULT result;
    int diff;

    if (mAsyncError != FMOD_OK && mAsyncError != FMOD_ERR_FILE_EOF)
    {
        return mAsyncError;
    }

    if (mNextPosition < mCurrentPosition)
    {
        diff = -1;
    }
    else
    {
        mPercentBuffered = (int)(100.0f * (((float)mNextPositionDisplay - (float)mCurrentPosition) / (float)mBufferSize));
        if (mPercentBuffered < 0 || mBufferSkip)
        {
            mPercentBuffered = 0;
        }

        diff = mNextPosition - mCurrentPosition;
        diff = (diff - 1 + mBlockSize) / mBlockSize;
    }

    if (mBufferSkip)
    {
        if (diff > 2)
        {
            mBufferSkip = 0;
        }
        else
        {
            diff = -1;
            mNextPosition = 0;
            mNextPositionDisplay = 0;
        }
    }

    if (diff != 2)
    {
        while (mBusy)
        {
            mStarving = true;
            FMOD_Time_Sleep(10);
        }
        mStarving = false;
    }

    if (diff == 1 && mBufferSize > mBlockSize)
    {
        mBusy = true;
        mNeedsFlip = true;

        mFileThread->mThread.wakeupThread(false);

        mNextPositionDisplay = mNextPosition;
        mNextPosition += mBlockSize;

        return FMOD_OK;
    }

    if ((mBufferSize > mBlockSize && diff == 2) || (mBufferSize == mBlockSize && diff == 1))
    {
        return FMOD_OK;
    }

    if (diff && mSeekable)
    {
        result = seekAndReset();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    mBusy = true;

    result = flip();
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    if (mBufferSize == mBlockSize && result == FMOD_ERR_FILE_EOF && (int)mLength == -1)
    {
        return result;
    }

    mNextPositionDisplay = mNextPosition;
    mNextPosition += mBlockSize;

    if (mBufferPos >= mBlockSize)
    {
        result = flip();
        if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
        {
            return result;
        }

        mNextPositionDisplay = mNextPosition;
        mNextPosition += mBlockSize;
    }

    return result;
}

FMOD_RESULT File::read(void * buffer, unsigned int size, unsigned int count, unsigned int * rd)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int oldsize = size;
    unsigned int bytesleft;
    unsigned int r = 0;
    bool eof = false;

    if (!buffer)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if ((int)(size * count) < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    size *= count;

    if (mCurrentPosition + size > mStartOffset + mLength)
    {
        if (mCurrentPosition <= mStartOffset + mLength)
        {
            size = mStartOffset + mLength - mCurrentPosition;
        }
        else
        {
            size = 0;
            return FMOD_ERR_FILE_BAD;
        }

        eof = true;
    }

    bytesleft = size;

    while (bytesleft)
    {
        size = bytesleft;

        if (mBlockSize == mBufferSize && bytesleft > mBlockSize && !mBufferPos && mSeekable && !(unsigned int)((char *)buffer + r) & 31)
        {
            if (mBlockSize)
            {
                if (mCurrentPosition != mNextPosition)
                {
                    result = seekAndReset();
                    if (result != FMOD_OK)
                    {
                        return result;
                    }
                }

                size /= mBlockSize;
                size *= mBlockSize;
            }

            mBusy = true;
            result = reallyRead((char *)buffer + r, size, &size);
            mBusy = false;

            if (mSystem && mSystem->mReadRiderCallback)
            {
                mSystem->mReadRiderCallback(mRiderHandle, (char *)buffer + r, size, 0, mRiderUserData);
            }

            if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
            {
                return result;
            }

            mNextPositionDisplay = mNextPosition;
            mNextPosition += size;

            if (!size)
            {
                result = FMOD_ERR_FILE_EOF;
            }

            if (result == FMOD_ERR_FILE_EOF)
            {
                break;
            }
        }
        else
        {
            result = checkBufferedStatus();
            if (result == FMOD_ERR_FILE_EOF && (mBlockSize != mBufferSize || (int)mLength != -1))
            {
                result = FMOD_OK;
            }

            if (result != FMOD_OK)
            {
                break;
            }

            if (size > mBlockSize - mBufferPos % mBlockSize)
            {
                size = mBlockSize - mBufferPos % mBlockSize;
            }

            memcpy((char *)buffer + r, (char *)mBuffer + mBufferPos, size);

            mBufferPos += size;
            if (mBufferPos >= mBufferSize)
            {
                mBufferPos = 0;
            }
        }

        mCurrentPosition += size;
        bytesleft -= size;
        r += size;
    }

    if (oldsize == 2)
    {
        unsigned short * wptr = (unsigned short *)buffer;
        unsigned int i;

        for (i = 0; i < r / oldsize; i++)
        {
            if (!mBigEndian)
            {
                wptr[i] = ((wptr[i] & 0xFF00) >> 8) | ((wptr[i] & 0xFF) << 8);
            }
        }
    }
    else if (oldsize == 4)
    {
        unsigned int * dptr = (unsigned int *)buffer;
        unsigned int i;

        for (i = 0; i < r / oldsize; i++)
        {
            if (!mBigEndian)
            {
                dptr[i] = ((dptr[i] & 0x000000FF) << 24) | ((dptr[i] & 0x0000FF00) << 8) | ((dptr[i] & 0x00FF0000) >> 8) | ((dptr[i] & 0xFF000000) >> 24);
            }
        }
    }

    r /= oldsize;

    if (mEncryptionKeyLength)
    {
        unsigned char * val = (unsigned char *)buffer;
        unsigned int count;

        for (count = 0; count < r; count++)
        {
            unsigned char tmp = mEncryptionKey[mEncryptionKeyIndex] ^ val[count];

            val[count] = 0;
            val[count] |= (tmp & 0x01) << 7;
            val[count] |= (tmp & 0x02) << 5;
            val[count] |= (tmp & 0x04) << 3;
            val[count] |= (tmp & 0x08) << 1;
            val[count] |= (tmp & 0x10) >> 1;
            val[count] |= (tmp & 0x20) >> 3;
            val[count] |= (tmp & 0x40) >> 5;
            val[count] |= (tmp & 0x80) >> 7;

            mEncryptionKeyIndex++;
            if (mEncryptionKeyIndex >= mEncryptionKeyLength)
            {
                mEncryptionKeyIndex = 0;
            }
        }
    }

    if (rd)
    {
        *rd = r;
    }

    if (result == FMOD_OK && eof)
    {
        result = FMOD_ERR_FILE_EOF;
    }

    return result;
}

FMOD_RESULT File::getByte(unsigned char * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

    result = read(&temp, 1, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getByte(unsigned short * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

    result = read(&temp, 1, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getByte(unsigned int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned char temp;

    result = read(&temp, 1, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getByte(signed char * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed char temp;

    result = read(&temp, 1, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getByte(int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    signed char temp;

    result = read(&temp, 1, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getWord(unsigned short * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned short temp;

    result = read(&temp, 2, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getWord(unsigned int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned short temp;

    result = read(&temp, 2, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getWord(int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    short temp;

    result = read(&temp, 2, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getDword(unsigned int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    unsigned int temp;

    result = read(&temp, 4, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::getDword(int * val)
{
    FMOD_RESULT result;
    unsigned int rd;
    int temp;

    result = read(&temp, 4, 1, &rd);
    if (val)
    {
        *val = temp;
    }

    return result;
}

FMOD_RESULT File::seek(int pos, int mode)
{
    FMOD_RESULT result = FMOD_OK;
    unsigned int newpos = 0;

    if (mode != SEEK_SET && mode != SEEK_CUR && mode != SEEK_END)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mode == SEEK_SET)
    {
        newpos = mStartOffset + pos;
    }
    else if (mode == SEEK_CUR)
    {
        newpos = mCurrentPosition + pos;
    }
    else if (mode == SEEK_END)
    {
        newpos = mLength + pos + mStartOffset;
    }

    if (newpos > mStartOffset + mLength)
    {
        if (pos >= 0)
        {
            newpos = mStartOffset + mLength;
        }
        else
        {
            newpos = 0;
        }
    }

    if (!mNextPosition && !mSeekable && newpos >= mBlockSize)
    {
        return FMOD_ERR_FILE_COULDNOTSEEK;
    }

    if (mBlockSize == mBufferSize && !mSeekable && mNextPosition >= mBlockSize)
    {
        if (mNextPosition && newpos < mNextPosition - mBlockSize)
        {
            return FMOD_ERR_FILE_COULDNOTSEEK;
        }
        if (newpos >= mNextPosition + mBlockSize)
        {
            return FMOD_ERR_FILE_COULDNOTSEEK;
        }
    }

    mCurrentPosition = newpos;

    if (mEncryptionKeyLength)
    {
        mEncryptionKeyIndex = mCurrentPosition % mEncryptionKeyLength;
    }

    if (mBufferSize)
    {
        mBufferPos = mCurrentPosition % mBufferSize;
    }
    else
    {
        result = reallySeek(newpos);

        if (mSystem && mSystem->mSeekRiderCallback)
        {
            mSystem->mSeekRiderCallback(mRiderHandle, newpos, mRiderUserData);
        }
    }

    return result;
}

FMOD_RESULT File::tell(unsigned int * pos)
{
    if (!pos)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *pos = mCurrentPosition;
    *pos -= mStartOffset;

    return FMOD_OK;
}

FMOD_RESULT File::setStartOffset(unsigned int offset)
{
    mStartOffset = offset;
    mLength = mLengthOriginal;

    if (mStartOffset + mLength > mFileSize)
    {
        mLength = mFileSize - mStartOffset;
    }

    return FMOD_OK;
}

FMOD_RESULT File::getStartOffset(unsigned int * offset)
{
    if (!offset)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *offset = mStartOffset;

    return FMOD_OK;
}

FMOD_RESULT File::enableDoubleBuffer(unsigned int sizebytes)
{
    FMOD_RESULT result;
    unsigned int oldblocksize;

    if (!mBlockSize)
    {
        return FMOD_OK;
    }

    while (mBusy)
    {
        FMOD_Time_Sleep(10);
    }

    if (sizebytes < FILE_SECTORSIZE)
    {
        sizebytes = FILE_SECTORSIZE;
    }

    oldblocksize = mBlockSize;
    if (sizebytes < oldblocksize)
    {
        sizebytes = oldblocksize;
    }

    mBlockSize = sizebytes;
    mBlockSize /= oldblocksize;
    mBlockSize *= oldblocksize;
    mBufferSkip = oldblocksize;
    mBlockOffset = 0;
    mNextPosition = mBlockSize;
    mNextPositionDisplay = mNextPosition;
    mBufferSize = mBlockSize;
    mBufferSize *= 2;

    mBuffer = FMOD_Memory_ReAlloc(mBuffer, mBufferSize);
    if (!mBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    result = getFileThread();
    if (result != FMOD_OK)
    {
        return result;
    }

    FMOD_OS_CriticalSection_Enter(mFileThread->mCrit);
    addAfter(&mFileThread->mHead);
    FMOD_OS_CriticalSection_Leave(mFileThread->mCrit);

    result = checkBufferedStatus();
    if (result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT File::getName(char * * name)
{
    if (name)
    {
        *name = mName;
    }

    return FMOD_OK;
}

FMOD_RESULT File::setName(char * name)
{
    if (!name)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(mName, name, 256);
    mName[255] = 0;

    return FMOD_OK;
}

// DiskFile (0x80609E34..0x80609FFC)

FMOD_RESULT DiskFile::reallyOpen(const char * name_or_data, unsigned int * filesize)
{
    FMOD_RESULT result;
    char name[2048];

    if (!FMOD_strlen(name_or_data))
    {
        return FMOD_ERR_FILE_NOTFOUND;
    }

    FMOD_strcpy(name, name_or_data);

    result = setName(name);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = FMOD_OS_File_Open(name_or_data, "rb", mUnicode, filesize, &mHandle);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT DiskFile::reallyClose()
{
    return FMOD_OS_File_Close(mHandle);
}

FMOD_RESULT DiskFile::reallyRead(void * buffer, unsigned int size, unsigned int * rd)
{
    FMOD_RESULT result;
    unsigned int r;

    result = FMOD_OS_File_Read(mHandle, buffer, size, &r);

    if (rd)
    {
        *rd = r;
    }

    if (result == FMOD_OK && size != r)
    {
        result = FMOD_ERR_FILE_EOF;
    }

    return result;
}

FMOD_RESULT DiskFile::reallySeek(unsigned int pos)
{
    return FMOD_OS_File_Seek(mHandle, pos);
}

// MemoryFile (0x8060A000..0x8060A140)

FMOD_RESULT MemoryFile::reallyOpen(const char * name_or_data, unsigned int * filesize)
{
    mMem = (void *)name_or_data;
    mPosition = 0;
    mUnk20 = false;

    return FMOD_OK;
}

FMOD_RESULT MemoryFile::reallyClose()
{
    return FMOD_OK;
}

FMOD_RESULT MemoryFile::reallyRead(void * buffer, unsigned int size, unsigned int * read)
{
    FMOD_RESULT result = FMOD_OK;

    if (mPosition + size > mLength)
    {
        size = mLength - mPosition;
        result = FMOD_ERR_FILE_EOF;
    }

    memcpy(buffer, (char *)mMem + mPosition, size);
    *read = size;
    mPosition += size;

    return result;
}

FMOD_RESULT MemoryFile::reallySeek(unsigned int pos)
{
    mPosition = pos;
    if (mPosition > mLength)
    {
        mPosition = mLength;
    }

    return FMOD_OK;
}

// NullFile (0x8060A144..0x8060A228)

FMOD_RESULT NullFile::reallyOpen(const char * name_or_data, unsigned int * filesize)
{
    mPosition = 0;
    *filesize = 0;

    return FMOD_OK;
}

FMOD_RESULT NullFile::reallyClose()
{
    return FMOD_OK;
}

FMOD_RESULT NullFile::reallyRead(void * buffer, unsigned int size, unsigned int * read)
{
    FMOD_RESULT result = FMOD_OK;

    if (mPosition + size > mLength)
    {
        size = mLength - mPosition;
        result = FMOD_ERR_INVALID_PARAM;
    }

    *read = size;
    mPosition += size;

    return result;
}

FMOD_RESULT NullFile::reallySeek(unsigned int pos)
{
    mPosition = pos;
    if (mPosition > mLength)
    {
        mPosition = mLength;
    }

    return FMOD_OK;
}

// UserFile (0x8060A22C..0x8060A3C4)

FMOD_FILE_OPENCALLBACK UserFile::gOpenCallback;
FMOD_FILE_CLOSECALLBACK UserFile::gCloseCallback;
FMOD_FILE_READCALLBACK UserFile::gReadCallback;
FMOD_FILE_SEEKCALLBACK UserFile::gSeekCallback;

FMOD_RESULT UserFile::reallyOpen(const char * name_or_data, unsigned int * filesize)
{
    FMOD_RESULT result = FMOD_OK;

    if (gOpenCallback)
    {
        result = gOpenCallback(name_or_data, mUnicode ? 1 : 0, filesize, &mHandle, &mUserData);
    }

    return result;
}

FMOD_RESULT UserFile::reallyClose()
{
    if (gCloseCallback)
    {
        gCloseCallback(mHandle, mUserData);
    }

    return FMOD_OK;
}

FMOD_RESULT UserFile::reallyRead(void * buffer, unsigned int size, unsigned int * rd)
{
    FMOD_RESULT result = FMOD_OK;

    if (gReadCallback)
    {
        result = gReadCallback(mHandle, buffer, size, rd, mUserData);
    }

    return result;
}

FMOD_RESULT UserFile::reallySeek(unsigned int pos)
{
    FMOD_RESULT result;

    if (gSeekCallback)
    {
        result = gSeekCallback(mHandle, pos, mUserData);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
