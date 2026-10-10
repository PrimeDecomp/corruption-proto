// G2MEAB File family. File is 0x19C bytes: the 4.06 members shifted by the 0x14-byte polymorphic
// node, without mDeviceType (mName follows mUnicode at +0x2D), mSema and the embedded 2048-byte
// buffer (mBuffer +0x158 is allocated by open 0x80608884 and grown by enableDoubleBuffer 0x80609B24).
// Offsets from init 0x80608788, open, flip 0x80608B80, checkBufferedStatus 0x80608E30, read
// 0x806090F8 and seek 0x806098C4. Vtable 0x806EDF10: destructor 0x80609CF4, getMetadata, getSize,
// then the four pure really* slots (zero). FileThread is 0x15C (getFileThread 0x80608640 allocation,
// constructor 0x8060845C). The DiskFile/MemoryFile/NullFile/UserFile code also lives in this unit.

#ifndef _FMOD_FILE_H
#define _FMOD_FILE_H

#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"
#include "fmod_thread.h"

namespace FMOD {
    class File;
    struct FileThread;
    class LinkedListNode;
    struct Metadata;
    struct SystemI;
}

namespace FMOD {

const unsigned int FILE_SECTORSIZE = 2048;

struct FileThread : public LinkedListNode
{
    Thread mThread; // offset 0x14
    bool mThreadActive; // offset 0x138
    LinkedListNode mHead; // offset 0x13C
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x150
    int mDeviceType; // offset 0x154
    bool mOwned; // offset 0x158
    FMOD_RESULT threadFunc();
    FileThread();
    FMOD_RESULT init(int devicetype, bool owned);
    FMOD_RESULT release();
};

class File : public LinkedListNode
{
    friend struct FileThread;
protected:
    unsigned int mLength; // offset 0x14
    unsigned int mLengthOriginal; // offset 0x18
    unsigned int mFileSize; // offset 0x1C
    bool mUnk20; // offset 0x20; set by init, cleared by MemoryFile::reallyOpen, gates the name copy in open
    void * mRiderUserData; // offset 0x24
    void * mRiderHandle; // offset 0x28
    bool mUnicode; // offset 0x2C
private:
    char mName[256]; // offset 0x2D
    char mEncryptionKey[32]; // offset 0x12D
    int mEncryptionKeyLength; // offset 0x150
    int mEncryptionKeyIndex; // offset 0x154
    void * mBuffer; // offset 0x158
    unsigned int mBufferPos; // offset 0x15C
    unsigned int mBufferSize; // offset 0x160
    unsigned int mBufferSkip; // offset 0x164
    unsigned int mBlockSize; // offset 0x168
    unsigned int mBlockOffset; // offset 0x16C
    unsigned int mCurrentPosition; // offset 0x170
    unsigned int mNextPosition; // offset 0x174
    unsigned int mNextPositionDisplay; // offset 0x178
    unsigned int mStartOffset; // offset 0x17C
    bool mBigEndian; // offset 0x180
    bool mBusy; // offset 0x181
    bool mStarving; // offset 0x182
    bool mExit; // offset 0x183
    int mPercentBuffered; // offset 0x184
    FMOD_RESULT mAsyncError; // offset 0x188
    bool mNeedsFlip; // offset 0x18C
    FileThread * mFileThread; // offset 0x190
public:
    SystemI * mSystem; // offset 0x194, set by SystemI::createSoundInternal 0x8061C418
    bool mSeekable; // offset 0x198

    // G2MEAB takes no frommainthread flag (callers 0x80608424, 0x80609030 leave r4 unset).
    FMOD_RESULT flip();
    FMOD_RESULT checkBufferedStatus();
    FMOD_RESULT seekAndReset();
    FMOD_RESULT getFileThread();
    File();
    FMOD_RESULT init(unsigned int filesize, int blocksize);
    FMOD_RESULT getByte(unsigned char * val);
    FMOD_RESULT getByte(unsigned short * val);
    FMOD_RESULT getByte(unsigned int * val);
    FMOD_RESULT getByte(signed char * val);
    FMOD_RESULT getByte(int * val);
    FMOD_RESULT getWord(unsigned short * val);
    FMOD_RESULT getWord(unsigned int * val);
    FMOD_RESULT getWord(int * val);
    FMOD_RESULT getDword(unsigned int * val);
    FMOD_RESULT getDword(int * val);
    FMOD_RESULT setStartOffset(unsigned int offset);
    FMOD_RESULT getStartOffset(unsigned int * offset);
    FMOD_RESULT enableDoubleBuffer(unsigned int sizebytes);
    FMOD_RESULT getName(char * * name);
    FMOD_RESULT setName(char * name);
    // Inline in G2MEAB: the codecs store mBigEndian directly (e.g. 0x805D65D0) and
    // SoundI::getOpenState 0x80618664 reads mPercentBuffered/mStarving directly.
    FMOD_RESULT setBigEndian(bool bigendian)
    {
        mBigEndian = bigendian;
        return FMOD_OK;
    }
    FMOD_RESULT isBusy(bool * busy, unsigned int * percentbuffered)
    {
        if (busy)
        {
            *busy = mBusy;
        }
        if (percentbuffered)
        {
            *percentbuffered = mPercentBuffered;
        }
        return FMOD_OK;
    }
    FMOD_RESULT isStarving(bool * starving)
    {
        if (starving)
        {
            *starving = mStarving;
        }
        return FMOD_OK;
    }
    FMOD_RESULT open(const char * name_or_data, unsigned int length, bool unicode, const char * encryptionkey);
    FMOD_RESULT close();
    // G2MEAB: inline; the weak copies are 0x805C1A14 (fmod_codec) and 0x805C2DE4 (fmod_codec_aiff),
    // File vtable 0x806EDF10 slots 1 and 2.
    virtual FMOD_RESULT getMetadata(Metadata * * metadata) { return FMOD_ERR_TAGNOTFOUND; }
    virtual FMOD_RESULT getSize(unsigned int * size) { *size = mLength; return FMOD_OK; }
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize) = 0;
    virtual FMOD_RESULT reallyClose() = 0;
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * rd) = 0;
    virtual FMOD_RESULT reallySeek(unsigned int pos) = 0;
    FMOD_RESULT read(void * buffer, unsigned int size, unsigned int count, unsigned int * rd);
    FMOD_RESULT seek(int pos, int mode);
    FMOD_RESULT tell(unsigned int * pos);
    static FMOD_RESULT shutDown();
    static LinkedListNode gFileThreadHead; // 0x8075427C
    static unsigned int gBufferSize; // 0x80796F00 (0x800), File() 0x80608768
};

} // namespace FMOD

namespace FMOD {

void fileThreadFunc(void * data);

} // namespace FMOD

#endif
