// G2MEAB fmod_dspi.cpp: complete reconstruction (group D).
// .text: 0x80606C50..0x806083B8 (36 native functions, in this order): doesUnitExist, alloc, getInput/getOutput
// (connection), execute (float, void), resetVisited, setPosition, ctor, release, getSystemObject,
// updateTreeLevel, addInput, disconnectFrom, remove and the remaining public virtuals in vtable order.

#include "fmod_dspi.h"
#include "fmod.h"
#include "fmod_dsp.h"
#include "fmod_dsp_connection.h"
#include "fmod_dsp_connectionpool.h"
#include "fmod_linkedlist.h"
#include "fmod_localcriticalsection.h"
#include "fmod_memory.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

FMOD_RESULT DSPI::doesUnitExist(DSPI * target)
{
    FMOD_RESULT result;
    int numinputs;
    int count;

    if (this == target)
    {
        return FMOD_OK;
    }

    result = getNumInputs(&numinputs);
    if (result != FMOD_OK)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    for (count = 0; count < numinputs; count++)
    {
        DSPConnection * connection;

        result = getInput(count, &connection);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = connection->mInputUnit->doesUnitExist(target);
        if (result == FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT DSPI::alloc(FMOD_DSP_DESCRIPTION_EX * description)
{
    FMOD_RESULT result;
    int samplerate;
    int numoutputchannels;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (description->channels < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->getSoftwareFormat(&samplerate, 0, &numoutputchannels, 0, 0, 0);
    if (result != FMOD_OK)
    {
        return result;
    }

    switch (description->mCategory)
    {
        case FMOD_DSP_CATEGORY_FILTER:
        {
            if (description->channels > numoutputchannels)
            {
                return FMOD_ERR_TOOMANYCHANNELS;
            }
            break;
        }
        case FMOD_DSP_CATEGORY_DSPCODEC:
        {
            if (!description->channels)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            break;
        }
        case FMOD_DSP_CATEGORY_SOUNDCARD:
        case FMOD_DSP_CATEGORY_WAVETABLE:
        {
            break;
        }
        case FMOD_DSP_CATEGORY_RESAMPLER:
        {
            if (description->channels)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    memcpy(&mDescription, description, sizeof(FMOD_DSP_DESCRIPTION_EX));

    mActive = false;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getInput(int index, DSPConnection * * connection)
{
    LinkedListNode * current;
    int count;

    if (index >= mNumInputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!connection)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = mInputHead.getNext();
    if (current == &mInputHead)
    {
        return FMOD_ERR_INTERNAL;
    }

    for (count = index; count > 0; count--)
    {
        current = current->getNext();
    }

    *connection = (DSPConnection *)current->getData();

    return FMOD_OK;
}

FMOD_RESULT DSPI::getOutput(int index, DSPConnection * * connection)
{
    LinkedListNode * current;
    int count;

    if (index >= mNumOutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }
    if (!connection)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    current = mOutputHead.getNext();
    if (current == &mOutputHead)
    {
        return FMOD_ERR_INTERNAL;
    }

    for (count = index; count > 0; count--)
    {
        current = current->getNext();
    }

    *connection = (DSPConnection *)current->getData();

    return FMOD_OK;
}

FMOD_RESULT DSPI::execute(float * inbuffer, float * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    return FMOD_ERR_INTERNAL;
}

FMOD_RESULT DSPI::execute(void * inbuffer, void * * outbuffer, unsigned int * length, int inchannels, int * outchannels, FMOD_SPEAKERMODE speakermode)
{
    return execute((float *)inbuffer, (float * *)outbuffer, length, inchannels, outchannels, speakermode);
}

FMOD_RESULT DSPI::resetVisited()
{
    LinkedListNode * current;

    current = mInputHead.getNext();
    while (current != &mInputHead)
    {
        DSPConnection * connection = (DSPConnection *)current->getData();

        if (!connection)
        {
            break;
        }

        connection->mInputUnit->resetVisited();

        current = current->getNext();
    }

    mVisited = false;

    return FMOD_OK;
}

FMOD_RESULT DSPI::setPosition(unsigned int position)
{
    LinkedListNode * current;

    current = mInputHead.getNext();
    while (current != &mInputHead)
    {
        DSPConnection * connection = (DSPConnection *)current->getData();

        connection->mInputUnit->setPosition(position);

        current = current->getNext();
    }

    if (!mDescription.setposition)
    {
        return FMOD_OK;
    }

    instance = (FMOD_DSP *)this;

    return mDescription.setposition(this, position);
}

DSPI::DSPI()
{
    mOutputBuffer = 0;
    mBuffer = 0;
    mVisited = false;
    mActive = false;
    mBypass = false;
    mUnk63 = false;
    mNumInputs = 0;
    mNumOutputs = 0;
    mTreeLevel = -1;
    mAllocated = false;
    mDefaultVolume = 1.0f;
    mDefaultFrequency = 44100.0f;
    mDefaultPan = 0.0f;
    mDefaultPriority = 128;
}

FMOD_RESULT DSPI::release(bool freethis)
{
    FMOD_RESULT result;

    if (mSystem)
    {
        result = mSystem->stopDSP(this);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = remove();
    if (result != FMOD_OK)
    {
        result = disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    removeNode();

    if (mDescription.release)
    {
        instance = (FMOD_DSP *)this;
        mDescription.release(this);
    }

    if (freethis)
    {
        FMOD_Memory_Free(this);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::getSystemObject(System * * system)
{
    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *system = (System *)mSystem;

    return FMOD_OK;
}

FMOD_RESULT DSPI::updateTreeLevel(int level)
{
    LinkedListNode * current = mInputHead.getNext();

    mTreeLevel = level;

    if (level > 0 && !mSystem->mDSPMixBuff[level - 1])
    {
        int numoutputchannels = mSystem->mMaxOutputChannels;

        if (numoutputchannels < 2)
        {
            numoutputchannels = 2;
        }

        mSystem->mDSPMixBuff[level - 1] = (float *)FMOD_Memory_Calloc(mSystem->mDSPBlockSize * numoutputchannels * sizeof(float));
        if (!mSystem->mDSPMixBuff[level - 1])
        {
            return FMOD_ERR_MEMORY;
        }
    }

    while (current != &mInputHead)
    {
        DSPConnection * connection = (DSPConnection *)current->getData();

        connection->mInputUnit->updateTreeLevel(mTreeLevel + 1);

        current = current->getNext();
    }

    mOutputBuffer = mSystem->mDSPMixBuff[mTreeLevel];

    return FMOD_OK;
}

FMOD_RESULT DSPI::addInput(DSPI * target)
{
    FMOD_RESULT result;
    DSPConnection * connection;
    LocalCriticalSection crit(mSystem->mDSPCrit);

    if (!target)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mDescription.mCategory == FMOD_DSP_CATEGORY_RESAMPLER && target->mNumOutputs)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    if (target->mDescription.mCategory == FMOD_DSP_CATEGORY_SOUNDCARD)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    if (target->doesUnitExist(this) == FMOD_OK)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    crit.enter();

    if (mSystem->mDSPActive)
    {
        return FMOD_ERR_DSP_RUNNING;
    }

    result = mSystem->mDSPConnectionPool.alloc(&connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    connection->mInputNode.addBefore(&mInputHead);
    mNumInputs++;

    connection->mOutputNode.addBefore(&target->mOutputHead);
    target->mNumOutputs++;

    connection->mInputUnit = target;
    connection->mOutputUnit = this;

    connection->reset();

    if (mTreeLevel >= 0)
    {
        target->updateTreeLevel(mTreeLevel + 1);
    }

    mOutputBuffer = mSystem->mDSPMixBuff[mTreeLevel];

    if (target->mNumOutputs > 1)
    {
        if (!target->mBuffer)
        {
            int numoutputchannels = mSystem->mMaxOutputChannels;

            if (numoutputchannels < 2)
            {
                numoutputchannels = 2;
            }

            target->mBuffer = (float *)FMOD_Memory_Calloc(mSystem->mDSPBlockSize * numoutputchannels * sizeof(float));
            if (!target->mBuffer)
            {
                return FMOD_ERR_MEMORY;
            }
        }

        target->mOutputBuffer = target->mBuffer;
    }

    crit.leave();

    return FMOD_OK;
}

FMOD_RESULT DSPI::disconnectFrom(DSPI * target)
{
    FMOD_RESULT result;
    int numinputs;
    int count;
    LocalCriticalSection crit(mSystem->mDSPCrit);

    if (!target)
    {
        while (mNumInputs)
        {
            DSPConnection * connection;

            result = getInput(0, &connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            disconnectFrom(connection->mInputUnit);
        }

        while (mNumOutputs)
        {
            DSPConnection * connection;

            result = getOutput(0, &connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            connection->mOutputUnit->disconnectFrom(this);
        }

        return FMOD_OK;
    }

    crit.enter();

    if (mSystem->mDSPActive)
    {
        return FMOD_ERR_DSP_RUNNING;
    }

    if (getNumInputs(&numinputs) != FMOD_OK)
    {
        return FMOD_OK;
    }

    for (count = 0; count < numinputs; count++)
    {
        DSPConnection * connection;

        result = getInput(count, &connection);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (connection->mInputUnit == target)
        {
            connection->mInputNode.removeNode();
            mNumInputs--;

            if (mBuffer && mNumOutputs <= 1)
            {
                FMOD_Memory_Free(mBuffer);
                mBuffer = 0;
            }

            connection->mOutputNode.removeNode();
            target->mNumOutputs--;

            result = mSystem->mDSPConnectionPool.free(connection);
            if (result != FMOD_OK)
            {
                return result;
            }

            return FMOD_OK;
        }
    }

    crit.leave();

    return FMOD_ERR_DSP_NOTFOUND;
}

FMOD_RESULT DSPI::remove()
{
    FMOD_RESULT result;
    DSPI * input;
    DSPI * output;

    if (mNumOutputs != 1 && mNumInputs != 1)
    {
        return FMOD_ERR_DSP_CONNECTION;
    }

    result = setActive(false);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = getInput(0, &input);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = getOutput(0, &output);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = disconnectFrom(0);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = output->addInput(input);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::getNumInputs(int * numinputs)
{
    if (!numinputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numinputs = mNumInputs;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getNumOutputs(int * numoutputs)
{
    if (!numoutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numoutputs = mNumOutputs;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getInput(int index, DSPI * * input)
{
    FMOD_RESULT result;
    DSPConnection * connection;

    if (!input)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getInput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    *input = connection->mInputUnit;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getOutput(int index, DSPI * * output)
{
    FMOD_RESULT result;
    DSPConnection * connection;

    if (!output)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getOutput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    *output = connection->mOutputUnit;

    return FMOD_OK;
}

FMOD_RESULT DSPI::setInputMix(int index, float volume)
{
    FMOD_RESULT result;
    DSPConnection * connection;

    result = getInput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    return connection->setMix(volume);
}

FMOD_RESULT DSPI::getInputMix(int index, float * volume)
{
    FMOD_RESULT result;
    DSPConnection * connection;

    result = getInput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    return connection->getMix(volume);
}

FMOD_RESULT DSPI::reset()
{
    if (!mDescription.reset)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    instance = (FMOD_DSP *)this;

    return mDescription.reset(this);
}

FMOD_RESULT DSPI::setParameter(int index, float value)
{
    FMOD_RESULT result;

    if (!mDescription.setparameter)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (index < 0 || index > mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = mSystem->lockDSP();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (value < mDescription.paramdesc[index].min)
    {
        value = mDescription.paramdesc[index].min;
    }
    if (value > mDescription.paramdesc[index].max)
    {
        value = mDescription.paramdesc[index].max;
    }

    instance = (FMOD_DSP *)this;

    result = mDescription.setparameter(this, index, value);
    if (result != FMOD_OK)
    {
        mSystem->unlockDSP();
        return result;
    }

    result = mSystem->unlockDSP();
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::getParameter(int index, float * value, char * valuestr, int valuestrlen)
{
    FMOD_RESULT result;
    float v;
    char s[16];

    if (!mDescription.getparameter)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    if (index < 0 || index > mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    instance = (FMOD_DSP *)this;

    result = mDescription.getparameter(this, index, &v, s);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (value)
    {
        *value = v;
    }

    if (valuestr)
    {
        FMOD_strncpy(valuestr, s, valuestrlen > 16 ? 16 : valuestrlen);
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::getNumParameters(int * numparams)
{
    if (!numparams)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numparams = mDescription.numparameters;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getParameterInfo(int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max)
{
    if (index < 0 || index >= mDescription.numparameters)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (name)
    {
        FMOD_strcpy(name, mDescription.paramdesc[index].name);
    }

    if (description && descriptionlen)
    {
        if (mDescription.paramdesc[index].description)
        {
            FMOD_strncpy(description, mDescription.paramdesc[index].description, descriptionlen);
        }
        else
        {
            description[0] = 0;
        }
    }

    if (label)
    {
        FMOD_strcpy(label, mDescription.paramdesc[index].label);
    }

    if (min)
    {
        *min = mDescription.paramdesc[index].min;
    }

    if (max)
    {
        *max = mDescription.paramdesc[index].max;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::showConfigDialog(void * hwnd, bool show)
{
    if (!mDescription.config)
    {
        return FMOD_ERR_UNSUPPORTED;
    }

    instance = (FMOD_DSP *)this;

    return mDescription.config(this, hwnd, show);
}

FMOD_RESULT DSPI::getInfo(char * name, unsigned int * version, int * channels, int * configwidth, int * configheight)
{
    if (name)
    {
        FMOD_strncpy(name, mDescription.name, 32);
    }

    if (version)
    {
        *version = mDescription.version;
    }

    if (channels)
    {
        *channels = mDescription.channels;
    }

    if (configwidth)
    {
        *configwidth = mDescription.configwidth;
    }

    if (configheight)
    {
        *configheight = mDescription.configheight;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::setDefaults(float frequency, float volume, float pan, int priority)
{
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }
    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    if (pan < -1.0f)
    {
        pan = -1.0f;
    }
    if (pan > 1.0f)
    {
        pan = 1.0f;
    }
    if (priority < 0)
    {
        priority = 0;
    }
    if (priority > 256)
    {
        priority = 256;
    }

    mDefaultFrequency = frequency;
    mDefaultVolume = volume;
    mDefaultPan = pan;
    mDefaultPriority = priority;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getDefaults(float * frequency, float * volume, float * pan, int * priority)
{
    if (frequency)
    {
        *frequency = mDefaultFrequency;
    }
    if (volume)
    {
        *volume = mDefaultVolume;
    }
    if (pan)
    {
        *pan = mDefaultPan;
    }
    if (priority)
    {
        *priority = mDefaultPriority;
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::setUserData(void * userdata)
{
    mDescription.userdata = userdata;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getUserData(void * * userdata)
{
    if (!userdata)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *userdata = mDescription.userdata;

    return FMOD_OK;
}

FMOD_RESULT DSPI::setInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
    FMOD_RESULT result;
    DSPConnection * connection;
    float levelsarray[8][8];
    int count;

    result = getInput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = connection->getLevels(&levelsarray[0][0]);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < numlevels && count < 8; count++)
    {
        levelsarray[speaker][count] = levels[count];
    }

    return connection->setLevels(&levelsarray[0][0], 8);
}

FMOD_RESULT DSPI::getInputLevels(int index, FMOD_SPEAKER speaker, float * levels, int numlevels)
{
    FMOD_RESULT result;
    DSPConnection * connection;
    float levelsarray[8][8];
    int count;

    result = getInput(index, &connection);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = connection->getLevels(&levelsarray[0][0]);
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < numlevels && count < 8; count++)
    {
        levels[count] = levelsarray[speaker][count];
    }

    return FMOD_OK;
}

FMOD_RESULT DSPI::setTargetFrequency(int frequency)
{
    mTargetFrequency = frequency;

    return FMOD_OK;
}

FMOD_RESULT DSPI::getTargetFrequency(int * frequency)
{
    if (!frequency)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *frequency = mTargetFrequency;

    return FMOD_OK;
}

} // namespace FMOD
