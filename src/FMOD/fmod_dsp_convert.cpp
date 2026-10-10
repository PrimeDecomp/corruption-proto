// G2MEAB fmod_dsp_convert.cpp: complete reconstruction (group D).
// .text: 0x80626F40..0x806281C4 (1 native function): DSPI::convert 0x80626F40 (+0x1284), the PCM/float
// format converter. Either side must be PCMFLOAT, otherwise FMOD_ERR_DSP_FORMAT. The 8/16-bit and
// float loops are hand-unrolled by 8 or 4 with a remainder loop (MWCC unrolls further); the 24-bit and
// clamped float loops are single counted loops. Local names follow the 4.06 DWARF.

#include "fmod.h"
#include "fmod_dspi.h"
#include "fmod_types.h"

namespace FMOD {

FMOD_RESULT DSPI::convert(void * outbuffer, void * inbuffer, FMOD_SOUND_FORMAT outformat, FMOD_SOUND_FORMAT informat, unsigned int length, int destchannelstep, int srcchannelstep, float volume)
{
    if (outformat == FMOD_SOUND_FORMAT_PCMFLOAT)
    {
        float * destptr = (float *)outbuffer;

        switch (informat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                signed char * srcptr = (signed char *)inbuffer;
                unsigned int len;

                volume /= 128.0f;

                len = length >> 3;
                while (len)
                {
                    destptr[0 * destchannelstep] = (float)srcptr[0 * srcchannelstep] * volume;
                    destptr[1 * destchannelstep] = (float)srcptr[1 * srcchannelstep] * volume;
                    destptr[2 * destchannelstep] = (float)srcptr[2 * srcchannelstep] * volume;
                    destptr[3 * destchannelstep] = (float)srcptr[3 * srcchannelstep] * volume;
                    destptr[4 * destchannelstep] = (float)srcptr[4 * srcchannelstep] * volume;
                    destptr[5 * destchannelstep] = (float)srcptr[5 * srcchannelstep] * volume;
                    destptr[6 * destchannelstep] = (float)srcptr[6 * srcchannelstep] * volume;
                    destptr[7 * destchannelstep] = (float)srcptr[7 * srcchannelstep] * volume;
                    destptr += 8 * destchannelstep;
                    srcptr += 8 * srcchannelstep;
                    len--;
                }

                len = length & 7;
                while (len)
                {
                    *destptr = (float)*srcptr * volume;
                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                    len--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short * srcptr = (short *)inbuffer;
                unsigned int len;

                volume /= 32768.0f;

                len = length >> 3;
                while (len)
                {
                    destptr[0 * destchannelstep] = (float)srcptr[0 * srcchannelstep] * volume;
                    destptr[1 * destchannelstep] = (float)srcptr[1 * srcchannelstep] * volume;
                    destptr[2 * destchannelstep] = (float)srcptr[2 * srcchannelstep] * volume;
                    destptr[3 * destchannelstep] = (float)srcptr[3 * srcchannelstep] * volume;
                    destptr[4 * destchannelstep] = (float)srcptr[4 * srcchannelstep] * volume;
                    destptr[5 * destchannelstep] = (float)srcptr[5 * srcchannelstep] * volume;
                    destptr[6 * destchannelstep] = (float)srcptr[6 * srcchannelstep] * volume;
                    destptr[7 * destchannelstep] = (float)srcptr[7 * srcchannelstep] * volume;
                    destptr += 8 * destchannelstep;
                    srcptr += 8 * srcchannelstep;
                    len--;
                }

                len = length & 7;
                while (len)
                {
                    *destptr = (float)*srcptr * volume;
                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                    len--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                FMOD_INT24 * srcptr = (FMOD_INT24 *)inbuffer;
                unsigned int count;

                volume /= 8388608.0f;

                for (count = 0; count < length; count++)
                {
                    int val;

                    val = srcptr->val[0] << 8;
                    val |= srcptr->val[srcchannelstep] << 16;
                    val |= srcptr->val[srcchannelstep * 2] << 24;
                    val >>= 8;

                    *destptr = (float)val * volume;
                    srcptr += srcchannelstep;
                    destptr += destchannelstep;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            {
                int * srcptr = (int *)inbuffer;
                unsigned int len;

                volume /= -2147483648.0f;

                len = length >> 2;
                while (len)
                {
                    destptr[0 * destchannelstep] = (float)srcptr[0 * srcchannelstep] * volume;
                    destptr[1 * destchannelstep] = (float)srcptr[1 * srcchannelstep] * volume;
                    destptr[2 * destchannelstep] = (float)srcptr[2 * srcchannelstep] * volume;
                    destptr[3 * destchannelstep] = (float)srcptr[3 * srcchannelstep] * volume;
                    destptr += 4 * destchannelstep;
                    srcptr += 4 * srcchannelstep;
                    len--;
                }

                len = length & 3;
                while (len)
                {
                    *destptr = (float)*srcptr * volume;
                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                    len--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                float * srcptr = (float *)inbuffer;
                unsigned int len;

                len = length >> 2;
                while (len)
                {
                    destptr[0 * destchannelstep] = srcptr[0 * srcchannelstep] * volume;
                    destptr[1 * destchannelstep] = srcptr[1 * srcchannelstep] * volume;
                    destptr[2 * destchannelstep] = srcptr[2 * srcchannelstep] * volume;
                    destptr[3 * destchannelstep] = srcptr[3 * srcchannelstep] * volume;
                    destptr += 4 * destchannelstep;
                    srcptr += 4 * srcchannelstep;
                    len--;
                }

                len = length & 3;
                while (len)
                {
                    *destptr = *srcptr * volume;
                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                    len--;
                }
                break;
            }
        }
    }
    else
    {
        float * srcptr = (float *)inbuffer;

        if (informat != FMOD_SOUND_FORMAT_PCMFLOAT)
        {
            return FMOD_ERR_DSP_FORMAT;
        }

        switch (outformat)
        {
            case FMOD_SOUND_FORMAT_PCM8:
            {
                signed char * destptr = (signed char *)outbuffer;
                unsigned int len;

                volume *= 128.0f;

                len = length >> 2;
                while (len)
                {
                    int val[4];

                    val[0] = (int)(srcptr[0 * srcchannelstep] * volume);
                    val[1] = (int)(srcptr[1 * srcchannelstep] * volume);
                    val[2] = (int)(srcptr[2 * srcchannelstep] * volume);
                    val[3] = (int)(srcptr[3 * srcchannelstep] * volume);

                    destptr[0 * destchannelstep] = val[0] < -128 ? -128 : val[0] > 127 ? 127 : (signed char)val[0];
                    destptr[1 * destchannelstep] = val[1] < -128 ? -128 : val[1] > 127 ? 127 : (signed char)val[1];
                    destptr[2 * destchannelstep] = val[2] < -128 ? -128 : val[2] > 127 ? 127 : (signed char)val[2];
                    destptr[3 * destchannelstep] = val[3] < -128 ? -128 : val[3] > 127 ? 127 : (signed char)val[3];

                    srcptr += 4 * srcchannelstep;
                    destptr += 4 * destchannelstep;
                    len--;
                }

                len = length & 3;
                while (len)
                {
                    int val;

                    val = (int)(*srcptr * volume);

                    *destptr = val < -128 ? -128 : val > 127 ? 127 : (signed char)val;

                    srcptr += srcchannelstep;
                    destptr += destchannelstep;
                    len--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM16:
            {
                short * destptr = (short *)outbuffer;
                unsigned int len;

                volume *= 32768.0f;

                len = length >> 2;
                while (len)
                {
                    int val[4];

                    val[0] = (int)(srcptr[0 * srcchannelstep] * volume);
                    val[1] = (int)(srcptr[1 * srcchannelstep] * volume);
                    val[2] = (int)(srcptr[2 * srcchannelstep] * volume);
                    val[3] = (int)(srcptr[3 * srcchannelstep] * volume);

                    destptr[0 * destchannelstep] = val[0] < -32768 ? -32768 : val[0] > 32767 ? 32767 : (short)val[0];
                    destptr[1 * destchannelstep] = val[1] < -32768 ? -32768 : val[1] > 32767 ? 32767 : (short)val[1];
                    destptr[2 * destchannelstep] = val[2] < -32768 ? -32768 : val[2] > 32767 ? 32767 : (short)val[2];
                    destptr[3 * destchannelstep] = val[3] < -32768 ? -32768 : val[3] > 32767 ? 32767 : (short)val[3];

                    srcptr += 4 * srcchannelstep;
                    destptr += 4 * destchannelstep;
                    len--;
                }

                len = length & 3;
                while (len)
                {
                    int val;

                    val = (int)(*srcptr * volume);

                    *destptr = val < -32768 ? -32768 : val > 32767 ? 32767 : (short)val;

                    srcptr += srcchannelstep;
                    destptr += destchannelstep;
                    len--;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM24:
            {
                FMOD_INT24 * destptr = (FMOD_INT24 *)outbuffer;
                unsigned int count;

                volume *= 8388608.0f;

                for (count = 0; count < length; count++)
                {
                    int val;

                    val = (int)(*srcptr * volume);
                    val = val < -8388608 ? -8388608 : val > 8388607 ? 8388607 : val;

                    destptr->val[0] = (unsigned char)(val & 0xFF);
                    destptr->val[1] = (unsigned char)((val >> 8) & 0xFF);
                    destptr->val[2] = (unsigned char)((val >> 16) & 0xFF);

                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCM32:
            {
                int * destptr = (int *)outbuffer;
                unsigned int count;

                volume *= -2147483648.0f;

                for (count = 0; count < length; count++)
                {
                    float val;

                    val = *srcptr * volume;

                    *destptr = val < -2147483648.0f ? -2147483647 : val > 2147483648.0f ? 2147483647 : (int)val;

                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                }
                break;
            }
            case FMOD_SOUND_FORMAT_PCMFLOAT:
            {
                float * destptr = (float *)outbuffer;
                unsigned int count;

                for (count = 0; count < length; count++)
                {
                    float val;

                    val = *srcptr * volume;

                    *destptr = val < -1.0f ? -1.0f : val > 1.0f ? 1.0f : val;

                    destptr += destchannelstep;
                    srcptr += srcchannelstep;
                }
                break;
            }
        }
    }

    return FMOD_OK;
}

} // namespace FMOD
