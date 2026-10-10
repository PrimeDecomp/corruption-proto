// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_OUTPUT_NOSOUND_H
#define _FMOD_OUTPUT_NOSOUND_H

#include "fmod.h"
#include "fmod_outputi.h"
#include "fmod_output_polled.h"

struct FMOD_OUTPUT_STATE;
namespace FMOD {
    struct FMOD_OUTPUT_DESCRIPTION_EX;
}

namespace FMOD {

// Synthesized from the definitions in fmod_output_nosound.cpp: the 4.06 DWARF has no type entry.
// G2MEAB: sizeof 0x20C (getDescriptionEx 0x8060F1B8 mSize); polled output (description polling = 1,
// members after OutputPolled's 0x204 bytes); init 0x8060F30C fills +0x204 with the buffer size in bytes
// and allocates +0x208; lock 0x8060F5E8 wraps offsets around +0x204.
struct OutputNoSound : public OutputPolled
{
    unsigned int mBufferLength; // offset 0x204, Guessed name
    void * mBuffer; // offset 0x208, Guessed name

    static FMOD_OUTPUT_DESCRIPTION_EX * getDescriptionEx();
    FMOD_RESULT getNumDrivers(int * numdrivers);
    FMOD_RESULT getDriverName(int driver, char * name, int namelen);
    FMOD_RESULT getDriverCaps(int id, FMOD_CAPS * caps);
    FMOD_RESULT init(int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    FMOD_RESULT close();
    FMOD_RESULT getPosition(unsigned int * pcm);
    FMOD_RESULT lock(unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
    static FMOD_RESULT getNumDriversCallback(FMOD_OUTPUT_STATE * output, int * numdrivers);
    static FMOD_RESULT getDriverNameCallback(FMOD_OUTPUT_STATE * output, int id, char * name, int namelen);
    static FMOD_RESULT getDriverCapsCallback(FMOD_OUTPUT_STATE * output, int id, FMOD_CAPS * caps);
    static FMOD_RESULT initCallback(FMOD_OUTPUT_STATE * output, int selecteddriver, FMOD_INITFLAGS flags, int * outputrate, int outputchannels, FMOD_SOUND_FORMAT * outputformat, int dspbufferlength, int dspnumbuffers, void * extradriverdata);
    static FMOD_RESULT closeCallback(FMOD_OUTPUT_STATE * output);
    static FMOD_RESULT getPositionCallback(FMOD_OUTPUT_STATE * output, unsigned int * pcm);
    static FMOD_RESULT lockCallback(FMOD_OUTPUT_STATE * output, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
};

} // namespace FMOD

#endif
