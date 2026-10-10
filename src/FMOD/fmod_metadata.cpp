// Complete reconstruction of the G2MEAB unit (.text 0x8060C2B4..0x8060CA08).

#include "fmod_metadata.h"
#include "fmod.h"
#include "fmod_memory.h"
#include "fmod_string.h"

#include <string.h>

namespace FMOD {

FMOD_RESULT TagNode::init(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype)
{
    mName = FMOD_strdup(name);
    if (!mName)
    {
        return FMOD_ERR_MEMORY;
    }

    mData[0] = FMOD_Memory_Calloc(datatype == FMOD_TAGDATATYPE_STRING ? datalen + 1 : datalen);
    if (!mData[0])
    {
        return FMOD_ERR_MEMORY;
    }

    memcpy(mData[0], data, datalen);

    mDataLen = datalen;
    mType = type;
    mDataType = datatype;
    mUpdated = true;
    mUnique = false;
    mCurrentBuffer = 0;

    return FMOD_OK;
}

FMOD_RESULT TagNode::release()
{
    if (mName)
    {
        FMOD_Memory_Free(mName);
        mName = 0;
    }

    if (mData[0])
    {
        FMOD_Memory_Free(mData[0]);
        mData[0] = 0;
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT TagNode::update(void * data, unsigned int datalen)
{
    if (mDataLen == datalen && !memcmp(mData[0], data, datalen))
    {
        mUpdated = true;
        return FMOD_OK;
    }

    if (mData[0])
    {
        FMOD_Memory_Free(mData[0]);
        mData[0] = 0;
    }

    mData[0] = FMOD_Memory_Alloc(datalen);
    if (!mData[0])
    {
        return FMOD_ERR_MEMORY;
    }

    memcpy(mData[0], data, datalen);
    mDataLen = datalen;
    mUpdated = true;

    return FMOD_OK;
}

FMOD_RESULT Metadata::release()
{
    TagNode * current = (TagNode *)mList.getNext();

    while (current != &mList)
    {
        TagNode * next = (TagNode *)current->getNext();

        current->removeNode();
        current->release();
        current = next;
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT Metadata::getNumTags(int * numtags, int * numtagsupdated)
{
    int count = 0;
    int countupdated = 0;
    TagNode * current;

    for (current = (TagNode *)mList.getNext(); current != &mList; current = (TagNode *)current->getNext())
    {
        count++;
        if (current->mUpdated)
        {
            countupdated++;
        }
    }

    if (numtags)
    {
        *numtags = count;
    }
    if (numtagsupdated)
    {
        *numtagsupdated = countupdated;
    }

    return FMOD_OK;
}

FMOD_RESULT Metadata::getTag(const char * name, int index, FMOD_TAG * tag)
{
    TagNode * current;

    if (index >= 0)
    {
        if (!name)
        {
            current = (TagNode *)mList.getNodeByIndex(index);
            if (!current)
            {
                return FMOD_ERR_TAGNOTFOUND;
            }
        }
        else
        {
            int count = 0;

            for (current = (TagNode *)mList.getNext(); ; current = (TagNode *)current->getNext())
            {
                if (current == &mList)
                {
                    return FMOD_ERR_TAGNOTFOUND;
                }
                if (!FMOD_strcmp(current->mName, name))
                {
                    if (count == index)
                    {
                        break;
                    }
                    count++;
                }
            }
        }
    }
    else
    {
        if (!name)
        {
            for (current = (TagNode *)mList.getNext(); ; current = (TagNode *)current->getNext())
            {
                if (current == &mList)
                {
                    return FMOD_ERR_TAGNOTFOUND;
                }
                if (current->mUpdated)
                {
                    break;
                }
            }
        }
        else
        {
            for (current = (TagNode *)mList.getNext(); ; current = (TagNode *)current->getNext())
            {
                if (current == &mList)
                {
                    return FMOD_ERR_TAGNOTFOUND;
                }
                if (current->mUpdated && !FMOD_strcmp(current->mName, name))
                {
                    break;
                }
            }
        }
    }

    tag->type = current->mType;
    tag->datatype = current->mDataType;
    tag->name = current->mName;
    tag->data = current->mData[0];
    tag->datalen = current->mDataLen;
    tag->updated = current->mUpdated;

    if (current->mUpdated)
    {
        current->mUpdated = false;
    }

    return FMOD_OK;
}

FMOD_RESULT Metadata::add(Metadata * metadata)
{
    TagNode * current = (TagNode *)metadata->mList.getNext();

    while (current != &metadata->mList)
    {
        TagNode * next = (TagNode *)current->getNext();

        current->removeNode();

        if (current->mUnique)
        {
            TagNode * existing;

            for (existing = (TagNode *)mList.getNext(); ; existing = (TagNode *)existing->getNext())
            {
                if (existing == &mList)
                {
                    existing = 0;
                    break;
                }
                if (!FMOD_strcmp(existing->mName, current->mName))
                {
                    break;
                }
            }

            if (existing)
            {
                existing->update(current->mData[0], current->mDataLen);
                current->release();
            }
            else
            {
                addTag(current);
            }
        }
        else
        {
            addTag(current);
        }

        current = next;
    }

    return FMOD_OK;
}

FMOD_RESULT Metadata::addTag(TagNode * node)
{
    node->addBefore(&mList);

    return FMOD_OK;
}

FMOD_RESULT Metadata::addTag(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique)
{
    FMOD_RESULT result;
    TagNode * node = 0;

    if (unique)
    {
        for (node = (TagNode *)mList.getNext(); ; node = (TagNode *)node->getNext())
        {
            if (node == &mList)
            {
                node = 0;
                break;
            }
            if (!FMOD_strcmp(node->mName, name) && node->mType == type)
            {
                break;
            }
        }
    }

    if (node && unique)
    {
        result = node->update(data, datalen);
    }
    else
    {
        node = FMOD_Object_Alloc(TagNode);
        if (!node)
        {
            return FMOD_ERR_MEMORY;
        }

        node->init(type, name, data, datalen, datatype);
        result = addTag(node);
    }

    if (unique)
    {
        node->mUnique = true;
    }

    return result;
}

} // namespace FMOD
