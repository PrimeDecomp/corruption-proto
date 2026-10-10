// G2MEAB UserFile: 0x1A4 bytes (SystemI::createSoundInternal vtable store 0x8061C3BC and inline
// constructor, vtable 0x806EDFD8). The callbacks are only the statics 0x8079B850..0x8079B85C; there are
// no per-instance callbacks. Its code is part of the fmod_file unit (0x8060A22C..0x8060A3C4).

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
    void * mHandle; // offset 0x19C
    void * mUserData; // offset 0x1A0
public:
    UserFile()
    {
        mHandle = 0;
        mUserData = 0;
    }
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
