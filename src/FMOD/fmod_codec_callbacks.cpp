// G2MEAB prototype translation unit; complete reconstruction.
// G2MEAB .text: 0x805B5D10..0x805B5DA4 (3 native functions).
// Provisional adapter-family filename; original standalone placement is unproven.
// Holds the static Codec state default callbacks that PluginFactory::createCodec stores into
// FMOD_CODEC_STATE +0x14/+0x18/+0x1C (metadata, file seek, file read bridges).

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference.

#include "fmod.h"
#include "fmod_codeci.h"
#include "fmod_file.h"

namespace FMOD {

FMOD_RESULT Codec::defaultMetaData(FMOD_CODEC_STATE * codec, FMOD_TAGTYPE type, char * name, void * data, unsigned int datalen, FMOD_TAGDATATYPE datatype, int unique)
{
    return ((Codec *)codec)->metaData(type, name, data, datalen, datatype, unique == 1 ? true : false);
}

FMOD_RESULT Codec::defaultFileSeek(void * handle, unsigned int pos, void * userdata)
{
    File * fp = (File *)handle;

    return fp->seek(pos, 0);
}

FMOD_RESULT Codec::defaultFileRead(void * handle, void * buffer, unsigned int sizebytes, unsigned int * bytesread, void * userdata)
{
    File * fp = (File *)handle;

    return fp->read(buffer, 1, sizebytes, bytesread);
}

} // namespace FMOD
