// G2MEAB .text: 0x80610968..0x80611C3C (18 native functions), all reconstructed. The last native
// function 0x80611AF0 is the weak destructor of DSPCodec (vtable 0x806EE9B0), emitted here because
// createDSP instantiates it and DSPCodec declares no virtuals of its own.

#include "fmod_pluginfactory.h"
#include "fmod.h"
#include "fmod_codec.h"
#include "fmod_codeci.h"
#include "fmod_dsp.h"
#include "fmod_dspi.h"
#include "fmod_dsp_codec.h"
#include "fmod_dsp_filter.h"
#include "fmod_dsp_resampler.h"
#include "fmod_dsp_soundcard.h"
#include "fmod_dsp_wavetable.h"
#include "fmod_memory.h"
#include "fmod_os_misc.h"
#include "fmod_output.h"
#include "fmod_output_polled.h"
#include "fmod_outputi.h"
#include "fmod_string.h"
#include "fmod_systemi.h"

#include <string.h>

namespace FMOD {

PluginFactory::PluginFactory()
{
    mSystem = 0;

    memset(mPluginPath, 0, 256);

    mDSPHead.initNode();
    mCodecHead.initNode();
    mOutputHead.initNode();
}

FMOD_RESULT PluginFactory::release()
{
    FMOD_RESULT result;
    int num;
    int count;

    result = getNumCodecs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }
    for (count = 0; count < num; count++)
    {
        result = unloadPlugin(FMOD_PLUGINTYPE_CODEC, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = getNumDSPs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }
    for (count = 0; count < num; count++)
    {
        result = unloadPlugin(FMOD_PLUGINTYPE_DSP, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = getNumOutputs(&num);
    if (result != FMOD_OK)
    {
        return result;
    }
    for (count = 0; count < num; count++)
    {
        result = unloadPlugin(FMOD_PLUGINTYPE_OUTPUT, 0);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    FMOD_Memory_Free(this);

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::setSystem(SystemI * system)
{
    mSystem = system;

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::setPluginPath(const char * path)
{
    if (FMOD_strlen(path) >= 256)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    FMOD_strncpy(mPluginPath, path, 256);

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::unloadPlugin(FMOD_PLUGINTYPE type, int index)
{
    FMOD_RESULT result;

    switch (type)
    {
        case FMOD_PLUGINTYPE_OUTPUT:
        {
            FMOD_OUTPUT_DESCRIPTION_EX * output;

            result = getOutput(index, &output);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (output->mModule)
            {
                FMOD_OS_Library_Free(output->mModule);
            }

            output->removeNode();
            FMOD_Memory_Free(output);
            break;
        }
        case FMOD_PLUGINTYPE_CODEC:
        {
            FMOD_CODEC_DESCRIPTION_EX * codec;

            result = getCodec(index, &codec);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (codec->mModule)
            {
                FMOD_OS_Library_Free(codec->mModule);
            }

            codec->removeNode();
            FMOD_Memory_Free(codec);
            break;
        }
        case FMOD_PLUGINTYPE_DSP:
        {
            FMOD_DSP_DESCRIPTION_EX * dsp;

            result = getDSP(index, &dsp);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (dsp->mAEffect && dsp->paramdesc)
            {
                FMOD_Memory_Free(dsp->paramdesc);
            }

            if (dsp->mModule)
            {
                FMOD_OS_Library_Free(dsp->mModule);
            }

            dsp->removeNode();
            FMOD_Memory_Free(dsp);
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::registerCodec(FMOD_CODEC_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index)
{
    FMOD_CODEC_DESCRIPTION_EX * newcodec;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newcodec = (FMOD_CODEC_DESCRIPTION_EX *)FMOD_Memory_Calloc(sizeof(FMOD_CODEC_DESCRIPTION_EX));
    if (!newcodec)
    {
        return FMOD_ERR_MEMORY;
    }

    newcodec->name = description->name;
    newcodec->version = description->version;
    newcodec->timeunits = description->timeunits;
    newcodec->defaultasstream = description->defaultasstream;
    newcodec->open = description->open;
    newcodec->close = description->close;
    newcodec->read = description->read;
    newcodec->getlength = description->getlength;
    newcodec->setposition = description->setposition;
    newcodec->getposition = description->getposition;
    newcodec->soundcreate = description->soundcreate;
    newcodec->mType = description->mType;
    newcodec->mSize = description->mSize;
    newcodec->mModule = description->mModule;

    newcodec->addBefore(&mCodecHead);

    if (plugintype)
    {
        *plugintype = FMOD_PLUGINTYPE_CODEC;
    }
    if (index)
    {
        *index = mCodecHead.getNodeIndex(newcodec);
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::registerDSP(FMOD_DSP_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index)
{
    FMOD_DSP_DESCRIPTION_EX * newdsp;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newdsp = (FMOD_DSP_DESCRIPTION_EX *)FMOD_Memory_Calloc(sizeof(FMOD_DSP_DESCRIPTION_EX));
    if (!newdsp)
    {
        return FMOD_ERR_MEMORY;
    }

    FMOD_strcpy(newdsp->name, description->name);
    newdsp->version = description->version;
    newdsp->channels = description->channels;
    newdsp->create = description->create;
    newdsp->release = description->release;
    newdsp->reset = description->reset;
    newdsp->read = description->read;
    newdsp->setposition = description->setposition;
    newdsp->numparameters = description->numparameters;
    newdsp->paramdesc = description->paramdesc;
    newdsp->setparameter = description->setparameter;
    newdsp->getparameter = description->getparameter;
    newdsp->config = description->config;
    newdsp->configwidth = description->configwidth;
    newdsp->configheight = description->configheight;
    newdsp->userdata = description->userdata;
    newdsp->mType = description->mType;
    newdsp->mCategory = description->mCategory;
    newdsp->mSize = description->mSize;
    newdsp->mModule = description->mModule;
    newdsp->mAEffect = description->mAEffect;
    newdsp->mFormat = description->mFormat;
    newdsp->mResamplerBlockLength = description->mResamplerBlockLength;

    newdsp->addBefore(&mDSPHead);

    if (plugintype)
    {
        *plugintype = FMOD_PLUGINTYPE_DSP;
    }
    if (index)
    {
        *index = mDSPHead.getNodeIndex(newdsp);
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::registerOutput(FMOD_OUTPUT_DESCRIPTION_EX * description, FMOD_PLUGINTYPE * plugintype, int * index)
{
    FMOD_OUTPUT_DESCRIPTION_EX * newoutput;

    if (!description)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newoutput = (FMOD_OUTPUT_DESCRIPTION_EX *)FMOD_Memory_Calloc(sizeof(FMOD_OUTPUT_DESCRIPTION_EX));
    if (!newoutput)
    {
        return FMOD_ERR_MEMORY;
    }

    newoutput->name = description->name;
    newoutput->version = description->version;
    newoutput->polling = description->polling;
    newoutput->getnumdrivers = description->getnumdrivers;
    newoutput->getdrivername = description->getdrivername;
    newoutput->getdrivercaps = description->getdrivercaps;
    newoutput->init = description->init;
    newoutput->close = description->close;
    newoutput->start = description->start;
    newoutput->stop = description->stop;
    newoutput->update = description->update;
    newoutput->gethandle = description->gethandle;
    newoutput->getposition = description->getposition;
    newoutput->lock = description->lock;
    newoutput->unlock = description->unlock;
    newoutput->getsamplemaxchannels = description->getsamplemaxchannels;
    newoutput->getdrivercapsex = description->getdrivercapsex;
    newoutput->initex = description->initex;
    newoutput->start = description->start;
    newoutput->stop = description->stop;
    newoutput->updatefinished = description->updatefinished;
    newoutput->createsample = description->createsample;
    newoutput->getsoundram = description->getsoundram;
    newoutput->record_getnumdrivers = description->record_getnumdrivers;
    newoutput->record_getdrivername = description->record_getdrivername;
    newoutput->record_start = description->record_start;
    newoutput->record_stop = description->record_stop;
    newoutput->record_getposition = description->record_getposition;
    newoutput->record_lock = description->record_lock;
    newoutput->record_unlock = description->record_unlock;
    newoutput->reverb_setproperties = description->reverb_setproperties;
    newoutput->mType = description->mType;
    newoutput->mSize = description->mSize;
    newoutput->mModule = description->mModule;

    newoutput->addBefore(&mOutputHead);

    if (plugintype)
    {
        *plugintype = FMOD_PLUGINTYPE_OUTPUT;
    }
    if (index)
    {
        *index = mOutputHead.getNodeIndex(newoutput);
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::getNumCodecs(int * numcodecs)
{
    FMOD_CODEC_DESCRIPTION_EX * head;
    FMOD_CODEC_DESCRIPTION_EX * current;

    if (!numcodecs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numcodecs = 0;

    head = &mCodecHead;
    for (current = (FMOD_CODEC_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_CODEC_DESCRIPTION_EX *)current->getNext())
    {
        (*numcodecs)++;
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::getNumDSPs(int * numdsps)
{
    FMOD_DSP_DESCRIPTION_EX * head;
    FMOD_DSP_DESCRIPTION_EX * current;

    if (!numdsps)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numdsps = 0;

    head = &mDSPHead;
    for (current = (FMOD_DSP_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_DSP_DESCRIPTION_EX *)current->getNext())
    {
        (*numdsps)++;
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::getNumOutputs(int * numoutputs)
{
    FMOD_OUTPUT_DESCRIPTION_EX * head;
    FMOD_OUTPUT_DESCRIPTION_EX * current;

    if (!numoutputs)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *numoutputs = 0;

    head = &mOutputHead;
    for (current = (FMOD_OUTPUT_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_OUTPUT_DESCRIPTION_EX *)current->getNext())
    {
        (*numoutputs)++;
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::getCodec(int index, FMOD_CODEC_DESCRIPTION_EX * * codecdesc)
{
    FMOD_CODEC_DESCRIPTION_EX * head;
    FMOD_CODEC_DESCRIPTION_EX * current;
    int count;

    if (!codecdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    count = 0;
    head = &mCodecHead;
    for (current = (FMOD_CODEC_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_CODEC_DESCRIPTION_EX *)current->getNext())
    {
        if (count == index)
        {
            *codecdesc = current;
            break;
        }
        count++;
    }

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::getDSP(int index, FMOD_DSP_DESCRIPTION_EX * * dspdesc)
{
    FMOD_DSP_DESCRIPTION_EX * head;
    FMOD_DSP_DESCRIPTION_EX * current;
    int count;

    if (!dspdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *dspdesc = 0;
    count = 0;

    head = &mDSPHead;
    for (current = (FMOD_DSP_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_DSP_DESCRIPTION_EX *)current->getNext())
    {
        if (count == index)
        {
            *dspdesc = current;
            return FMOD_OK;
        }
        count++;
    }

    return FMOD_ERR_DSP_NOTFOUND;
}

FMOD_RESULT PluginFactory::getOutput(int index, FMOD_OUTPUT_DESCRIPTION_EX * * outputdesc)
{
    FMOD_OUTPUT_DESCRIPTION_EX * head;
    FMOD_OUTPUT_DESCRIPTION_EX * current;
    int count;

    if (!outputdesc)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    count = 0;
    head = &mOutputHead;
    for (current = (FMOD_OUTPUT_DESCRIPTION_EX *)head->getNext(); current != head; current = (FMOD_OUTPUT_DESCRIPTION_EX *)current->getNext())
    {
        if (count == index)
        {
            *outputdesc = current;
            return FMOD_OK;
        }
        count++;
    }

    return FMOD_ERR_INVALID_PARAM;
}

FMOD_RESULT PluginFactory::createCodec(FMOD_CODEC_DESCRIPTION_EX * codecdesc, Codec * * codec)
{
    Codec * newcodec;

    if (!codecdesc || !codec)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    newcodec = FMOD_Object_CallocSize(Codec, codecdesc->mSize);
    if (!newcodec)
    {
        return FMOD_ERR_MEMORY;
    }

    memcpy(&newcodec->mDescription, codecdesc, sizeof(FMOD_CODEC_DESCRIPTION_EX));

    *codec = newcodec;

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::createDSP(FMOD_DSP_DESCRIPTION_EX * dspdesc, DSPI * * dsp)
{
    FMOD_RESULT result;
    DSPI * newdsp;

    if (!dspdesc || !dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    switch (dspdesc->mCategory)
    {
        case FMOD_DSP_CATEGORY_FILTER:
        {
            newdsp = FMOD_Object_CallocSize(DSPFilter, dspdesc->mSize);
            break;
        }
        case FMOD_DSP_CATEGORY_DSPCODEC:
        {
            newdsp = FMOD_Object_CallocSize(DSPCodec, dspdesc->mSize);
            break;
        }
        case FMOD_DSP_CATEGORY_RESAMPLER:
        {
            newdsp = FMOD_Object_CallocSize(DSPInputResampler, dspdesc->mSize);
            break;
        }
        case FMOD_DSP_CATEGORY_SOUNDCARD:
        {
            newdsp = FMOD_Object_CallocSize(DSPSoundCard, dspdesc->mSize);
            break;
        }
        case FMOD_DSP_CATEGORY_WAVETABLE:
        {
            newdsp = FMOD_Object_CallocSize(DSPWaveTable, dspdesc->mSize);
            break;
        }
        default:
        {
            return FMOD_ERR_INVALID_PARAM;
        }
    }

    if (!newdsp)
    {
        *dsp = 0;
        return FMOD_ERR_MEMORY;
    }

    newdsp->mSystem = mSystem;

    result = newdsp->alloc(dspdesc);
    if (result != FMOD_OK)
    {
        FMOD_Memory_Free(newdsp);
        return result;
    }

    if (dspdesc->create)
    {
        newdsp->instance = (FMOD_DSP *)newdsp;

        result = dspdesc->create((FMOD_DSP_STATE *)newdsp);
        if (result != FMOD_OK)
        {
            FMOD_Memory_Free(newdsp);
            return result;
        }
    }

    *dsp = newdsp;

    return FMOD_OK;
}

FMOD_RESULT PluginFactory::createOutput(FMOD_OUTPUT_DESCRIPTION_EX * outputdesc, Output * * output)
{
    Output * newoutput;

    if (!outputdesc || !output)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (outputdesc->polling)
    {
        newoutput = FMOD_Object_CallocSize(OutputPolled, outputdesc->mSize);
    }
    else
    {
        newoutput = FMOD_Object_CallocSize(Output, outputdesc->mSize);
    }

    if (!newoutput)
    {
        *output = 0;
        return FMOD_ERR_MEMORY;
    }

    memcpy(&newoutput->mDescription, outputdesc, sizeof(FMOD_OUTPUT_DESCRIPTION_EX));
    newoutput->mSystem = mSystem;
    newoutput->readfrommixer = Output::mixCallback;

    *output = newoutput;

    return FMOD_OK;
}

} // namespace FMOD
