// G2MEAB prototype translation unit; all retained native functions reconstructed.
// .text: 0x806262F4..0x806267E0 (12 native functions).
// Source identity: inferred from non-real-time no-sound output descriptor. Extent confidence: high.
// Complete native inventory retained, including callbacks and emitted helpers.
// Bodies reconstructed from the native code with the 4.06 reference (close is dead-stripped natively).
// 0x806262F4 +0xA4: non-real-time no-sound output descriptor builder
// 0x80626398 +0x10: retained native; no unsupported symbol identity assigned
// 0x806263A8 +0x38: retained native; no unsupported symbol identity assigned
// 0x806263E0 +0x50: retained native; no unsupported symbol identity assigned
// 0x80626430 +0x21C: retained native; no unsupported symbol identity assigned
// 0x8062664C +0x3C: retained native; no unsupported symbol identity assigned
// 0x80626688 +0x2C: retained native; no unsupported symbol identity assigned
// 0x806266B4 +0x2C: retained native; no unsupported symbol identity assigned
// 0x806266E0 +0x2C: retained native; no unsupported symbol identity assigned
// 0x8062670C +0x34: retained native; no unsupported symbol identity assigned
// 0x80626740 +0x2C: retained native; no unsupported symbol identity assigned
// 0x8062676C +0x74: registered no-sound output static initializer

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_output_nosound_nrt.h"
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

FMOD_OUTPUT_DESCRIPTION_EX nosoundoutput_nrt;

FMOD_OUTPUT_DESCRIPTION_EX * OutputNoSound_NRT::getDescriptionEx()
{
    memset(&nosoundoutput_nrt, 0, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));

    nosoundoutput_nrt.name = "FMOD NoSound Output - Non real-time";
    nosoundoutput_nrt.version = 0x00010100;
    nosoundoutput_nrt.polling = 0;
    nosoundoutput_nrt.getnumdrivers = &OutputNoSound_NRT::getNumDriversCallback;
    nosoundoutput_nrt.getdrivername = &OutputNoSound_NRT::getDriverNameCallback;
    nosoundoutput_nrt.getdrivercaps = &OutputNoSound_NRT::getDriverCapsCallback;
    nosoundoutput_nrt.init = &OutputNoSound_NRT::initCallback;
    nosoundoutput_nrt.update = &OutputNoSound_NRT::updateCallback;

    nosoundoutput_nrt.mType = FMOD_OUTPUTTYPE_NOSOUND_NRT;
    nosoundoutput_nrt.mSize = sizeof(OutputNoSound_NRT);

    return &nosoundoutput_nrt;
}

FMOD_RESULT OutputNoSound_NRT::getNumDrivers(int * numdrivers)
{
    *numdrivers = 1;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::getDriverName(int driver, char * name, int namelen)
{
    FMOD_strncpy(name, "NoSound Driver", namelen);

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::getDriverCaps(int id, FMOD_CAPS * caps)
{
    *caps |= FMOD_CAPS_OUTPUT_MULTICHANNEL;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM8;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM16;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM24;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCM32;
    *caps |= FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT;

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    FMOD_RESULT result;
    FMOD_SOUND_FORMAT format;
    unsigned int bufferlengthbytes;
    int channels;

    result = mSystem->getSoftwareFormat(0, &format, &channels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    mBufferLength = dspbufferlength;

    result = SoundI::getBytesFromSamples(mBufferLength, &bufferlengthbytes, channels, format);
    if (result != FMOD_OK)
    {
        return result;
    }

    mBuffer = FMOD_Memory_Calloc(bufferlengthbytes);
    if (!mBuffer)
    {
        return FMOD_ERR_MEMORY;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::close()
{
    if (mBuffer)
    {
        FMOD_Memory_Free(mBuffer);
        mBuffer = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::update()
{
    FMOD_RESULT result;

    result = mix(mBuffer, mBufferLength);
    if (result != FMOD_OK)
    {
        return FMOD_OK;
    }

    return FMOD_OK;
}

FMOD_RESULT OutputNoSound_NRT::getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->getNumDrivers(numdrivers);
}

FMOD_RESULT OutputNoSound_NRT::getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->getDriverName(id, name, namelen);
}

FMOD_RESULT OutputNoSound_NRT::getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->getDriverCaps(id, caps);
}

FMOD_RESULT OutputNoSound_NRT::initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->init(selecteddriver, flags, outputrate, outputchannels, outputformat, dspbufferlength, dspnumbuffers, extradriverdata);
}

FMOD_RESULT OutputNoSound_NRT::closeCallback(FMOD_OUTPUT_STATE * output)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->close();
}

FMOD_RESULT OutputNoSound_NRT::updateCallback(FMOD_OUTPUT_STATE * output)
{
    OutputNoSound_NRT * nosound = (OutputNoSound_NRT *)output;

    return nosound->update();
}

} // namespace FMOD
