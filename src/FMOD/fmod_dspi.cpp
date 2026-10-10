// NonMatching translation-unit scaffold; function bodies are empty placeholders.
// G2MEAB .text: 0x80606C50..0x806083B8 (36 retained native functions).
// directly named by target allocation/free body.
// Evidence: Leading recursive reachability06C50 calls own input getter06E2C and is used by graph
// connect073E0 for cycle checks. Descriptor install06D14,input/output connection
// getters06E2C/06EC8,graph reset06F98/07008 and ctor070B8 share lists+28/+3C, counts+50/+54 and
// descriptor+74. Release071AC,graph buffer allocation072E4,connect073E0/disconnect076D0 name
// fmod_dspi.cpp806EDF00. Preserve all retained helpers, callback thunks, raw-only natives and
// inline expansions in target order.

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "fmod_dspi.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_dsp.h"
#include "fmod_dsp_connection.h"
#include "fmod_linkedlist.h"

namespace FMOD {

FMOD_RESULT DSPI::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
}

FMOD_RESULT DSPI::getNumInputs(int * numinputs)
{
}

FMOD_RESULT DSPI::getInput(int index, DSPConnection * * input, DSPI * * inputdsp)
{
}

FMOD_RESULT DSPI::doesUnitExist(DSPI * target)
{
}

FMOD_RESULT DSPI::getOutput(int index, DSPConnection * * output, DSPI * * outputdsp)
{
}

FMOD_RESULT DSPI::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
}

FMOD_RESULT DSPI::execute(void * inbuffer, void * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
}

FMOD_RESULT DSPI::stepBack(LinkedListNode * & current, DSPConnection * & connection, DSPI * & t, LinkedListNode * & next)
{
}

FMOD_RESULT DSPI::stepForwards(LinkedListNode * & current, DSPConnection * & connection, DSPConnection * & prevconnection, DSPI * & t, LinkedListNode * & next)
{
}

FMOD_RESULT DSPI::run(float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
}

FMOD_RESULT DSPI::resetVisited()
{
}

FMOD_RESULT DSPI::validate(DSP * dsp, DSPI * * dspi)
{
}

FMOD_RESULT DSPI::getInput(int index, DSPI * * input)
{
}

FMOD_RESULT DSPI::setPosition(unsigned int position)
{
}

FMOD_RESULT DSPI::calculateSpeakerLevels(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, FMOD_SPEAKERMODE speakermode, int channels, float * outlevels, int * numinputlevels)
{
}

DSPI::DSPI()
{
}

FMOD_RESULT DSPI::getSystemObject(System * * system)
{
}

FMOD_RESULT DSPI::updateTreeLevel(int level)
{
}

FMOD_RESULT DSPI::addInputInternal(DSPI * target, bool checkcircular, DSPConnection * connection, DSPConnection * * connection_out)
{
}

FMOD_RESULT DSPI::addInputQueued(DSPI * target, bool checkcircular, DSPConnection * * connection_out)
{
}

FMOD_RESULT DSPI::addInput(DSPI * target)
{
}

FMOD_RESULT DSPI::getNumOutputs(int * numoutputs)
{
}

FMOD_RESULT DSPI::disconnectFromInternal(DSPI * target)
{
}

FMOD_RESULT DSPI::disconnectFromQueued(DSPI * target)
{
}

FMOD_RESULT DSPI::disconnectFrom(DSPI * target)
{
}

FMOD_RESULT DSPI::release(bool freethis)
{
}

FMOD_RESULT DSPI::disconnectAllQueued(bool inputs, bool outputs)
{
}

FMOD_RESULT DSPI::disconnectAll(bool inputs, bool outputs)
{
}

FMOD_RESULT DSPI::getOutput(int index, DSPI * * output)
{
}

FMOD_RESULT DSPI::disconnectAllInternal(bool inputs, bool outputs)
{
}

FMOD_RESULT DSPI::remove()
{
}

FMOD_RESULT DSPI::setInputMix(int index, float volume)
{
}

FMOD_RESULT DSPI::getInputMix(int index, float * volume)
{
}

FMOD_RESULT DSPI::setInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT DSPI::getInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT DSPI::setOutputMix(int index, float volume)
{
}

FMOD_RESULT DSPI::getOutputMix(int index, float * volume)
{
}

FMOD_RESULT DSPI::setOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT DSPI::getOutputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
}

FMOD_RESULT DSPI::reset()
{
}

FMOD_RESULT DSPI::setParameter(int index, float value)
{
}

FMOD_RESULT DSPI::getParameter(int index, float * value, char * valuestr, int valuestrlen)
{
}

FMOD_RESULT DSPI::getNumParameters(int * numparams)
{
}

FMOD_RESULT DSPI::getParameterInfo(int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max)
{
}

FMOD_RESULT DSPI::showConfigDialog(void * hwnd, bool show)
{
}

FMOD_RESULT DSPI::getInfo(char * name, unsigned int * version, int * channels, int * configwidth, int * configheight)
{
}

FMOD_RESULT DSPI::getType(FMOD_DSP_TYPE * type)
{
}

FMOD_RESULT DSPI::setDefaults(float frequency, float volume, float pan, int priority)
{
}

FMOD_RESULT DSPI::getDefaults(float * frequency, float * volume, float * pan, int * priority)
{
}

FMOD_RESULT DSPI::setUserData(void * userdata)
{
}

FMOD_RESULT DSPI::getUserData(void * * userdata)
{
}

FMOD_RESULT DSPI::setTargetFrequency(int frequency)
{
}

FMOD_RESULT DSPI::getTargetFrequency(int * frequency)
{
}

} // namespace FMOD
