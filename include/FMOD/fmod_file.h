// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    Thread mThread; // offset 0xC
    bool mThreadActive; // offset 0x130
    LinkedListNode mHead; // offset 0x134
    FMOD_OS_CRITICALSECTION * mCrit; // offset 0x140
    int mDeviceType; // offset 0x144
    bool mOwned; // offset 0x148
    FMOD_RESULT threadFunc();
    FileThread();
    FMOD_RESULT init(int devicetype, bool owned);
    FMOD_RESULT release();
};

class File : public LinkedListNode
{
protected:
    unsigned int mLength; // offset 0x10
    unsigned int mLengthOriginal; // offset 0x14
    unsigned int mFileSize; // offset 0x18
    bool mBufferIsString; // offset 0x1C
    void * mRiderUserData; // offset 0x20
    void * mRiderHandle; // offset 0x24
    bool mUnicode; // offset 0x28
    int mDeviceType; // offset 0x2C
private:
    char mName[256]; // offset 0x30
    char mEncryptionKey[32]; // offset 0x130
    int mEncryptionKeyLength; // offset 0x150
    int mEncryptionKeyIndex; // offset 0x154
    unsigned int mBufferPos; // offset 0x158
    unsigned int mBufferSize; // offset 0x15C
    unsigned int mBufferSkip; // offset 0x160
    unsigned int mBlockSize; // offset 0x164
    unsigned int mBlockOffset; // offset 0x168
    unsigned int mCurrentPosition; // offset 0x16C
    unsigned int mNextPosition; // offset 0x170
    unsigned int mNextPositionDisplay; // offset 0x174
    unsigned int mStartOffset; // offset 0x178
    bool mBigEndian; // offset 0x17C
    bool mBusy; // offset 0x17D
    bool mStarving; // offset 0x17E
    bool mExit; // offset 0x17F
    int mPercentBuffered; // offset 0x180
    FMOD_RESULT mAsyncError; // offset 0x184
    bool mNeedsFlip; // offset 0x188
    FileThread * mFileThread; // offset 0x18C
    SystemI * mSystem; // offset 0x190
    FMOD_OS_SEMAPHORE * mSema; // offset 0x194
public:
    FMOD_RESULT flip(bool frommainthread);
    FMOD_RESULT checkBufferedStatus();
    FMOD_RESULT seekAndReset();
    FMOD_RESULT getFileThread();
    bool mSeekable; // offset 0x198
    char mBufferMemory[2048]; // offset 0x1A0
    void * mBuffer; // offset 0x9A0
    File();
    FMOD_RESULT init(unsigned int filesize, int blocksize);
    FMOD_RESULT getByte(unsigned char * val);
    FMOD_RESULT getByte(unsigned short * val);
    FMOD_RESULT getByte(unsigned int * val);
    FMOD_RESULT getByte(signed char * val);
    FMOD_RESULT getByte(short * val);
    FMOD_RESULT getByte(int * val);
    FMOD_RESULT getWord(unsigned short * val);
    FMOD_RESULT getWord(unsigned int * val);
    FMOD_RESULT getWord(short * val);
    FMOD_RESULT getWord(int * val);
    FMOD_RESULT getDword(unsigned int * val);
    FMOD_RESULT getDword(int * val);
    FMOD_RESULT setStartOffset(unsigned int offset);
    FMOD_RESULT getStartOffset(unsigned int * offset);
    FMOD_RESULT enableDoubleBuffer(unsigned int sizebytes);
    FMOD_RESULT getName(char * * name);
    FMOD_RESULT setName(char * name);
    FMOD_RESULT setBigEndian(bool);
    FMOD_RESULT isBusy(bool *, unsigned int *);
    FMOD_RESULT isStarving(bool *);
    FMOD_RESULT open(const char * name_or_data, unsigned int length, bool unicode, const char * encryptionkey);
    FMOD_RESULT close();
    virtual FMOD_RESULT getMetadata(Metadata * * metadata);
    virtual FMOD_RESULT getSize(unsigned int * size);
    virtual FMOD_RESULT reallyOpen(const char *, unsigned int *);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void *, unsigned int, unsigned int *);
    virtual FMOD_RESULT reallySeek(unsigned int);
    FMOD_RESULT read(void * buffer, unsigned int size, unsigned int count, unsigned int * rd);
    FMOD_RESULT seek(int pos, int mode);
    FMOD_RESULT tell(unsigned int * pos);
    static FMOD_RESULT shutDown();
    static LinkedListNode gFileThreadHead;
    static FMOD_OS_CRITICALSECTION * gFileCrit;
    static int gFileBusy;
    static bool gUsesUserCallbacks;
    static unsigned int gBufferSize;
};

} // namespace FMOD

#ifdef __cplusplus
extern "C" {
#endif

FMOD_RESULT FMOD_File_GetDiskBusy(int * busy);
FMOD_RESULT FMOD_File_SetDiskBusy(int busy);

#ifdef __cplusplus
}
#endif

namespace FMOD {

void fileThreadFunc(void * data);

} // namespace FMOD

#endif
