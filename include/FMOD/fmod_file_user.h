// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_FILE_USER_H
#define _FMOD_FILE_USER_H

#include "fmod.h"
#include "fmod_file.h"

namespace FMOD {
    class UserFile;
}

namespace FMOD {

class UserFile : public File
{
    FMOD_FILE_OPENCALLBACK mOpenCallback; // offset 0x9A4
    FMOD_FILE_CLOSECALLBACK mCloseCallback; // offset 0x9A8
    FMOD_FILE_READCALLBACK mReadCallback; // offset 0x9AC
    FMOD_FILE_SEEKCALLBACK mSeekCallback; // offset 0x9B0
    void * mHandle; // offset 0x9B4
    void * mUserData; // offset 0x9B8
public:
    UserFile();
    FMOD_RESULT setUserCallbacks(FMOD_FILE_OPENCALLBACK, FMOD_FILE_CLOSECALLBACK, FMOD_FILE_READCALLBACK, FMOD_FILE_SEEKCALLBACK);
    FMOD_RESULT setGlobalUserCallbacks(FMOD_FILE_OPENCALLBACK, FMOD_FILE_CLOSECALLBACK, FMOD_FILE_READCALLBACK, FMOD_FILE_SEEKCALLBACK);
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * rd);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
    static FMOD_FILE_OPENCALLBACK gOpenCallback;
    static FMOD_FILE_CLOSECALLBACK gCloseCallback;
    static FMOD_FILE_READCALLBACK gReadCallback;
    static FMOD_FILE_SEEKCALLBACK gSeekCallback;
};

} // namespace FMOD

#endif
