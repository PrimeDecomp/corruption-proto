// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

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
    void * mHandle; // offset 0x9A4
public:
    DiskFile();
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * rd);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
};

} // namespace FMOD

#endif
