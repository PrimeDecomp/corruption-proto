// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_REVERBI_H
#define _FMOD_REVERBI_H

#include "fmod.h"
#include "fmod_linkedlist.h"

struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_REVERB_PROPERTIES;
struct FMOD_VECTOR;
namespace FMOD {
    class DSPConnection;
    class DSPI;
    struct FMOD_REVERB_CHANNELDATA;
    struct Reverb;
    class ReverbI;
    struct SystemI;
}

namespace FMOD {

struct FMOD_REVERB_CHANNELDATA
{
    FMOD_REVERB_CHANNELPROPERTIES mChanProps; // offset 0x0
    DSPConnection * mDSPConnection; // offset 0x48
};

class ReverbI : public LinkedListNode
{
public:
    ReverbI();
    ~ReverbI();
    FMOD_RESULT init(SystemI * system, unsigned int instance);
    FMOD_RESULT release();
    unsigned int getInstance();
    FMOD_RESULT setProperties(const FMOD_REVERB_PROPERTIES * prop_source);
    FMOD_RESULT getProperties(FMOD_REVERB_PROPERTIES * properties);
    FMOD_RESULT setChanProperties(int index, const FMOD_REVERB_CHANNELPROPERTIES * props, DSPConnection * connection);
    FMOD_RESULT getChanProperties(int index, FMOD_REVERB_CHANNELPROPERTIES * props, DSPConnection * * connection);
    FMOD_RESULT createDSP();
    FMOD_RESULT releaseDSP();
    DSPI * getDSP();
    FMOD_RESULT setGeometry(FMOD_VECTOR, float);
    FMOD_RESULT setPosition(FMOD_VECTOR);
    FMOD_RESULT setRadius(float);
    FMOD_RESULT getPosition(FMOD_VECTOR *);
    FMOD_RESULT getRadius(float *);
    static FMOD_RESULT validate(Reverb * reverb, ReverbI * * reverbi);
private:
    SystemI * mSystem; // offset 0xC
    DSPI * mDSP; // offset 0x10
    FMOD_REVERB_CHANNELDATA * mChannelData; // offset 0x14
    FMOD_REVERB_PROPERTIES mProps; // offset 0x18
    unsigned int mInstance; // offset 0x94
    FMOD_VECTOR mPosition; // offset 0x98
    float mRadius; // offset 0xA4
};

} // namespace FMOD

#endif
