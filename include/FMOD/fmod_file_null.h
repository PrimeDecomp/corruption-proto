// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_FILE_NULL_H
#define _FMOD_FILE_NULL_H

#include "fmod.h"
#include "fmod_file.h"

namespace FMOD {

class NullFile : public File
{
    unsigned int mPosition; // offset 0x9A4
public:
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * read);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
};

} // namespace FMOD

#endif
