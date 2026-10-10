// G2MEAB prototype translation unit; all retained native functions reconstructed.
// G2MEAB .text: 0x8060F1B8..0x8060F80C (16 retained native functions).
// direct target filename in allocation/free body.
// Evidence: F1B8 fills shared descriptor80755E3C named FMOD NoSound Output and installs seven
// callbacks. F274/F284/F2BC report one NoSound Driver and capabilities; F30C/F500 allocate/free
// buffer using fmod_output_nosound.cpp806EE73B lines0xB9/0xDB. Shared buffer fields+204/+208 and
// clock-position/lock operations close the same output. Preserve retained helpers, thunks and
// inline expansions in observed native order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_nosound.h"
#include "fmod.h"
#include "fmod_output.h"
#include "fmod_outputi.h"
#include "fmod_memory.h"
#include "fmod_soundi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"
#include "fmod_time.h"

#include <string.h>

namespace FMOD {

FMOD_OUTPUT_DESCRIPTION_EX nosoundoutput;

FMOD_OUTPUT_DESCRIPTION_EX * OutputNoSound::getDescriptionEx()
{
    memset(&nosoundoutput, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    nosoundoutput.name = "FMOD NoSound Output";
    nosoundoutput.version = 0x00010100;
    nosoundoutput.polling = 1;
    nosoundoutput.getnumdrivers = &OutputNoSound::getNumDriversCallback;
    nosoundoutput.getdrivername = &OutputNoSound::getDriverNameCallback;
    nosoundoutput.getdrivercaps = &OutputNoSound::getDriverCapsCallback;
    nosoundoutput.init = &OutputNoSound::initCallback;
    nosoundoutput.close = &OutputNoSound::closeCallback;
    nosoundoutput.getposition = &OutputNoSound::getPositionCallback;
    nosoundoutput.lock = &OutputNoSound::lockCallback;

    nosoundoutput.mType = FMOD_OUTPUTTYPE_NOSOUND;
    nosoundoutput.mSize = sizeof(OutputNoSound);

    return &nosoundoutput;
}

FMOD_RESULT OutputNoSound::getNumDrivers(int * numdrivers)
{
    *numdrivers = 1;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::getDriverName(int driver, char * name, int namelen)
{
    FMOD_strncpy(name, "NoSound Driver", namelen);

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::getDriverCaps(int id, FMOD_CAPS * caps)
{
    *caps |= FMOD_CAPS_OUTPUT_MULTICHANNEL;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM8;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM16;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM24;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM32;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    Plugin::init();

    SoundI::getBytesFromSamples(dspbufferlength * dspnumbuffers, &mBufferLength, outputchannels, *outputformat);

    mBuffer = FMOD_Memory_Calloc(mBufferLength);
    if (!mBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::close()
{
    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
    }
    mBuffer = 0;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::getPosition(unsigned int * pcm)
{
    FMOD_RESULT result;
    unsigned int pos = 0;
    int outputrate;

    result = mSystem->getSoftwareFormat(&outputrate, 0, 0, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    FMOD_Time_Get(&pos);

    pos *= outputrate;
    pos /= 1000;

    *pcm = pos;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    offset %= mBufferLength;

    if (offset + length > mBufferLength)
    {
        *ptr1 = (char *)mBuffer + offset;
        *ptr2 = mBuffer;
        *len1 = mBufferLength - offset;
        *len2 = length - (mBufferLength - offset);
    }
    else
    {
        *ptr1 = (char *)mBuffer + offset;
        *ptr2 = 0;
        *len1 = length;
        *len2 = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound::getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->getNumDrivers(numdrivers);
}

FMOD_RESULT OutputNoSound::getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->getDriverName(id, name, namelen);
}

FMOD_RESULT OutputNoSound::getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->getDriverCaps(id, caps);
}

FMOD_RESULT OutputNoSound::initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->init(selecteddriver, flags, outputrate, outputchannels, outputformat, dspbufferlength, dspnumbuffers, extradriverdata);
}

FMOD_RESULT OutputNoSound::closeCallback(FMOD_OUTPUT_STATE * output)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->close();
}

FMOD_RESULT OutputNoSound::getPositionCallback(FMOD_OUTPUT_STATE * output, unsigned int * pcm)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->getPosition(pcm);
}

FMOD_RESULT OutputNoSound::lockCallback(FMOD_OUTPUT_STATE * output, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2)
{
    OutputNoSound * nosound = (OutputNoSound *)output;

    return nosound->lock(offset, length, ptr1, ptr2, len1, len2);
}

} // namespace FMOD
