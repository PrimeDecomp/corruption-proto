// G2MEAB DiskFile: 0x1A0 bytes (SystemI::createSoundInternal vtable store 0x8061C400, vtable 0x806EDF60).
// Its code is part of the fmod_file unit (0x80609E34..0x80609FFC).

#ifndef _FMOD_FILE_DISK_H
#define _FMOD_FILE_DISK_H

#include "fmod.h"
#include "fmod_file.h"

namespace FMOD {
    class DiskFile;
}

namespace FMOD {

class DiskFile : public File
{
    void * mHandle; // offset 0x19C, reallyOpen 0x80609EA8
public:
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * rd);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
};

} // namespace FMOD

#endif
