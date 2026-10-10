// G2MEAB metadata. TagNode is 0x34 (allocation in Metadata::addTag 0x8060C930); offsets from TagNode::init
// 0x8060C2B4 and Metadata::getTag 0x8060C5E4.

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
    FMOD_TAGTYPE mType; // offset 0x14
    FMOD_TAGDATATYPE mDataType; // offset 0x18
    char * mName; // offset 0x1C
    void * mData[2]; // offset 0x20, only [0] is used
    unsigned int mDataLen; // offset 0x28
    bool mUpdated; // offset 0x2C
    bool mUnique; // offset 0x2D
    int mCurrentBuffer; // offset 0x30
    // G2MEAB inline constructor (Codec::getMetadataFromFile 0x805C18A8); vtable 0x806E2D08.
    TagNode()
    {
        mType = FMOD_TAGTYPE_UNKNOWN;
        mDataType = FMOD_TAGDATATYPE_BINARY;
        mName = 0;
        mData[0] = mData[1] = 0;
        mDataLen = 0;
        mUpdated = true;
        mUnique = false;
        mCurrentBuffer = 0;
    }
    FMOD_RESULT init(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype);
    FMOD_RESULT release();
    FMOD_RESULT update(void * data, unsigned int datalen);
};

struct Metadata
{
    TagNode mList; // offset 0x0 (list head)
    FMOD_RESULT release();
    FMOD_RESULT getNumTags(int * numtags, int * numtagsupdated);
    FMOD_RESULT getTag(const char * name, int index, FMOD_TAG * tag);
    FMOD_RESULT add(Metadata * metadata);
    FMOD_RESULT addTag(TagNode * node);
    FMOD_RESULT addTag(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique);
};

} // namespace FMOD

#endif
