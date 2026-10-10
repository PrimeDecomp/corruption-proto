// G2MEAB prototype translation unit; complete reconstruction of all 37 native functions: every
// ChannelReal method, the static FMOD_Cart2Angle helper 805B7998 (from fmod_3d.h) and
// ChannelRealManual3D::set2DFreqVolumePanFor3D 805B7AB4 (remaining diffs are register allocation).
// G2MEAB .text: 0x805B6E70..0x805B8A3C (37 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Base ctor805B6E70 initializes the common0x78-byte voice layout and vtable806E270C,
// called by emulated805B6C3C, DSP805B8A3C, aggregate805BAB64 and platform80621870 constructors.
// Full vtable slot inventory binds retained setters/getters/stubs/conversion methods through
// mode805B77CC. Integer-angle helper805B7998 is called only by large spatial update805B7AB4;
// vtable806E27A0 replaces base slot+8 with that update and otherwise shares base slots, and the
// following DSP ctor installs that intermediate table before its own table. Keep both spatial
// helper/update in this coherent common/real voice family; a separate original spatial TU remains
// possible, so basename and historical extent are inferred. Next805B8A3C introduces DSP-specific
// fields/vtable and node creation, establishing a concrete implementation transition. Preserve
// every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty
// are recorded externally.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelReal uses the verified G2MEAB layout (0x78, vptr +0x74).

#include "fmod_channel_real.h"
#include "fmod_3d.h"
#include "fmod_channel_realmanual3d.h"
#include "fmod_channeli.h"
#include "fmod_channelpool.h"
#include "fmod_msl_math.h"
#include "fmod_soundi.h"
#include "fmod.h"
#include "fmod_listener.h"
#include "fmod_systemi.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

