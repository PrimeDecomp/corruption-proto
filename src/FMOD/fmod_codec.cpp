// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805C168C..0x805C2540 (12 native functions).
// Original basename named by the allocation/free file strings ("fmod_codec.cpp").
// Also emits the weak TagNode destructor 0x805C19B0 and File::getMetadata 0x805C1A14 (inline,
// header-defined), the implicit Codec destructor 0x805C2414 and the FMOD_CODEC_DESCRIPTION_EX
// destructor 0x805C24C4 with its -0x2C adjustor 0x805C2538.
// __LINE__ anchors: release frees at line 43, getMetadataFromFile allocates at line 173 and
// metaData at line 404; the layout of this file keeps those lines.
// setPosition differs from 4.06: no getwaveformat callback (the subsound format is copied from the
// waveformat array into mWaveFormat) and it clears mPCMBufferOffsetBytes before seeking.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod_codec.h"
#include "fmod.h"
#include "fmod_codeci.h"
#include "fmod_file.h"
#include "fmod_memory.h"
#include "fmod_metadata.h"
#include "fmod_soundi.h"

#include <string.h>

namespace FMOD {








FMOD_RESULT Codec::release()
{
    if (mDescription.close)
    {
        mDescription.close(this);
    }

    if (mFile)
    {
        mFile->close();
        FMOD_Memory_Free(mFile);
        mFile = 0;
    }

    return Plugin::release();
}

FMOD_RESULT Codec::read(void * buffer, unsigned int sizebytes, unsigned int * bytesread)
{
    FMOD_RESULT result = FMOD_OK;
    bool checkmetadata = false;
    unsigned int read = 0;

    if (mPCMBuffer && mPCMBufferLengthBytes)
    {
        while (sizebytes)
        {
            unsigned int lenbytes = sizebytes;
            unsigned int bytesdecoded = 0;

            if (!mPCMBufferOffsetBytes)
            {
                result = mDescription.read(this, mPCMBuffer, mPCMBufferLengthBytes, &bytesdecoded);
                if (result != FMOD_OK)
                {
                    break;
                }

                mPCMBufferFilledBytes = bytesdecoded;

                lenbytes = bytesdecoded;
                if (lenbytes > sizebytes)
                {
                    lenbytes = sizebytes;
                }

                checkmetadata = true;
            }

            if (mPCMBufferOffsetBytes + lenbytes > mPCMBufferFilledBytes)
            {
                lenbytes = mPCMBufferFilledBytes - mPCMBufferOffsetBytes;
            }

            memcpy((char *)buffer + read, mPCMBuffer + mPCMBufferOffsetBytes, lenbytes);

            mPCMBufferOffsetBytes += lenbytes;
            if (mPCMBufferOffsetBytes >= mPCMBufferFilledBytes)
            {
                mPCMBufferOffsetBytes = 0;
            }

            sizebytes -= lenbytes;
            read += lenbytes;
        }
    }
    else
    {
        result = mDescription.read(this, buffer, sizebytes, &read);
        if (result == FMOD_OK)
        {
            checkmetadata = true;
        }
    }

    if (checkmetadata)
    {
        getMetadataFromFile();
    }

    if (bytesread)
    {
        *bytesread = read;
    }

    return result;
}








































FMOD_RESULT Codec::getMetadataFromFile()
{
    FMOD_RESULT result = FMOD_OK;

    if (mFile)
    {
        Metadata * filemetadata;

        result = mFile->getMetadata(&filemetadata);
        if (result == FMOD_OK)
        {
            if (!mMetadata)
            {
                mMetadata = FMOD_Object_Alloc(Metadata);
                if (!mMetadata)
                {
                    return FMOD_ERR_MEMORY;
                }
            }

            result = mMetadata->add(filemetadata);
        }
    }

    return result;
}

FMOD_RESULT Codec::getLength(unsigned int * length, FMOD_TIMEUNIT lengthtype)
{
    FMOD_RESULT result;

    if (lengthtype == FMOD_TIMEUNIT_RAWBYTES)
    {
        *length = mWaveFormat.lengthbytes;
        return FMOD_OK;
    }

    if (!mDescription.getlength)
    {
        *length = 0;
        return FMOD_ERR_UNSUPPORTED;
    }

    result = mDescription.getlength(this, length, lengthtype);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT Codec::setPosition(int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;

    if (subsound < 0 || (numsubsounds && subsound >= numsubsounds))
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mDescription.setposition)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (numsubsounds > 0 && &mWaveFormat != &waveformat[subsound])
    {
        memcpy(&mWaveFormat, &waveformat[subsound], sizeof(FMOD_CODEC_WAVEFORMAT));
    }

    if (mDescription.timeunits & FMOD_TIMEUNIT_PCM)
    {
        if (postype & FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getSamplesFromBytes(position, &position, mWaveFormat.channels, mWaveFormat.format);
            postype = FMOD_TIMEUNIT_PCM;
        }
        else if (postype & FMOD_TIMEUNIT_MS)
        {
            position = (unsigned int)((float)position / 1000.0f * (float)mWaveFormat.frequency);
            postype = FMOD_TIMEUNIT_PCM;
        }
    }
    else if (mDescription.timeunits & FMOD_TIMEUNIT_PCMBYTES)
    {
        if (postype & FMOD_TIMEUNIT_PCM)
        {
            SoundI::getBytesFromSamples(position, &position, mWaveFormat.channels, mWaveFormat.format);
            postype = FMOD_TIMEUNIT_PCMBYTES;
        }
        else if (postype & FMOD_TIMEUNIT_MS)
        {
            position = (unsigned int)((float)position / 1000.0f * (float)mWaveFormat.frequency);
            SoundI::getBytesFromSamples(position, &position, mWaveFormat.channels, mWaveFormat.format);
            postype = FMOD_TIMEUNIT_PCMBYTES;
        }
    }
    else if (mDescription.timeunits & FMOD_TIMEUNIT_MS)
    {
        if (postype & FMOD_TIMEUNIT_PCM)
        {
            position = (unsigned int)(1000.0f * ((float)position / (float)mWaveFormat.frequency));
            postype = FMOD_TIMEUNIT_MS;
        }
        else if (postype & FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getSamplesFromBytes(position, &position, mWaveFormat.channels, mWaveFormat.format);
            position = (unsigned int)(1000.0f * ((float)position / (float)mWaveFormat.frequency));
            postype = FMOD_TIMEUNIT_MS;
        }
    }

    if (!(postype & mDescription.timeunits))
    {
        return FMOD_ERR_FORMAT;
    }

    mPCMBufferOffsetBytes = 0;

    result = mDescription.setposition(this, subsound, position, postype);
    if (result != FMOD_OK && result != FMOD_ERR_FILE_EOF)
    {
        return result;
    }

    mSubSoundIndex = subsound;

    return FMOD_OK;
}

FMOD_RESULT Codec::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;

    if (postype == FMOD_TIMEUNIT_RAWBYTES)
    {
        if (!mFile)
        {
            *position = 0;
        }

        result = mFile->tell(position);
        if (result != FMOD_OK)
        {
            *position = 0;
            return result;
        }

        *position -= mSrcDataOffset;
    }

    if (!mDescription.getposition)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (!(postype & mDescription.timeunits))
    {
        return FMOD_ERR_FORMAT;
    }

    result = mDescription.getposition(this, position, postype);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}






































































FMOD_RESULT Codec::metaData(FMOD_TAGTYPE type, const char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, bool unique)
{
    if (!mMetadata)
    {
        mMetadata = FMOD_Object_Alloc(Metadata);
        if (!mMetadata)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    return mMetadata->addTag(type, name, data, datalen, datatype, unique);
}

} // namespace FMOD
