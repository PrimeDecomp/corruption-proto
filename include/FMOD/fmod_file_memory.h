// G2MEAB MemoryFile: 0x1A4 bytes (embedded in DSPCodec at +0x17C, vtable 0x806EDF88). Its code is
// part of the fmod_file unit (0x8060A000..0x8060A140).

#ifndef _FMOD_FILE_MEMORY_H
#define _FMOD_FILE_MEMORY_H

#include "fmod.h"
#include "fmod_file.h"

namespace FMOD {
    class MemoryFile;
}

namespace FMOD {

class MemoryFile : public File
{
    unsigned int mPosition; // offset 0x19C
public:
    void * mMem; // offset 0x1A0, reallyOpen 0x8060A000
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * read);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
};

} // namespace FMOD

#endif
