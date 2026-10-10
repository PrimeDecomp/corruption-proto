// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x806083B8..0x8060A3C8 (55 retained native functions).
// directly named by target allocation/free body.
// Evidence: Thread entry083B8/pump083D8,Thread ctor0845C,start084F4,release085AC and default worker
// allocation08640 share worker list8075427C and per-file queues. File
// ctor08720/reset08788,open08884/close08A54,async pump08B80,buffer/read08D64/08E30/090F8,ten typed
// reads095C8..09878,seek/tell/subfile/buffer/name methods098C4..09CA4 form complete File lifecycle.
// Allocation/free/reallocation names fmod_file.cpp806EDF51. Two dtors09CF4/09D58 and list
// initializer09DD8 close regular methods. Preserve all retained helpers, callback thunks, raw-only
// natives and inline expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_file.h"
#include "fmod.h"
#include "fmod_linkedlist.h"
#include "fmod_os_misc.h"

namespace FMOD {

unsigned int File::gBufferSize;
int File::gFileBusy;
FMOD_OS_CRITICALSECTION * File::gFileCrit;
bool File::gUsesUserCallbacks;
LinkedListNode File::gFileThreadHead;

} // namespace FMOD

FMOD_RESULT FMOD_File_GetDiskBusy(int * busy)
{
}

FMOD_RESULT FMOD_File_SetDiskBusy(int busy)
{
}

namespace FMOD {

FileThread::FileThread()
{
}

FMOD_RESULT File::seek(int pos, int mode)
{
}

FMOD_RESULT FileThread::init(int devicetype, bool owned)
{
}

FMOD_RESULT FileThread::release()
{
}

FMOD_RESULT File::getFileThread()
{
}

FMOD_RESULT File::init(unsigned int filesize, int blocksize)
{
}

File::File()
{
}

FMOD_RESULT File::shutDown()
{
}

FMOD_RESULT File::open(const char * name_or_data, unsigned int length, bool unicode, const char * encryptionkey)
{
}

FMOD_RESULT File::flip(bool frommainthread)
{
}

FMOD_RESULT FileThread::threadFunc()
{
}

void fileThreadFunc(void * data)
{
}

FMOD_RESULT File::close()
{
}

FMOD_RESULT File::seekAndReset()
{
}

FMOD_RESULT File::checkBufferedStatus()
{
}

FMOD_RESULT File::read(void * buffer, unsigned int size, unsigned int count, unsigned int * rd)
{
}

FMOD_RESULT File::getByte(unsigned char * val)
{
}

FMOD_RESULT File::getByte(unsigned short * val)
{
}

FMOD_RESULT File::getByte(unsigned int * val)
{
}

FMOD_RESULT File::getByte(signed char * val)
{
}

FMOD_RESULT File::getByte(short * val)
{
}

FMOD_RESULT File::getByte(int * val)
{
}

FMOD_RESULT File::getWord(unsigned short * val)
{
}

FMOD_RESULT File::getWord(unsigned int * val)
{
}

FMOD_RESULT File::getWord(short * val)
{
}

FMOD_RESULT File::getWord(int * val)
{
}

FMOD_RESULT File::getDword(unsigned int * val)
{
}

FMOD_RESULT File::getDword(int * val)
{
}

FMOD_RESULT File::tell(unsigned int * pos)
{
}

FMOD_RESULT File::setStartOffset(unsigned int offset)
{
}

FMOD_RESULT File::getStartOffset(unsigned int * offset)
{
}

FMOD_RESULT File::enableDoubleBuffer(unsigned int sizebytes)
{
}

FMOD_RESULT File::getName(char * * name)
{
}

FMOD_RESULT File::setName(char * name)
{
}

} // namespace FMOD
