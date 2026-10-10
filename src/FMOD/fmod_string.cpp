// Complete reconstruction of the retained G2MEAB string helpers (.text 0x80619D8C..0x8061A228). The unit
// also holds the wide-string helpers (FMOD_strlenW 0x8061A1C8, FMOD_strncpyW 0x8061A1F4), which by
// alphabetical position are the separate fmod_stringw.cpp of later FMOD builds. Local names follow
// another MWCC FMOD build's debug information. FMOD_stricmp, FMOD_strchr, FMOD_memcmp,
// FMOD_strstr, FMOD_eatwhite and FMOD_atoi are dead-stripped and stay as empty placeholders.

#include "fmod_string.h"
#include "fmod_memory.h"

int FMOD_strlen(const char * s)
{
    const char * sc;

    for (sc = s; *sc != '\0'; ++sc)
    {
    }
    return sc - s;
}

char * FMOD_strcpy(char * dest, const char * src)
{
    char * tmp = dest;

    while ((*dest++ = *src++) != '\0')
    {
    }
    return tmp;
}

char * FMOD_strncpy(char * dest, const char * src, int count)
{
    char * tmp = dest;

    while (count-- && (*dest++ = *src++) != '\0')
    {
    }
    return tmp;
}

char * FMOD_strcat(char * dest, const char * src)
{
    char * tmp = dest;

    while (*dest)
    {
        dest++;
    }
    while ((*dest++ = *src++) != '\0')
    {
    }
    return tmp;
}

char * FMOD_strncat(char * dest, const char * src, int count)
{
    char * tmp = dest;

    if (count)
    {
        while (*dest)
        {
            dest++;
        }
        while ((*dest++ = *src++) != '\0')
        {
            if (--count == 0)
            {
                *dest = '\0';
                break;
            }
        }
    }
    return tmp;
}

char FMOD_tolower(char in)
{
    if (in >= 'A' && in <= 'Z')
    {
        in += 'a' - 'A';
    }
    return in;
}

char * FMOD_strupr(char * string)
{
    char * cp;

    for (cp = string; *cp; ++cp)
    {
        if ('a' <= *cp && *cp <= 'z')
        {
            *cp += 'A' - 'a';
        }
    }
    return string;
}

int FMOD_strcmp(const char * string1, const char * string2)
{
    char c1, c2;

    do
    {
        c1 = *string1++;
        c2 = *string2++;
    } while (c1 && c1 == c2);

    return c1 - c2;
}

int FMOD_strncmp(const char * string1, const char * string2, int len)
{
    char c1, c2;
    int count = 0;

    do
    {
        c1 = *string1++;
        c2 = *string2++;
        count++;
    } while (c1 && c1 == c2 && count < len);

    return c1 - c2;
}

int FMOD_stricmp(const char * string1, const char * string2)
{
}

int FMOD_strnicmp(const char * string1, const char * string2, int len)
{
    char c1, c2;
    int count = 0;

    do
    {
        c1 = FMOD_tolower(*string1++);
        c2 = FMOD_tolower(*string2++);
        count++;
    } while (c1 && c1 == c2 && count < len);

    return c1 - c2;
}

char * FMOD_strchr(const char * s1, int c)
{
}

int FMOD_memcmp(const void * cs, const void * ct, int count)
{
}

char * FMOD_strstr(const char * s1, const char * s2)
{
}

void * FMOD_memmove(void * dest, const void * src, int count)
{
    char * tmp;
    char * s;

    if (dest <= src)
    {
        tmp = (char *)dest;
        s = (char *)src;
        while (count--)
        {
            *tmp++ = *s++;
        }
    }
    else
    {
        tmp = (char *)dest + count;
        s = (char *)src + count;
        while (count--)
        {
            *--tmp = *--s;
        }
    }
    return dest;
}

char * FMOD_strdup(const char * src)
{
    char * ret = (char *)FMOD_Memory_Alloc(FMOD_strlen(src) + 1);

    if (ret)
    {
        FMOD_strcpy(ret, src);
    }
    return ret;
}

char * FMOD_eatwhite(const char * string)
{
}

int FMOD_atoi(const char * s)
{
}

int FMOD_strlenW(const short * s)
{
    const short * sc;

    for (sc = s; *sc; ++sc)
    {
    }
    return sc - s;
}

short * FMOD_strncpyW(short * dest, const short * src, int count)
{
    short * tmp = dest;

    while (count-- && (*dest++ = *src++) != 0)
    {
    }
    return tmp;
}