namespace FMOD {

ChannelReal::ChannelReal()
{
    int count;

    for (count = 0; count < FMOD_CHANNEL_MAXREALSUBCHANNELS; count++)
    {
        mRealChannel[count] = 0;
    }

    mSound = 0;
    mSystem = 0;
    mOutput = 0;
    mPool = 0;
    mLoopCount = -1;
    mMinFrequency = 100.0f;
    mMaxFrequency = 1000000.0f;
    mNumRealChannels = 1;
}

FMOD_RESULT ChannelReal::init(int index, SystemI * system, Output * output, DSPI * dspmixtarget)
{
    mSound = 0;
    mFlags = 0;
    mMode = 0;
    mPosition = 0;
    mDirection = 0;
    mLoopCount = -1;
    mStartDelay = 0;
    mEndDelay = 0;
    mOutput = output;
    mSystem = system;
    mIndex = index;
    mNumRealChannels = 1;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::close()
{
    return stop(true, true);
}

FMOD_RESULT ChannelReal::alloc()
{
    mPosition = 0;

    if (mPool)
    {
        mPool->mChannelsUsed++;
    }
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::alloc(DSPI * dsp)
{
    if (mPool)
    {
        mPool->mChannelsUsed++;
    }

    mPosition = 0;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set2DFreqVolumePanFor3D()
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::update(int delta)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::updateStream()
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::start()
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::stop(bool force, bool updateflags)
{
    if (!force && mEndDelay)
    {
        mFlags |= CHANNELREAL_FLAG_ENDDELAY;
        return FMOD_OK;
    }

    if (mPool)
    {
        mPool->mChannelsUsed--;
    }

    if (updateflags)
    {
        mFlags &= ~CHANNELREAL_FLAG_ALLOCATED;
        mFlags &= ~CHANNELREAL_FLAG_PLAYING;
        mFlags &= ~CHANNELREAL_FLAG_PAUSED;
        mFlags &= ~CHANNELREAL_FLAG_UNK200;
        mFlags &= ~CHANNELREAL_FLAG_ENDDELAY;
        mFlags |= CHANNELREAL_FLAG_STOPPED;
    }

    mStartDelay = 0;
    mEndDelay = 0;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setPaused(bool paused)
{
    if (paused)
    {
        mFlags |= CHANNELREAL_FLAG_PAUSED;
    }
    else
    {
        mFlags &= ~CHANNELREAL_FLAG_PAUSED;
    }
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::getPaused(bool * paused)
{
    if (!paused)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *paused = (mFlags & CHANNELREAL_FLAG_PAUSED) ? true : false;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setVolume(float volume)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setFrequency(float frequency)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setPan(float pan, float fbpan)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setDelay(unsigned int startdelay, unsigned int enddelay)
{
    mStartDelay = startdelay;
    mEndDelay = enddelay;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setSpeakerMix(float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setSpeakerLevels(int speaker, float * levels, int numlevels)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setPosition(unsigned int position, FMOD_TIMEUNIT postype)
{
    FMOD_RESULT result;
    unsigned int pcm;
    unsigned int lengthpcm;

    if (postype != FMOD_TIMEUNIT_MS && postype != FMOD_TIMEUNIT_PCM && postype != FMOD_TIMEUNIT_PCMBYTES)
    {
        return FMOD_ERR_FORMAT;
    }

    if (mSound)
    {
        result = mSound->getLength(&lengthpcm, FMOD_TIMEUNIT_PCM);
        if (result != FMOD_OK)
        {
            return result;
        }

        if (postype == FMOD_TIMEUNIT_PCM)
        {
            pcm = position;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getSamplesFromBytes(position, &pcm, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            pcm = (unsigned int)((float)position / 1000.0f * mSound->mDefaultFrequency);
        }

        if (pcm >= lengthpcm)
        {
            pcm = lengthpcm;
        }

        mPosition = pcm;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelReal::getPosition(unsigned int * position, FMOD_TIMEUNIT postype)
{
    bool getsubsoundtime = false;

    if (!position)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    postype &= ~FMOD_TIMEUNIT_BUFFERED;

    if (postype == FMOD_TIMEUNIT_SENTENCE_MS)
    {
        postype = FMOD_TIMEUNIT_MS;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCM)
    {
        postype = FMOD_TIMEUNIT_PCM;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE_PCMBYTES)
    {
        postype = FMOD_TIMEUNIT_PCMBYTES;
        getsubsoundtime = true;
    }
    else if (postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        getsubsoundtime = true;
    }

    if (getsubsoundtime && !mSound->mSubSoundList)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (postype == FMOD_TIMEUNIT_MS || postype == FMOD_TIMEUNIT_PCM || postype == FMOD_TIMEUNIT_PCMBYTES ||
        postype == FMOD_TIMEUNIT_SENTENCE || postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
    {
        int currentsubsoundid = 0;
        int currentsentenceid = 0;
        unsigned int pcmcurrent = mPosition;

        if (getsubsoundtime)
        {
            for (currentsubsoundid = 0; currentsubsoundid < mSound->mSubSoundListNum; currentsubsoundid++)
            {
                SoundI *sound = mSound->mSubSound[mSound->mSubSoundList[currentsubsoundid]];

                if (pcmcurrent < sound->mLength)
                {
                    break;
                }

                pcmcurrent -= sound->mLength;
                currentsentenceid++;
            }
        }

        if (postype == FMOD_TIMEUNIT_SENTENCE)
        {
            *position = currentsentenceid;
        }
        else if (postype == FMOD_TIMEUNIT_SENTENCE_SUBSOUND)
        {
            *position = currentsubsoundid;
        }
        else if (postype == FMOD_TIMEUNIT_PCM)
        {
            *position = pcmcurrent;
        }
        else if (postype == FMOD_TIMEUNIT_PCMBYTES)
        {
            SoundI::getBytesFromSamples(pcmcurrent, position, mSound->mChannels, mSound->mFormat);
        }
        else if (postype == FMOD_TIMEUNIT_MS)
        {
            *position = (unsigned int)(1000.0f * ((float)pcmcurrent / mSound->mDefaultFrequency));
        }
    }
    else
    {
        return FMOD_ERR_FORMAT;
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setLoopPoints(unsigned int loopstart, unsigned int looplength)
{
    if (!mSound)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (loopstart >= mSound->mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (loopstart + looplength > mSound->mLength)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    mLoopStart = loopstart;
    mLoopLength = looplength;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::setLoopCount(int loopcount)
{
    mLoopCount = loopcount;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set3DAttributes()
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set3DMinMaxDistance()
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set3DConeSettings(float insideconeangle, float outsideconeangle, float outsidevolume)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set3DConeOrientation(FMOD_VECTOR * orientation)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::set3DOcclusion(float directOcclusion, float reverbOcclusion)
{
    return setVolume(mParent->mVolume);
}

FMOD_RESULT ChannelReal::setReverbProperties(const FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::getReverbProperties(FMOD_REVERB_CHANNELPROPERTIES * prop)
{
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::isPlaying(bool * isplaying)
{
    if (!isplaying)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    if (mFlags & CHANNELREAL_FLAG_PLAYING || mFlags & CHANNELREAL_FLAG_ALLOCATED || mFlags & CHANNELREAL_FLAG_ENDDELAY)
    {
        *isplaying = true;
    }
    else
    {
        *isplaying = false;
    }
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::isVirtual(bool * isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isvirtual = false;
    return FMOD_OK;
}

FMOD_RESULT ChannelReal::getSpectrum(float * spectrumarray, int numentries, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype)
{
    return FMOD_ERR_NEEDSSOFTWARE;
}

FMOD_RESULT ChannelReal::getWaveData(float * wavearray, int numvalues, int channeloffset)
{
    return FMOD_ERR_NEEDSSOFTWARE;
}

FMOD_RESULT ChannelReal::getDSPHead(DSPI * * dsp)
{
    return FMOD_ERR_DSP_NOTFOUND;
}

FMOD_RESULT ChannelReal::setMode(FMOD_MODE mode)
{
    if (mode & (FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI))
    {
        mMode &= ~(FMOD_LOOP_OFF | FMOD_LOOP_NORMAL | FMOD_LOOP_BIDI);

        if (mode & FMOD_LOOP_OFF)
        {
            mMode |= FMOD_LOOP_OFF;
        }
        else if (mode & FMOD_LOOP_NORMAL)
        {
            mMode |= FMOD_LOOP_NORMAL;
        }
        else if (mode & FMOD_LOOP_BIDI)
        {
            mMode |= FMOD_LOOP_BIDI;
        }
    }

    if (mode & FMOD_3D_HEADRELATIVE)
    {
        mMode &= ~FMOD_3D_WORLDRELATIVE;
        mMode |= FMOD_3D_HEADRELATIVE;
    }
    else if (mode & FMOD_3D_WORLDRELATIVE)
    {
        mMode &= ~FMOD_3D_HEADRELATIVE;
        mMode |= FMOD_3D_WORLDRELATIVE;
    }

    if (mode & FMOD_3D_LOGROLLOFF)
    {
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |= FMOD_3D_LOGROLLOFF;
    }
    else if (mode & FMOD_3D_LINEARROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_CUSTOMROLLOFF;
        mMode |= FMOD_3D_LINEARROLLOFF;
    }
    else if (mode & FMOD_3D_CUSTOMROLLOFF)
    {
        mMode &= ~FMOD_3D_LOGROLLOFF;
        mMode &= ~FMOD_3D_LINEARROLLOFF;
        mMode |= FMOD_3D_CUSTOMROLLOFF;
    }

    if (!(mMode & FMOD_HARDWARE))
    {
        if (mode & FMOD_2D)
        {
            mMode &= ~FMOD_3D;
            mMode |= FMOD_2D;
            mParent->mVolume3D = 1.0f;
            mParent->mVolumeOcclusion = 1.0f;
            mParent->mConeVolume3D = 1.0f;
            mParent->mPitch3D = 1.0f;
        }
        else if (mode & FMOD_3D)
        {
            mMode &= ~FMOD_2D;
            mMode |= FMOD_3D;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelRealManual3D::set2DFreqVolumePanFor3D()
{
    float lrpan[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    float fbpan[8] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    int subchannel;
    int numsubchannels;

    if (!(mMode & FMOD_3D))
    {
        return FMOD_OK;
    }

    numsubchannels = mSound->mChannels;

    if (mSystem->mNumListeners == 1)
    {
        for (subchannel = 0; subchannel < numsubchannels; subchannel++)
        {
            FMOD_VECTOR currdiff;
            FMOD_VECTOR front;
            FMOD_VECTOR right;
            FMOD_VECTOR position;
            float distance;
            int index = (numsubchannels > 1) ? subchannel : mSubChannelIndex;

            position = mParent->mPosition3D;
            if (mParent->mUnk5C)
            {
                position.x += mParent->mUnk5C[index].x;
                position.y += mParent->mUnk5C[index].y;
                position.z += mParent->mUnk5C[index].z;
            }

            if (mMode & FMOD_3D_HEADRELATIVE)
            {
                front.x = 0.0f;
                front.y = 0.0f;
                front.z = 1.0f;
                right.x = 1.0f;
                right.y = 0.0f;
                right.z = 0.0f;
                currdiff = position;
            }
            else
            {
                front = mSystem->mListener[0].mFront;
                right = mSystem->mListener[0].mRight;
                currdiff.x = position.x - mSystem->mListener[0].mPosition.x;
                currdiff.y = position.y - mSystem->mListener[0].mPosition.y;
                currdiff.z = position.z - mSystem->mListener[0].mPosition.z;
            }

            if (mSystem->mFlags & FMOD_INIT_3D_RIGHTHANDED)
            {
                front.z = -front.z;
                currdiff.z = -currdiff.z;
            }

            distance = MSL_sqrtf(currdiff.x * currdiff.x + currdiff.y * currdiff.y + currdiff.z * currdiff.z);
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

            lrpan[subchannel] = currdiff.x * right.x + currdiff.y * right.y + currdiff.z * right.z;
            fbpan[subchannel] = currdiff.x * front.x + currdiff.y * front.y + currdiff.z * front.z;
        }
    }

    setFrequency(mParent->mFrequency);

    switch (mSystem->mSpeakerMode)
    {
        case FMOD_SPEAKERMODE_RAW:
        case FMOD_SPEAKERMODE_MONO:
        {
            if (mParent->mMute)
            {
                setVolume(0.0f);
            }
            else
            {
                setVolume(mParent->mVolume);
            }
            break;
        }
        case FMOD_SPEAKERMODE_STEREO:
        {
            if (mParent->mMute)
            {
                setVolume(0.0f);
            }
            else
            {
                setVolume(mParent->mVolume);
            }

            if (numsubchannels > 1)
            {
                float levels[8];

                for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                {
                    levels[subchannel] = MSL_sqrtf(1.0f - (1.0f + lrpan[subchannel]) * 0.5f);
                }
                setSpeakerLevels(FMOD_SPEAKER_FRONT_LEFT, levels, numsubchannels);

                for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                {
                    levels[subchannel] = MSL_sqrtf((1.0f + lrpan[subchannel]) * 0.5f);
                }
                setSpeakerLevels(FMOD_SPEAKER_FRONT_RIGHT, levels, numsubchannels);
            }
            else
            {
                setPan(lrpan[0], 1.0f);
            }
            break;
        }
        case FMOD_SPEAKERMODE_PROLOGIC:
        {
            if (mParent->mMute)
            {
                setVolume(0.0f);
            }
            else
            {
                setVolume(mParent->mVolume);
            }

            if (numsubchannels > 1)
            {
                float levels[8];

                for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                {
                    levels[subchannel] = MSL_sqrtf(1.0f - (1.0f + lrpan[subchannel]) * 0.5f);
                }
                setSpeakerLevels(FMOD_SPEAKER_FRONT_LEFT, levels, numsubchannels);

                for (subchannel = 0; subchannel < numsubchannels; subchannel++)
                {
                    levels[subchannel] = MSL_sqrtf((1.0f + lrpan[subchannel]) * 0.5f);
                }
                setSpeakerLevels(FMOD_SPEAKER_FRONT_RIGHT, levels, numsubchannels);
            }
            else
            {
                setPan(lrpan[0], 1.0f);
            }
            break;
        }
        case FMOD_SPEAKERMODE_QUAD:
        case FMOD_SPEAKERMODE_SURROUND:
        case FMOD_SPEAKERMODE_5POINT1:
        case FMOD_SPEAKERMODE_7POINT1:
        {
            float speakerlevel[8][8];
            int numspeakers = 0;
            int numrealspeakers = 0;

            memset(speakerlevel, 0, sizeof(speakerlevel));

            for (subchannel = 0; subchannel < numsubchannels; subchannel++)
            {
                speakerlevel[FMOD_SPEAKER_LOW_FREQUENCY][subchannel] = 1.0f;
            }

            if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_QUAD)
            {
                numspeakers = 4;
                numrealspeakers = 4;
            }
            if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_SURROUND)
            {
                numspeakers = 5;
                numrealspeakers = 5;
            }
            else if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_5POINT1)
            {
                numspeakers = 5;
                numrealspeakers = 6;
            }
            else if (mSystem->mSpeakerMode == FMOD_SPEAKERMODE_7POINT1)
            {
                numspeakers = 7;
                numrealspeakers = 8;
            }

            for (subchannel = 0; subchannel < numsubchannels; subchannel++)
            {
                if (fbpan[subchannel] == 0.0f && lrpan[subchannel] == 0.0f)
                {
                    float level = 1.4142135f / (float)numspeakers;

                    speakerlevel[FMOD_SPEAKER_FRONT_LEFT][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_FRONT_RIGHT][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_FRONT_CENTER][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_BACK_LEFT][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_BACK_RIGHT][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_SIDE_LEFT][subchannel] = level;
                    speakerlevel[FMOD_SPEAKER_SIDE_RIGHT][subchannel] = level;
                }
                else
                {
                    int speaker;
                    int xzangle = FMOD_Cart2Angle((int)(lrpan[subchannel] * 256.0f), (int)(fbpan[subchannel] * 256.0f));

                    for (speaker = 1; speaker < numspeakers + 1; speaker++)
                    {
                        FMOD_SPEAKER speakerA;
                        FMOD_SPEAKER speakerB;
                        int angleA;
                        int angleB;

                        angleA = mSystem->mSpeakerList[speaker - 1]->mXZAngle;
                        angleB = mSystem->mSpeakerList[speaker] ? mSystem->mSpeakerList[speaker]->mXZAngle : mSystem->mSpeakerList[0]->mXZAngle + 360;
                        speakerA = mSystem->mSpeakerList[speaker - 1]->mSpeaker;
                        speakerB = mSystem->mSpeakerList[speaker] ? mSystem->mSpeakerList[speaker]->mSpeaker : mSystem->mSpeakerList[0]->mSpeaker;

                        if (angleA == angleB)
                        {
                            continue;
                        }

                        if ((xzangle >= angleA && xzangle < angleB) || (xzangle + 360 >= angleA && xzangle + 360 < angleB))
                        {
                            float pan;

                            if (xzangle < angleA)
                            {
                                pan = (float)(xzangle + 360 - angleA) / (float)(angleB - angleA);
                            }
                            else
                            {
                                pan = (float)(xzangle - angleA) / (float)(angleB - angleA);
                            }

                            speakerlevel[speakerA][subchannel] = MSL_sqrtf(1.0f - pan);
                            speakerlevel[speakerB][subchannel] = MSL_sqrtf(pan);
                            break;
                        }
                    }
                }
            }

            if (numsubchannels > 1)
            {
                int speaker;

                for (speaker = 0; speaker < numrealspeakers; speaker++)
                {
                    setSpeakerLevels(speaker, speakerlevel[speaker], numsubchannels);
                }
            }
            else
            {
                setSpeakerMix(speakerlevel[0][0], speakerlevel[1][0], speakerlevel[2][0], speakerlevel[3][0],
                    speakerlevel[4][0], speakerlevel[5][0], speakerlevel[6][0], speakerlevel[7][0]);
            }

            if (!mParent->mMute)
            {
                setVolume(mParent->mVolume);
            }
            break;
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
