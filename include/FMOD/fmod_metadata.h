// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_METADATA_H
#define _FMOD_METADATA_H

#include "fmod.h"
#include "fmod_linkedlist.h"

struct FMOD_TAG;
namespace FMOD {
    struct Metadata;
    struct TagNode;
}

namespace FMOD {

struct TagNode : public LinkedListNode
{
    FMOD_TAGTYPE mType; // offset 0xC
    FMOD_TAGDATATYPE mDataType; // offset 0x10
    char * mName; // offset 0x14
    void * mData[2]; // offset 0x18
    unsigned int mDataLen; // offset 0x20
    bool mUpdated; // offset 0x24
    bool mUnique; // offset 0x25
    int mCurrentBuffer; // offset 0x28
    TagNode() {}
    FMOD_RESULT init(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype);
    FMOD_RESULT release();
    FMOD_RESULT update(void * data, unsigned int datalen);
};

struct Metadata
{
    TagNode mList; // offset 0x0
    FMOD_RESULT release();
    FMOD_RESULT getNumTags(int * numtags, int * numtagsupdated);
    FMOD_RESULT getTag(const char * name, int index, FMOD_TAG * tag);
    FMOD_RESULT add(Metadata * metadata);
    FMOD_RESULT addTag(TagNode * node);
    FMOD_RESULT addTag(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique);
};

} // namespace FMOD

#endif
