// G2MEAB NullFile: 0x1A0 bytes (SystemI::createSoundInternal vtable store 0x8061C378, vtable 0x806EDFB0).
// Its code is part of the fmod_file unit (0x8060A144..0x8060A228).

#ifndef _FMOD_FILE_NULL_H
#define _FMOD_FILE_NULL_H

#include "fmod.h"
#include "fmod_file.h"

namespace FMOD {

class NullFile : public File
{
    unsigned int mPosition; // offset 0x19C
public:
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * read);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
};

} // namespace FMOD

#endif
