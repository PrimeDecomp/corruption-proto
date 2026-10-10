// G2MEAB PluginFactory (sizeof 0x27C, owned by SystemI+0xB5C). Offsets from the constructor 0x80610968
// (head nodes +0x12C/+0x1AC/+0x214, mSystem +0x278), getNumCodecs/DSPs/Outputs 0x80611344..0x8061140C
// and createDSP 0x80611904. Only the EX registration overloads exist in this build.

#ifndef _FMOD_PLUGINFACTORY_H
#define _FMOD_PLUGINFACTORY_H

#include "fmod.h"
#include "fmod_codeci.h"
#include "fmod_dspi.h"
#include "fmod_outputi.h"

struct FMOD_CODEC_DESCRIPTION;
struct FMOD_DSP_DESCRIPTION;
struct FMOD_OUTPUT_DESCRIPTION;
namespace FMOD {
    struct Codec;
    class DSPI;
    struct FMOD_CODEC_DESCRIPTION_EX;
    struct FMOD_DSP_DESCRIPTION_EX;
    struct FMOD_OUTPUT_DESCRIPTION_EX;
    class Output;
    class PluginFactory;
    struct SystemI;
}

namespace FMOD {

class PluginFactory
{
    char mPluginPath[256]; // offset 0x0
    FMOD_CODEC_DESCRIPTION_EX mCodecHead; // offset 0x100
    FMOD_DSP_DESCRIPTION_EX mDSPHead; // offset 0x150
    FMOD_OUTPUT_DESCRIPTION_EX mOutputHead; // offset 0x1E0
    SystemI * mSystem; // offset 0x278
public:
    PluginFactory();
    FMOD_RESULT release();
    FMOD_RESULT setSystem(SystemI * system);
    FMOD_RESULT getSystem(SystemI * * system);
    FMOD_RESULT setPluginPath(const char * path);
    FMOD_RESULT loadPlugin(const char * dllname, FMOD_PLUGINTYPE * plugintype, int * index, bool calledinternally);
    FMOD_RESULT unloadPlugin(FMOD_PLUGINTYPE type, int index);
    FMOD_RESULT registerCodec(FMOD_CODEC_DESCRIPTION * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT registerCodec(FMOD_CODEC_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT registerDSP(FMOD_DSP_DESCRIPTION * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT registerDSP(FMOD_DSP_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT registerOutput(FMOD_OUTPUT_DESCRIPTION * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT registerOutput(FMOD_OUTPUT_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index);
    FMOD_RESULT getNumCodecs(int * numcodecs);
    FMOD_RESULT getNumDSPs(int * numdsps);
    FMOD_RESULT getNumOutputs(int * numoutputs);
    FMOD_RESULT getCodec(int index, FMOD_CODEC_DESCRIPTION_EX * * codecdesc);
    FMOD_RESULT getDSP(int index, FMOD_DSP_DESCRIPTION_EX * * dspdesc);
    FMOD_RESULT getOutput(int index, FMOD_OUTPUT_DESCRIPTION_EX * * outputdesc);
    FMOD_RESULT createCodec(FMOD_CODEC_DESCRIPTION_EX * codecdesc, Codec * * codec);
    FMOD_RESULT createDSP(FMOD_DSP_DESCRIPTION_EX * dspdesc, DSPI * * dsp);
    FMOD_RESULT createOutput(FMOD_OUTPUT_DESCRIPTION_EX * outputdesc, Output * * output);
};

} // namespace FMOD

#endif
