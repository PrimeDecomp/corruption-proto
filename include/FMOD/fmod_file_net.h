// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_FILE_NET_H
#define _FMOD_FILE_NET_H

#include "fmod.h"
#include "fmod_file.h"
#include "fmod_metadata.h"

namespace FMOD {
    struct Metadata;
    class NetFile;
}

namespace FMOD {

class NetFile : public File
{
    void * mHandle; // offset 0x9A4
    int mProtocol; // offset 0x9A8
    unsigned int mAbsolutePos; // offset 0x9AC
    int mHttpStatus; // offset 0x9B0
    int mMetaint; // offset 0x9B4
    unsigned int mBytesBeforeMeta; // offset 0x9B8
    char * mMetabuf; // offset 0x9BC
    int mMetaFormat; // offset 0x9C0
    Metadata mMetadata; // offset 0x9C4
    unsigned short mPort; // offset 0x9F0
    char mHost[256]; // offset 0x9F2
    FMOD_RESULT mConnectStatus; // offset 0xAF4
    bool mChunked; // offset 0xAF8
    unsigned int mBytesLeftInChunk; // offset 0xAFC
public:
    void asyncConnectCallback(void *);
    FMOD_RESULT getSeekable(bool *);
    void asyncConnect();
    NetFile();
    static FMOD_RESULT init();
    static FMOD_RESULT shutDown();
    FMOD_RESULT parseUrl(char * url, char * host, int hostlen, char * auth, int authlen, unsigned short * port, char * file, int filelen, bool * mms);
    FMOD_RESULT openAsHTTP(const char * name_or_data, char * host, char * file, char * auth, unsigned short port, unsigned int * filesize);
    FMOD_RESULT openAsMMS(const char * name_or_data, char * host, char * file, char * auth, unsigned short port, unsigned int * filesize);
    virtual FMOD_RESULT reallyOpen(const char * name_or_data, unsigned int * filesize);
    virtual FMOD_RESULT reallyClose();
    virtual FMOD_RESULT reallyRead(void * buffer, unsigned int size, unsigned int * rd);
    virtual FMOD_RESULT reallySeek(unsigned int pos);
    virtual FMOD_RESULT getMetadata(Metadata * * metadata);
};

} // namespace FMOD

#endif
