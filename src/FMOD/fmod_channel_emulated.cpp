// G2MEAB prototype translation unit; complete reconstruction (all 4 functions match; the vtable lives in an auto data split).
// G2MEAB .text: 0x805B6C3C..0x805B6E70 (4 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Ctor805B6C3C calls base805B6E70 and installs vtable806E25F0; external emulated-output
// initializer8060EF40 allocates param2*0x78 bytes and constructs these voices, independently
// proving emulated family identity. Vtable differs from base at advance slot+24 (805B6C78) and
// capability slot+80 (805B6E48). Retained805B6E68 is shared by other low-level vtables, not
// discarded as generic li0/blr. Final8-byte native ends805B6E70; next begins shared base
// constructor installing806E270C. Preserve every retained stub, emitted helper and adjustor thunk;
// full inventory and inlining uncertainty are recorded externally.

// Reconstructed with the FMOD Ex 4.06.00 (PS3) debug information as reference; ChannelReal/ChannelEmulated use the G2MEAB layout.
// The weak inline ChannelReal::isStream is emitted here (805B6E68) as the earliest TU using a ChannelReal vtable.

#include "fmod_channel_emulated.h"
#include "fmod.h"
#include "fmod_channeli.h"
#include "fmod_soundi.h"

namespace FMOD {

ChannelEmulated::ChannelEmulated()
{
}

FMOD_RESULT ChannelEmulated::update(int delta)
{
    FMOD_RESULT result;

    result = ChannelReal::update(delta);
    if (result != FMOD_OK)
    {
        return result;
    }

    if (mFlags & CHANNELREAL_FLAG_PAUSED || mFlags & CHANNELREAL_FLAG_ENDDELAY || !(mFlags & CHANNELREAL_FLAG_PLAYING))
    {
        return FMOD_OK;
    }

    delta = delta * (int)(mParent->mFrequency * mParent->mPitch3D) / 1000;
    if (mDirection == 1)
    {
        delta = -delta;
    }
    mPosition += delta;

    if (mSound)
    {
        SoundI *soundi = mSound;

        if (mMode & FMOD_LOOP_NORMAL || (mMode & FMOD_LOOP_BIDI && mLoopCount))
        {
            while (mPosition >= soundi->mLoopStart + soundi->mLoopLength)
            {
                if (!mLoopCount)
                {
                    mPosition = soundi->mLength;
                    mFlags &= ~CHANNELREAL_FLAG_PLAYING;
                    break;
                }

                if (mMode & FMOD_LOOP_NORMAL)
                {
                    mPosition -= soundi->mLoopLength;
                }
                else if (mMode & FMOD_LOOP_BIDI)
                {
                    if (mDirection == 0)
                    {
                        mDirection = 1;
                    }
                    else
                    {
                        mDirection = 0;
                    }
                    mPosition -= delta;
                }

                if (mLoopCount >= 0)
                {
                    mLoopCount--;
                }
            }
        }
        else if (mPosition >= soundi->mLength)
        {
            mPosition = soundi->mLength;
            mFlags &= ~CHANNELREAL_FLAG_PLAYING;
        }
    }

    return FMOD_OK;
}

FMOD_RESULT ChannelEmulated::isVirtual(bool * isvirtual)
{
    if (!isvirtual)
    {
        return FMOD_ERR_INVALID_PARAM;
    }

    *isvirtual = true;
    return FMOD_OK;
}

} // namespace FMOD
