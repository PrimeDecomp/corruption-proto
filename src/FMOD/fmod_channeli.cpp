// G2MEAB prototype translation unit; complete reconstruction in native address order.
// .text: 0x805BCCA8..0x805C11FC (55 native functions, including the weak SoundI::isStream copy
// 0x805BE2A8 and the implicit ChannelI destructor 0x805C1164).
// Original basename from the target allocation/free evidence: stopEx 0x805BE878 and
// setSpeakerLevels 0x805BF364 pass "fmod_channeli.cpp" (0x806E2C2C) to the memory pool.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelI and
// FMOD_CHANNEL_INFO are the G2MEAB layouts (fmod_channeli.h).

#include "fmod_channeli.h"
#include "fmod.h"
#include "fmod.hpp"
#include "fmod_channel_real.h"
#include "fmod_channelgroupi.h"
#include "fmod_dspi.h"
#include "fmod_geometry_mgr.h"
#include "fmod_listener.h"
#include "fmod_memory.h"
#include "fmod_msl_math.h"
#include "fmod_sound_sample.h"
#include "fmod_soundi.h"
#include "fmod_syncpoint.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace FMOD {

FMOD_RESULT ChannelI::validate(Channel * channel, ChannelI * * channeli)
{
    unsigned int handle = (unsigned int)channel;
    unsigned int systemid = handle >> SYSTEMID_SHIFT;
    unsigned int index = (handle >> CHANINDEX_SHIFT) & CHANINDEX_MASK;
    unsigned int refcount = handle & REFCOUNT_MASK;
    SystemI * system;
    ChannelI * chan;

    if (!channeli)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *channeli = 0;

    if (SystemI::getInstance(systemid, &system) != FMOD_OK)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!system->mChannel)
    {
        return FMOD_ERR_UNINITIALIZED;
    }

    if ((int)index >= system->mNumChannels)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    chan = &system->mChannel[index];

    if (refcount && chan->mHandleCurrent != handle)
    {
        if ((chan->mHandleCurrent & REFCOUNT_MASK) - refcount >= 2)
        {
            return FMOD_ERR_CHANNEL_STOLEN;
        }

        return FMOD_ERR_INVALID_HANDLE;
    }

    *channeli = chan;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::returnToFreeList()
{
    SystemI * system = mSystem;

    if (!system)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mSortedListNode.removeNode();
    removeNode();
    addAfter(&system->mChannelFreeListHead);

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setDefaults(int priority, float frequency, float volume, float pan, float frequencyvariation, float volumevariation, float panvariation)
{
    float freq;
    float vol;
    float panvalue;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    mPriority = priority;
    freq = frequency;
    vol = volume;
    panvalue = pan;

    if (frequencyvariation > 0.0f)
    {
        freq += frequencyvariation * ((float)(rand() % 32768) / 16384.0f - 1.0f);
    }
    if (volumevariation > 0.0f)
    {
        vol += volumevariation * ((float)(rand() % 32768) / 16384.0f - 1.0f);
    }
    if (panvariation > 0.0f)
    {
        panvalue += panvariation * ((float)(rand() % 32768) / 8192.0f - 2.0f);
    }

    setFrequency(freq);
    setVolume(vol);
    setPan(panvalue, true);

    mDirectOcclusion = 0.0f;
    mReverbOcclusion = 0.0f;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::referenceStamp(bool newstamp)
{
    unsigned int systemid = mHandleCurrent >> SYSTEMID_SHIFT;
    unsigned int index = (mHandleCurrent >> CHANINDEX_SHIFT) & CHANINDEX_MASK;
    unsigned int handle;
    unsigned int refcount;

    if (newstamp)
    {
        handle = mHandleCurrent;
    }
    else
    {
        handle = mHandleOriginal;
    }

    refcount = (handle & REFCOUNT_MASK) + 1;
    if (refcount > REFCOUNT_MASK)
    {
        refcount = 1;
    }

    mHandleCurrent = (systemid << SYSTEMID_SHIFT) | ((index << CHANINDEX_SHIFT) & (CHANINDEX_MASK << CHANINDEX_SHIFT)) | refcount;

    if (newstamp)
    {
        mHandleOriginal = mHandleCurrent;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::updatePosition()
{
    float audibility;
    unsigned int oldposition;

    if (!mSystem)
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    getAudibility(&audibility);

    oldposition = mListPosition;

    mListPosition = mPriority * (FMOD_MAXAUDIBIILITY + 1);
    mListPosition += FMOD_MAXAUDIBIILITY - (int)(audibility * 1000.0f);

    if (mListPosition != oldposition)
    {
        mSortedListNode.removeNode();
        mSortedListNode.addAt(&mSystem->mChannelSortedListHead, mListPosition);
        mSortedListNode.setData(this);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::getChannelInfo(FMOD_CHANNEL_INFO * info)
{
    int count;

    info->mRealChannel = mRealChannel[0];

    if (mLevels)
    {
        for (count = 0; count < mSystem->mMaxOutputChannels; count++)
        {
            getSpeakerLevels(count, &info->mLevels[count * 8], mSystem->mMaxInputChannels);
        }
    }

    getPosition(&info->mPCM, FMOD_TIMEUNIT_PCM);
    getLoopPoints(&info->mLoopStart, FMOD_TIMEUNIT_PCM, &info->mLoopEnd, FMOD_TIMEUNIT_PCM);
    getCurrentSound(&info->mSound);
    getLoopCount(&info->mLoopCount);
    getMute(&info->mMute);
    getPaused(&info->mPaused);
    getDelay(&info->mStartDelay, &info->mEndDelay);
    getReverbProperties(&info->mReverbProperties);

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setChannelInfo(FMOD_CHANNEL_INFO * info)
{
    int count;

    setVolume(mVolume);
    setFrequency(mFrequency);

    bool pan = false;
    bool speakermix = false;
    bool speakerlevels = false;

    if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
    {
        pan = true;
    }
    else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
    {
        speakermix = true;
    }
    else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
    {
        speakerlevels = true;
    }

    setPan(mPan, pan);
    setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR, speakermix);

    if (mLevels)
    {
        for (count = 0; count < mSystem->mMaxOutputChannels; count++)
        {
            setSpeakerLevels(count, &mLevels[count * 2], mSystem->mMaxInputChannels, speakerlevels);
        }
    }

    set3DAttributes(&mPosition3D, &mVelocity3D);
    setDelay(info->mStartDelay, info->mEndDelay);
    setPosition(info->mPCM, FMOD_TIMEUNIT_PCM);
    setLoopCount(info->mLoopCount);
    setLoopPoints(info->mLoopStart, FMOD_TIMEUNIT_PCM, info->mLoopEnd, FMOD_TIMEUNIT_PCM);
    setLoopCount(info->mLoopCount);
    setMute(info->mMute);
    setReverbProperties(&info->mReverbProperties);

    if (mCallback[FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE])
    {
        bool isvirtual;

        isVirtual(&isvirtual);

        mCallback[FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE]((FMOD_CHANNEL *)mHandleCurrent, FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE, mCallbackCommand[FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE], isvirtual ? 1 : 0, 0);
    }

    update(0, true);

    return FMOD_OK;
}

FMOD_RESULT ChannelI::getRealChannel(ChannelReal * * realchan, int * subchannels)
{
    int count;

    if (mRealChannel[0]->isStream())
    {
        if (realchan)
        {
            for (count = 0; count < mRealChannel[0]->mNumRealChannels; count++)
            {
                realchan[count] = mRealChannel[0]->mRealChannel[count];
            }
        }
        if (subchannels)
        {
            *subchannels = mRealChannel[0]->mNumRealChannels;
        }
    }
    else
    {
        if (realchan)
        {
            for (count = 0; count < mNumRealChannels; count++)
            {
                realchan[count] = mRealChannel[count];
            }
        }
        if (subchannels)
        {
            *subchannels = mNumRealChannels;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::calcVolumeAndPitchFor3D()
{
    FMOD_RESULT result;
    float pitch = 1.0f;
    float volume = 1.0f;
    float conevolume = 1.0f;
    int count;

    for (count = 0; count < mNumRealChannels; count++)
    {
        int numlisteners;
        int listenernum;
        float mindistance;

        if (mRealChannel[count]->mMode & FMOD_2D)
        {
            continue;
        }

        result = mSystem->get3DNumListeners(&numlisteners);
        if (result != FMOD_OK)
        {
            return result;
        }

        mindistance = 999999.0f;

        for (listenernum = 0; listenernum < numlisteners; listenernum++)
        {
            Listener * listener;
            float dopplerscale;
            float distancescale;
            float rolloffscale;
            FMOD_VECTOR currdiff;
            float distance;

            result = mSystem->getListenerObject(listenernum, &listener);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (!mMoved && !listener->mMoved)
            {
                return FMOD_OK;
            }

            result = mSystem->get3DSettings(&dopplerscale, &distancescale, &rolloffscale);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (mRealChannel[count]->mMode & FMOD_3D_HEADRELATIVE)
            {
                currdiff.x = mPosition3D.x;
                currdiff.y = mPosition3D.y;
                currdiff.z = mPosition3D.z;
            }
            else
            {
                currdiff.x = mPosition3D.x - listener->mPosition.x;
                currdiff.y = mPosition3D.y - listener->mPosition.y;
                currdiff.z = mPosition3D.z - listener->mPosition.z;
            }

            if (mSystem->mFlags & FMOD_INIT_3D_RIGHTHANDED)
            {
                currdiff.z = -currdiff.z;
            }

            distance = MSL_sqrtf(currdiff.x * currdiff.x + currdiff.y * currdiff.y + currdiff.z * currdiff.z);

            if (distance < mindistance)
            {
                mindistance = distance;
                mDistance = distance;

                if (mRolloffPoints && mNumRolloffPoints && (mRealChannel[count]->mMode & FMOD_3D_CUSTOMROLLOFF))
                {
                    if (distance >= mRolloffPoints[mNumRolloffPoints - 1].x)
                    {
                        volume = mRolloffPoints[mNumRolloffPoints - 1].y;
                    }
                    else
                    {
                        int point;

                        for (point = 1; point < mNumRolloffPoints; point++)
                        {
                            if (distance >= mRolloffPoints[point - 1].x && distance < mRolloffPoints[point].x)
                            {
                                float frac = (distance - mRolloffPoints[point - 1].x) / (mRolloffPoints[point].x - mRolloffPoints[point - 1].x);

                                volume = (1.0f - frac) * mRolloffPoints[point - 1].y + frac * mRolloffPoints[point].y;
                                break;
                            }
                        }
                    }
                }
                else
                {
                    if (distance >= mMaxDistance)
                    {
                        distance = mMaxDistance;
                    }
                    if (distance < mMinDistance)
                    {
                        distance = mMinDistance;
                    }

                    if (mRealChannel[count]->mMode & FMOD_3D_LINEARROLLOFF)
                    {
                        float range = mMaxDistance - mMinDistance;

                        if (range <= 0.0f)
                        {
                            volume = 1.0f;
                        }
                        else
                        {
                            volume = (mMaxDistance - distance) / range;
                        }
                    }
                    else
                    {
                        if (distance > mMinDistance && rolloffscale != 1.0f)
                        {
                            distance = (distance - mMinDistance) * rolloffscale + mMinDistance;
                        }

                        if (distance < 0.000001f)
                        {
                            distance = 0.000001f;
                        }

                        volume = mMinDistance / distance;
                    }
                }

                if (volume < 0.0f)
                {
                    volume = 0.0f;
                }
                if (volume > 1.0f)
                {
                    volume = 1.0f;
                }

                if (mConeOutsideAngle < 360.0f || mConeInsideAngle < 360.0f)
                {
                    float angle;

                    if (distance <= 0.0f)
                    {
                        currdiff.x = 0.0f;
                        currdiff.y = 0.0f;
                        currdiff.z = 0.0f;
                    }
                    else
                    {
                        currdiff.x /= distance;
                        currdiff.y /= distance;
                        currdiff.z /= distance;
                    }

                    angle = currdiff.x * mConeOrientation.x + currdiff.y * mConeOrientation.y + currdiff.z * mConeOrientation.z;
                    angle = angle < -1.0f ? -1.0f : (angle > 1.0f ? 1.0f : angle);
                    angle = 180.0f * (1.0f - angle);

                    if (angle < mConeInsideAngle)
                    {
                        conevolume = 1.0f;
                    }
                    else if (angle < mConeOutsideAngle)
                    {
                        float frac = (angle - mConeInsideAngle) / (mConeOutsideAngle - mConeInsideAngle);

                        conevolume = (1.0f - frac) + mConeOutsideVolume * frac;
                    }
                    else
                    {
                        conevolume = mConeOutsideVolume;
                    }
                }
            }

            if (dopplerscale > 0.0f && numlisteners == 1)
            {
                FMOD_VECTOR lastposition;
                FMOD_VECTOR lastdiff;
                float lastdistance;
                float speedofsound;

                lastposition.x = mPosition3D.x - mVelocity3D.x;
                lastposition.y = mPosition3D.y - mVelocity3D.y;
                lastposition.z = mPosition3D.z - mVelocity3D.z;

                if (mRealChannel[count]->mMode & FMOD_3D_HEADRELATIVE)
                {
                    lastdiff.x = lastposition.x;
                    lastdiff.y = lastposition.y;
                    lastdiff.z = lastposition.z;
                }
                else
                {
                    lastdiff.x = lastposition.x - (listener->mPosition.x - listener->mVelocity.x);
                    lastdiff.y = lastposition.y - (listener->mPosition.y - listener->mVelocity.y);
                    lastdiff.z = lastposition.z - (listener->mPosition.z - listener->mVelocity.z);
                }

                lastdistance = MSL_sqrtf(lastdiff.x * lastdiff.x + lastdiff.y * lastdiff.y + lastdiff.z * lastdiff.z);

                speedofsound = 340.0f * distancescale;
                pitch = speedofsound - dopplerscale * (mDistance - lastdistance);
                pitch /= speedofsound;
            }

            if (pitch < 0.000001f)
            {
                pitch = 0.000001f;
            }
        }
    }

    mVolume3D = volume;
    mConeVolume3D = conevolume;
    mPitch3D = pitch;

    return FMOD_OK;
}

ChannelI::ChannelI()
{
    init();
}

ChannelI::ChannelI(int index, SystemI * system)
{
    init();

    mIndex = index;
    mSystem = system;

    mHandleCurrent = (mSystem->mIndex << SYSTEMID_SHIFT) | ((mIndex << CHANINDEX_SHIFT) & (CHANINDEX_MASK << CHANINDEX_SHIFT)) | 1;
    mHandleOriginal = mHandleCurrent;
}

FMOD_RESULT ChannelI::init()
{
    int count;

    mSystem = 0;
    mHandleCurrent = 0;
    mHandleOriginal = 0;

    for (count = 0; count < FMOD_CHANNEL_CALLBACKTYPE_MAX; count++)
    {
        mCallback[count] = 0;
        mCallbackCommand[count] = 0;
    }

    for (count = 0; count < FMOD_CHANNEL_MAXREALSUBCHANNELS; count++)
    {
        mRealChannel[count] = 0;
    }

    mNumRealChannels = 1;
    mPriority = FMOD_CHANNEL_DEFAULTPRIORITY;
    mListPosition = (unsigned int)-1;
    mIndex = 0;
    mVolume = 1.0f;
    mFrequency = DEFAULT_FREQUENCY;
    mPan = 0.0f;
    mSpeakerFL = 1.0f;
    mSpeakerFR = 1.0f;
    mSpeakerC = 1.0f;
    mSpeakerLFE = 1.0f;
    mSpeakerBL = 1.0f;
    mSpeakerBR = 1.0f;
    mSpeakerSL = 1.0f;
    mSpeakerSR = 1.0f;
    mLevels = 0;
    mVolume3D = 1.0f;
    mPitch3D = 1.0f;
    mConeVolume3D = 1.0f;
    mVolumeOcclusion = 1.0f;
    mPosition3D.x = 0.0f;
    mPosition3D.y = 0.0f;
    mPosition3D.z = 0.0f;
    mVelocity3D.x = 0.0f;
    mVelocity3D.y = 0.0f;
    mVelocity3D.z = 0.0f;
    mMinDistance = 1.0f;
    mMaxDistance = 1000000000.0f;
    mUnk5C = 0;
    mRolloffPoints = 0;
    mNumRolloffPoints = 0;
    mLastPaused = false;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::play(SoundI * sound, bool paused, bool reset)
{
    FMOD_RESULT result;

    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    result = alloc(sound, reset);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = setPaused(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (reset)
    {
        result = setDefaults(sound->mDefaultPriority, sound->mDefaultFrequency, sound->mDefaultVolume, sound->mDefaultPan, sound->mFrequencyVariation, sound->mVolumeVariation, sound->mPanVariation);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = setPosition(0, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    result = start();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (reset)
    {
        FMOD_MODE mode;

        sound->getMode(&mode);

        if (mode & FMOD_3D)
        {
            FMOD_VECTOR velocity = { 0.0f, 0.0f, 0.0f };

            result = set3DAttributes(&mSystem->mListener[0].mPosition, &velocity);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = update(0, true);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (!paused)
    {
        result = setPaused(paused);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::alloc(SoundI * sound, bool reset)
{
    FMOD_RESULT result;
    int count;

    if (reset)
    {
        mSyncPointLastPos = 0;

        if (mSystem)
        {
            mChannelGroup = mSystem->mChannelGroup;
        }

        mMute = false;
        mMoved = false;
        mVolume3D = 1.0f;
        mConeVolume3D = 1.0f;
        mPitch3D = 1.0f;
        mVolumeOcclusion = 1.0f;
        mMinDistance = sound->mMinDistance;
        mMaxDistance = sound->mMaxDistance;
        mDistance = 0.0f;
        mConeInsideAngle = sound->mConeInsideAngle;
        mConeOutsideAngle = sound->mConeOutsideAngle;
        mConeOutsideVolume = sound->mConeOutsideVolume;
        mConeOrientation.x = 0.0f;
        mConeOrientation.y = 0.0f;
        mConeOrientation.z = 1.0f;
        mRolloffPoints = sound->mRolloffPoint;
        mNumRolloffPoints = sound->mNumRolloffPoints;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (!mRealChannel[count])
        {
            return FMOD_ERR_INVALID_HANDLE;
        }

        if (sound->isStream())
        {
            mRealChannel[count]->mSound = sound;
        }
        else
        {
            mRealChannel[count]->mSound = mNumRealChannels > 1 ? ((Sample *)sound)->mSubSample[count] : sound;
        }

        mRealChannel[count]->mSubChannelIndex = count;
        mRealChannel[count]->mDSP = 0;
        mRealChannel[count]->mMode = sound->mMode;
        mRealChannel[count]->mLoopStart = sound->mLoopStart;
        mRealChannel[count]->mLoopLength = sound->mLoopLength;
        mRealChannel[count]->mLoopCount = sound->mLoopCount;
        mRealChannel[count]->mLength = sound->mLength;
        mRealChannel[count]->mStartDelay = 0;
        mRealChannel[count]->mEndDelay = 0;
        mRealChannel[count]->mParent = this;

        result = mRealChannel[count]->alloc();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::start()
{
    FMOD_RESULT result;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->start();
        if (result != FMOD_OK)
        {
            return result;
        }

        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_STOPPED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;
        mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_PLAYING;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::update(int delta, bool callrealupdate)
{
    FMOD_RESULT result;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        if ((mRealChannel[count]->mMode & FMOD_3D) && delta > 0 && (mMoved || mSystem->mListener[0].mMoved || mSystem->mGeometryMgr.mMoved) && mSystem->mGeometryList)
        {
            float occlusiondelta = 0.002f * (float)delta;
            float directocclusion = 0.0f;
            float reverbocclusion = 0.0f;

            if (mRealChannel[count]->mMode & FMOD_3D_HEADRELATIVE)
            {
                FMOD_VECTOR position;

                position.x = mPosition3D.x + mSystem->mListener[0].mPosition.x;
                position.y = mPosition3D.y + mSystem->mListener[0].mPosition.y;
                position.z = mPosition3D.z + mSystem->mListener[0].mPosition.z;

                mSystem->mGeometryMgr.lineTestAll(&mSystem->mListener[0].mPosition, &position, &directocclusion, &reverbocclusion);
            }
            else
            {
                mSystem->mGeometryMgr.lineTestAll(&mSystem->mListener[0].mPosition, &mPosition3D, &directocclusion, &reverbocclusion);
            }

            if (directocclusion != mDirectOcclusion || reverbocclusion != mReverbOcclusion)
            {
                if (mDirectOcclusion < directocclusion)
                {
                    mDirectOcclusion += occlusiondelta;
                    if (mDirectOcclusion > directocclusion)
                    {
                        mDirectOcclusion = directocclusion;
                    }
                }
                else if (mDirectOcclusion > directocclusion)
                {
                    mDirectOcclusion -= occlusiondelta;
                    if (mDirectOcclusion < directocclusion)
                    {
                        mDirectOcclusion = directocclusion;
                    }
                }

                if (mReverbOcclusion < reverbocclusion)
                {
                    mReverbOcclusion += occlusiondelta;
                    if (mReverbOcclusion > reverbocclusion)
                    {
                        mReverbOcclusion = reverbocclusion;
                    }
                }
                else if (mReverbOcclusion > reverbocclusion)
                {
                    mReverbOcclusion -= occlusiondelta;
                    if (mReverbOcclusion < reverbocclusion)
                    {
                        mReverbOcclusion = reverbocclusion;
                    }
                }

                set3DOcclusion(mDirectOcclusion, mReverbOcclusion);
            }
        }
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (mRealChannel[count]->mEndDelay && (mRealChannel[count]->mFlags & CHANNELREAL_FLAG_ENDDELAY))
        {
            if (mRealChannel[count]->mEndDelay <= (unsigned int)delta)
            {
                mRealChannel[count]->mEndDelay = 0;
                mRealChannel[count]->stop(true, true);
            }
            else
            {
                mRealChannel[count]->mEndDelay -= delta;
            }
        }
    }

    result = calcVolumeAndPitchFor3D();
    if (result != FMOD_OK)
    {
        return result;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->set2DFreqVolumePanFor3D();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (mRealChannel[0]->mSound)
    {
        SoundI * soundparent = mRealChannel[0]->mSound->mSubSampleParent;

        if (mCallback[FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT] && soundparent->mNumSyncPoints)
        {
            unsigned int position;

            if (getPosition(&position, FMOD_TIMEUNIT_PCM) == FMOD_OK)
            {
                SyncPoint * point;
                int index;

                if (mFrequency > 0.0f)
                {
                    index = 0;
                    point = (SyncPoint *)soundparent->mSyncPointHead.getNext();
                }
                else
                {
                    index = soundparent->mNumSyncPoints - 1;
                    point = (SyncPoint *)soundparent->mSyncPointHead.getPrev();
                }

                while ((mFrequency > 0.0f && point->mOffset < position) || (mFrequency < 0.0f && point->mOffset > position))
                {
                    if ((point->mOffset > mSyncPointLastPos && point->mOffset <= position) || (point->mOffset < mSyncPointLastPos && point->mOffset >= position))
                    {
                        mCallback[FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT]((FMOD_CHANNEL *)mHandleCurrent, FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT, mCallbackCommand[FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT], index, 0);
                        break;
                    }

                    if (mFrequency > 0.0f)
                    {
                        index++;
                        point = (SyncPoint *)point->getNext();
                        if (index >= soundparent->mNumSyncPoints)
                        {
                            break;
                        }
                    }
                    else
                    {
                        index--;
                        point = (SyncPoint *)point->getPrev();
                        if (index < 0)
                        {
                            break;
                        }
                    }
                }

                mSyncPointLastPos = position;
            }
        }
    }

    if (callrealupdate)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            result = mRealChannel[count]->update(delta);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    if (mMoved || mSystem->mListener[0].mMoved)
    {
        result = updatePosition();
        if (result != FMOD_OK)
        {
            return result;
        }

        mMoved = false;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::stop()
{
    return stopEx(true, true, true, true, false);
}

FMOD_RESULT ChannelI::stopEx(bool refstamp, bool updatelist, bool resetcallbacks, bool updateflags, bool callendcallback)
{
    FMOD_RESULT result;
    ChannelReal * realchannel[FMOD_CHANNEL_MAXREALSUBCHANNELS];
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_STOPPED)
    {
        return FMOD_OK;
    }

    if (updateflags)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_RESERVED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PLAYING;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_UNK200;
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;
            mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_STOPPED;
        }
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->stop(true, updateflags);
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    if (updatelist)
    {
        result = returnToFreeList();
        if (result != FMOD_OK)
        {
            return result;
        }

        mListPosition = (unsigned int)-1;
    }

    mJustWentVirtual = false;

    for (count = 0; count < mNumRealChannels; count++)
    {
        realchannel[count] = mRealChannel[count];
    }

    if (callendcallback && mCallback[FMOD_CHANNEL_CALLBACKTYPE_END])
    {
        mCallback[FMOD_CHANNEL_CALLBACKTYPE_END]((FMOD_CHANNEL *)mHandleCurrent, FMOD_CHANNEL_CALLBACKTYPE_END, mCallbackCommand[FMOD_CHANNEL_CALLBACKTYPE_END], 0, 0);
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        if (!(mRealChannel[count]->mFlags & CHANNELREAL_FLAG_PLAYING) || realchannel[count] != mRealChannel[count])
        {
            realchannel[count]->mSound = 0;
            realchannel[count]->mDSP = 0;
            realchannel[count]->mParent = 0;
        }
    }

    if (mLevels)
    {
        FMOD_Memory_Free(mLevels);
        mLevels = 0;
    }

    if (mUnk5C)
    {
        FMOD_Memory_Free(mUnk5C);
        mUnk5C = 0;
    }

    if (mListPosition == (unsigned int)-1)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            mRealChannel[count] = 0;
        }

        if (resetcallbacks)
        {
            for (count = 0; count < FMOD_CHANNEL_CALLBACKTYPE_MAX; count++)
            {
                mCallback[count] = 0;
                mCallbackCommand[count] = 0;
            }
        }

        if (refstamp)
        {
            result = referenceStamp(false);
            if (result != FMOD_OK)
            {
                return result;
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setPaused(bool paused)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    getPaused(&mLastPaused);

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setPaused(paused);
        if (result == FMOD_OK)
        {
            result = result2;
        }

        if (paused)
        {
            mRealChannel[count]->mFlags |= CHANNELREAL_FLAG_PAUSED;
        }
        else
        {
            mRealChannel[count]->mFlags &= ~CHANNELREAL_FLAG_PAUSED;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getPaused(bool * paused)
{
    if (!paused)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *paused = (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_PAUSED) ? true : false;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setVolume(float volume)
{
    FMOD_RESULT result = FMOD_OK;
    int count;
    bool changed = false;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (volume < 0.0f)
    {
        volume = 0.0f;
    }
    if (volume > 1.0f)
    {
        volume = 1.0f;
    }

    if (mVolume != volume)
    {
        changed = true;
    }

    mVolume = volume;

    if (mMute)
    {
        volume = 0.0f;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        result = mRealChannel[count]->setVolume(volume);
    }

    if (changed)
    {
        result = updatePosition();
        if (result != FMOD_OK)
        {
            return result;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getVolume(float * volume)
{
    if (!volume)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *volume = mVolume;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setFrequency(float frequency)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (frequency < mRealChannel[0]->mMinFrequency)
    {
        frequency = mRealChannel[0]->mMinFrequency;
    }
    if (frequency > mRealChannel[0]->mMaxFrequency)
    {
        frequency = mRealChannel[0]->mMaxFrequency;
    }

    mFrequency = frequency;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setFrequency(mFrequency);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::setPan(float pan, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_ERR_NEEDS2D;
    }

    if (pan < -1.0f)
    {
        pan = -1.0f;
    }
    if (pan > 1.0f)
    {
        pan = 1.0f;
    }

    mPan = pan;
    mLastPanMode = FMOD_CHANNEL_PANMODE_PAN;

    if (calldriver)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2;

            if (mNumRealChannels == 2)
            {
                if (count == 0)
                {
                    pan = -1.0f;
                }
                else
                {
                    pan = 1.0f;
                }
            }

            result2 = mRealChannel[count]->setPan(pan, 1.0f);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
}

FMOD_RESULT ChannelI::setDelay(unsigned int startdelay, unsigned int enddelay)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setDelay(startdelay, enddelay);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getDelay(unsigned int * startdelay, unsigned int * enddelay)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (startdelay)
    {
        *startdelay = mRealChannel[0]->mStartDelay;
    }
    if (enddelay)
    {
        *enddelay = mRealChannel[0]->mEndDelay;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        return FMOD_ERR_NEEDS2D;
    }

    mSpeakerFL = frontleft < 0.0f ? 0.0f : (frontleft > 1.0f ? 1.0f : frontleft);
    mSpeakerFR = frontright < 0.0f ? 0.0f : (frontright > 1.0f ? 1.0f : frontright);
    mSpeakerC = center < 0.0f ? 0.0f : (center > 1.0f ? 1.0f : center);
    mSpeakerLFE = lfe < 0.0f ? 0.0f : (lfe > 1.0f ? 1.0f : lfe);
    mSpeakerBL = backleft < 0.0f ? 0.0f : (backleft > 1.0f ? 1.0f : backleft);
    mSpeakerBR = backright < 0.0f ? 0.0f : (backright > 1.0f ? 1.0f : backright);
    mSpeakerSL = sideleft < 0.0f ? 0.0f : (sideleft > 1.0f ? 1.0f : sideleft);
    mSpeakerSR = sideright < 0.0f ? 0.0f : (sideright > 1.0f ? 1.0f : sideright);

    mLastPanMode = FMOD_CHANNEL_PANMODE_SPEAKERMIX;

    if (calldriver)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2 = mRealChannel[count]->setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
}

FMOD_RESULT ChannelI::setSpeakerLevels(int speaker, float * levels, int numlevels, bool calldriver)
{
    FMOD_RESULT result = FMOD_OK;
    float clevels[8];
    int count;

    if (!levels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (numlevels > mSystem->mMaxInputChannels)
    {
        return FMOD_ERR_TOOMANYCHANNELS;
    }

    switch (mSystem->mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_MONO:
        {
            if (speaker != FMOD_SPEAKER_MONO)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT && speaker != FMOD_SPEAKER_BACK_LEFT && speaker != FMOD_SPEAKER_BACK_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }

            // Quad stores the back pair in the third and fourth level slots.
            if (speaker == FMOD_SPEAKER_BACK_LEFT)
            {
                speaker = 2;
            }
            if (speaker == FMOD_SPEAKER_BACK_RIGHT)
            {
                speaker = 3;
            }
            break;
        }
        case FMOD_SPEAKERMODE_SURROUND:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT && speaker != FMOD_SPEAKER_FRONT_CENTER && speaker != FMOD_SPEAKER_BACK_CENTER)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_5POINT1:
        {
            if (speaker != FMOD_SPEAKER_FRONT_LEFT && speaker != FMOD_SPEAKER_FRONT_RIGHT && speaker != FMOD_SPEAKER_BACK_LEFT && speaker != FMOD_SPEAKER_BACK_RIGHT && speaker != FMOD_SPEAKER_FRONT_CENTER && speaker != FMOD_SPEAKER_LOW_FREQUENCY)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        case FMOD_SPEAKERMODE_7POINT1:
        {
            if (speaker > FMOD_SPEAKER_SIDE_RIGHT)
            {
                return FMOD_ERR_INVALID_SPEAKER;
            }
            break;
        }
        default:
        {
            break;
        }
    }

    memset(clevels, 0, sizeof(clevels));

    if (!mLevels)
    {
        mLevels = (float *)FMOD_Memory_Calloc(sizeof(float) * 16);
        if (!mLevels)
        {
            return FMOD_ERR_MEMORY;
        }
    }

    for (count = 0; count < numlevels; count++)
    {
        float level = levels[count];

        if (level < 0.0f)
        {
            level = 0.0f;
        }
        if (level > 1.0f)
        {
            level = 1.0f;
        }

        mLevels[speaker * 2 + count] = level;
        clevels[count] = level;
    }

    mLastPanMode = FMOD_CHANNEL_PANMODE_SPEAKERLEVELS;

    if (calldriver)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2 = mRealChannel[count]->setSpeakerLevels(speaker, clevels, numlevels);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getSpeakerLevels(int speaker, float * levels, int numlevels)
{
    int count;

    if (!levels || !numlevels)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mLevels)
    {
        for (count = 0; count < numlevels; count++)
        {
            levels[count] = mLevels[speaker * 2 + count];
        }
    }
    else
    {
        for (count = 0; count < numlevels; count++)
        {
            levels[count] = 0.0f;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setMute(bool mute)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    mMute = mute;

    if (mute)
    {
        for (count = 0; count < mNumRealChannels; count++)
        {
            FMOD_RESULT result2 = mRealChannel[count]->setVolume(0.0f);
            if (result == FMOD_OK)
            {
                result = result2;
            }
        }
    }
    else
    {
        result = setVolume(mVolume);
    }

    return result;
}

FMOD_RESULT ChannelI::getMute(bool * mute)
{
    if (!mute)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *mute = mMute;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::set3DAttributes(const FMOD_VECTOR * pos, const FMOD_VECTOR * vel)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (pos)
    {
        if (mPosition3D.x != pos->x || mPosition3D.y != pos->y || mPosition3D.z != pos->z)
        {
            mMoved = true;
        }

        mPosition3D.x = pos->x;
        mPosition3D.y = pos->y;
        mPosition3D.z = pos->z;
    }

    if (vel)
    {
        if (mVelocity3D.x != vel->x || mVelocity3D.y != vel->y || mVelocity3D.z != vel->z)
        {
            mMoved = true;
        }

        mVelocity3D.x = vel->x;
        mVelocity3D.y = vel->y;
        mVelocity3D.z = vel->z;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_OK;
    }

    if (mRealChannel[0]->mFlags & CHANNELREAL_FLAG_PAUSED)
    {
        update(0, true);
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->set3DAttributes();
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::get3DAttributes(FMOD_VECTOR * pos, FMOD_VECTOR * vel)
{
    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (pos)
    {
        pos->x = mPosition3D.x;
        pos->y = mPosition3D.y;
        pos->z = mPosition3D.z;
    }

    if (vel)
    {
        vel->x = mVelocity3D.x;
        vel->y = mVelocity3D.y;
        vel->z = mVelocity3D.z;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::set3DMinMaxDistance(float mindistance, float maxdistance)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (mindistance < 0.0f || maxdistance < 0.0f || maxdistance < mindistance)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mMinDistance = mindistance;
    mMaxDistance = maxdistance;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->set3DMinMaxDistance();
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::set3DCustomRolloff(FMOD_VECTOR * points, int numpoints)
{
    int count;

    if (numpoints < 0)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (points)
    {
        for (count = 1; count < numpoints; count++)
        {
            if (points[count].x <= points[count - 1].x)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
            if (points[count].y < 0.0f || points[count].y > 1.0f)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
        }
    }

    mRolloffPoints = points;
    mNumRolloffPoints = numpoints;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::set3DOcclusion(float direct, float reverb)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (!(mRealChannel[0]->mMode & FMOD_3D))
    {
        return FMOD_ERR_NEEDS3D;
    }

    if (direct < 0.0f)
    {
        direct = 0.0f;
    }
    if (reverb < 0.0f)
    {
        reverb = 0.0f;
    }
    if (direct > 1.0f)
    {
        direct = 1.0f;
    }
    if (reverb > 1.0f)
    {
        reverb = 1.0f;
    }

    mDirectOcclusion = direct;
    mReverbOcclusion = reverb;
    mVolumeOcclusion = 1.0f - mDirectOcclusion;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->set3DOcclusion(direct, reverb);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    result = updatePosition();
    if (result != FMOD_OK)
    {
        return result;
    }

    return result;
}

FMOD_RESULT ChannelI::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setReverbProperties(prop);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->getReverbProperties(prop);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::setChannelGroup(ChannelGroupI * channelgroup)
{
    FMOD_RESULT result;
    DSPI * dsphead;
    int numoutputs;
    float levels[2][8];
    int count;

    if (mChannelGroup)
    {
        mChannelGroup->mNumChannels--;
        mChannelGroupNode.removeNode();
    }

    mChannelGroup = channelgroup;
    mChannelGroupNode.addAfter(&mChannelGroup->mChannelHead);
    mChannelGroupNode.setData(this);
    mChannelGroup->mNumChannels++;

    if (getDSPHead(&dsphead) == FMOD_OK)
    {
        result = dsphead->getNumOutputs(&numoutputs);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
        {
            for (count = 0; count < 2; count++)
            {
                getSpeakerLevels(count, levels[count], 8);
            }
        }

        for (count = 0; count < numoutputs; count++)
        {
            DSPI * output;

            result = dsphead->getOutput(count, &output);
            if (result != FMOD_OK)
            {
                return result;
            }

            result = output->disconnectFrom(dsphead);
            if (result != FMOD_OK)
            {
                return result;
            }
        }

        result = channelgroup->mDSPHead->addInput(dsphead);
        if (result != FMOD_OK)
        {
            return result;
        }

        setVolume(mVolume);

        if (mLastPanMode == FMOD_CHANNEL_PANMODE_PAN)
        {
            setPan(mPan, true);
        }
        else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERMIX)
        {
            setSpeakerMix(mSpeakerFL, mSpeakerFR, mSpeakerC, mSpeakerLFE, mSpeakerBL, mSpeakerBR, mSpeakerSL, mSpeakerSR, true);
        }
        else if (mLastPanMode == FMOD_CHANNEL_PANMODE_SPEAKERLEVELS)
        {
            for (count = 0; count < 2; count++)
            {
                setSpeakerLevels(count, levels[count], 8, true);
            }
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::isPlaying(bool * isplaying)
{
    FMOD_RESULT result;
    int count;

    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isplaying = false;

    result = validateInternal();
    if (result != FMOD_OK)
    {
        return result;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        bool playing;

        // Polls the first real channel on every pass (0x805C0188 never advances the slot).
        result = mRealChannel[0]->isPlaying(&playing);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (playing)
        {
            *isplaying = true;
            break;
        }
    }

    // Tests the pointer, not the value: a stopped channel is never moved to the list head.
    if (!isplaying)
    {
        mListPosition = (unsigned int)-1;
        mSortedListNode.removeNode();
        mSortedListNode.addBefore(&mSystem->mChannelSortedListHead);
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::isVirtual(bool * isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *isvirtual = false;
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->isVirtual(isvirtual);
}

FMOD_RESULT ChannelI::getAudibility(float * audibility)
{
    if (!audibility)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mMode & FMOD_3D)
    {
        *audibility = mVolume * mVolume3D * mConeVolume3D * mVolumeOcclusion * mChannelGroup->mRealVolume;
    }
    else
    {
        *audibility = mVolume * mChannelGroup->mRealVolume;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::getCurrentSound(SoundI * * sound)
{
    if (!sound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *sound = 0;
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mSound)
    {
        *sound = mRealChannel[0]->mSound->mSubSampleParent;
    }
    else
    {
        *sound = 0;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::getCurrentDSP(DSPI * * dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        *dsp = 0;
        return FMOD_ERR_INVALID_HANDLE;
    }

    *dsp = mRealChannel[0]->mDSP;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if (mRealChannel[0]->mSound)
    {
        SoundI * soundparent = mRealChannel[0]->mSound->mSubSampleParent;
        unsigned int length;

        if (postype == FMOD_TIMEUNIT_SENTENCE_MS || postype == FMOD_TIMEUNIT_SENTENCE_PCM || postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
        {
            unsigned int subsound;
            unsigned int sentence;

            if (!soundparent->mSubSoundList)
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            result = getPosition(&subsound, FMOD_TIMEUNIT_SENTENCE);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
            {
                postype = FMOD_TIMEUNIT_MS;
            }
            else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
            {
                postype = FMOD_TIMEUNIT_PCM;
            }
            else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
            {
                postype = FMOD_TIMEUNIT_PCMBYTES;
            }

            result = soundparent->mSubSound[subsound]->getLength(&length, postype);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (position >= length)
            {
                return FMOD_ERR_INVALID_PARAM;
            }

            for (sentence = 0; sentence < subsound; sentence++)
            {
                soundparent->mSubSound[soundparent->mSubSoundList[sentence]]->getLength(&length, postype);
                position += length;
            }
        }
        else
        {
            result = soundparent->getLength(&length, postype);
            if (result != FMOD_OK)
            {
                return result;
            }

            if (position >= length)
            {
                return FMOD_ERR_INVALID_PARAM;
            }
        }
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setPosition(position, postype);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->getPosition(position, postype);
}

FMOD_RESULT ChannelI::getDSPHead(DSPI * * dsp)
{
    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    return mRealChannel[0]->getDSPHead(dsp);
}

FMOD_RESULT ChannelI::addDSP(DSPI * dsp)
{
    FMOD_RESULT result;
    DSPI * dsphead = 0;
    DSPI * dspinput = 0;
    int numinputs;
    int dspnuminputs;

    if (!dsp)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    result = getDSPHead(&dsphead);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->getNumInputs(&numinputs);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (numinputs > 1)
    {
        return FMOD_ERR_DSP_TOOMANYCONNECTIONS;
    }

    if (numinputs == 1)
    {
        result = dsp->disconnectFrom(0);
        if (result != FMOD_OK)
        {
            return result;
        }

        result = dsp->getNumInputs(&dspnuminputs);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (dspnuminputs > 0)
        {
            return FMOD_ERR_DSP_CONNECTION;
        }
    }

    result = dsphead->getInput(0, &dspinput);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->disconnectFrom(dspinput);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsphead->addInput(dsp);
    if (result != FMOD_OK)
    {
        return result;
    }

    result = dsp->addInput(dspinput);
    if (result != FMOD_OK)
    {
        return result;
    }

    dsp->reset();

    result = dsp->setActive(true);
    if (result != FMOD_OK)
    {
        return result;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelI::getMode(FMOD_MODE * mode)
{
    if (!mode)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *mode = mRealChannel[0]->mMode;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setLoopCount(int loopcount)
{
    FMOD_RESULT result = FMOD_OK;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setLoopCount(loopcount);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getLoopCount(int * loopcount)
{
    if (!loopcount)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    *loopcount = mRealChannel[0]->mLoopCount;

    return FMOD_OK;
}

FMOD_RESULT ChannelI::setLoopPoints(unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype)
{
    FMOD_RESULT result = FMOD_OK;
    SoundI * soundparent;
    unsigned int loopstartpcm = 0;
    unsigned int loopendpcm = 0;
    unsigned int looplength;
    int count;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) ||
        (loopendtype != FMOD_TIMEUNIT_MS && loopendtype != FMOD_TIMEUNIT_PCM && loopendtype != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (!mRealChannel[0]->mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    soundparent = mRealChannel[0]->mSound->mSubSampleParent;

    if (loopstarttype == FMOD_TIMEUNIT_PCM)
    {
        loopstartpcm = loopstart;
    }
    else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopstart, &loopstartpcm, soundparent->mChannels, soundparent->mFormat);
    }
    else if (loopstarttype == FMOD_TIMEUNIT_MS)
    {
        loopstartpcm = (unsigned int)((float)loopstart / 1000.0f * soundparent->mDefaultFrequency);
    }

    if (loopendtype == FMOD_TIMEUNIT_PCM)
    {
        loopendpcm = loopend;
    }
    else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
    {
        SoundI::getSamplesFromBytes(loopend, &loopendpcm, soundparent->mChannels, soundparent->mFormat);
    }
    else if (loopendtype == FMOD_TIMEUNIT_MS)
    {
        loopendpcm = (unsigned int)((float)loopend / 1000.0f * soundparent->mDefaultFrequency);
    }

    if (loopstartpcm >= loopendpcm)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    looplength = loopendpcm - loopstartpcm + 1;

    for (count = 0; count < mNumRealChannels; count++)
    {
        FMOD_RESULT result2 = mRealChannel[count]->setLoopPoints(loopstartpcm, looplength);
        if (result == FMOD_OK)
        {
            result = result2;
        }
    }

    return result;
}

FMOD_RESULT ChannelI::getLoopPoints(unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype)
{
    SoundI * soundparent;

    if (!mRealChannel[0])
    {
        return FMOD_ERR_INVALID_HANDLE;
    }

    if ((loopstarttype != FMOD_TIMEUNIT_MS && loopstarttype != FMOD_TIMEUNIT_PCM && loopstarttype != FMOD_TIMEUNIT_PCMBYTES) ||
        (loopendtype != FMOD_TIMEUNIT_MS && loopendtype != FMOD_TIMEUNIT_PCM && loopendtype != FMOD_TIMEUNIT_PCMBYTES))
    {
        return FMOD_ERR_FORMAT;
    }

    if (!mRealChannel[0]->mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    soundparent = mRealChannel[0]->mSound->mSubSampleParent;

    if (loopstart)
    {
        if (loopstarttype == FMOD_TIMEUNIT_PCM)
        {
            *loopstart = mRealChannel[0]->mLoopStart;
        }
        else if (loopstarttype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(mRealChannel[0]->mLoopStart, loopstart, soundparent->mChannels, soundparent->mFormat);
        }
        else if (loopstarttype == FMOD_TIMEUNIT_MS)
        {
            *loopstart = (unsigned int)(1000.0f * (float)mRealChannel[0]->mLoopStart / soundparent->mDefaultFrequency);
        }
    }

    if (loopend)
    {
        unsigned int loopendpcm = mRealChannel[0]->mLoopStart + mRealChannel[0]->mLoopLength - 1;

        if (loopendtype == FMOD_TIMEUNIT_PCM)
        {
            *loopend = loopendpcm;
        }
        else if (loopendtype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(loopendpcm, loopend, soundparent->mChannels, soundparent->mFormat);
        }
        else if (loopendtype == FMOD_TIMEUNIT_MS)
        {
            *loopend = (unsigned int)(1000.0f * (float)loopendpcm / soundparent->mDefaultFrequency);
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
