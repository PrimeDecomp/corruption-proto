// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_OUTPUT_NOSOUND_NRT_H
#define _FMOD_OUTPUT_NOSOUND_NRT_H

#include "fmod.h"
#include "fmod_outputi.h"

struct FMOD_OUTPUT_STATE;
namespace FMOD {
    struct FMOD_OUTPUT_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_output_nosound_nrt.cpp: the 4.06 DWARF has no type entry.
// G2MEAB: sizeof 0xDC (getDescriptionEx 0x806262F4 mSize); init 0x80626430 stores dspbufferlength at +0xD4
// and the calloc'd buffer at +0xD8; update 0x8062664C mixes mBufferLength samples into mBuffer.
struct OutputNoSound_NRT : public Output
{
    unsigned int mBufferLength; // offset 0xD4, Guessed name
    void * mBuffer; // offset 0xD8, Guessed name

    static FMOD_OUTPUT_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int driver, char * name, int namelen);
    FMOD_RESULT getDriverCaps(int id, FMOD_CAPS * caps);
    FMOD_RESULT init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT update();
    static FMOD_RESULT getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers);
    static FMOD_RESULT getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen);
    static FMOD_RESULT getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps);
    static FMOD_RESULT initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    static FMOD_RESULT closeCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT updateCallback(FMOD_OUTPUT_STATE * output);
};

} // namespace FMOD

#endif
