// Synthesized: no DWARF declarations exist for this header in the 4.06 data; contents are prototypes/classes rebuilt from the definitions in the matching 4.06 PS3 object.

#ifndef _FMOD_STRING_H
#define _FMOD_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

int FMOD_strlen(const char * s);
char * FMOD_strcpy(char * dest, const char * src);
char * FMOD_strncpy(char * dest, const char * src, int count);
char * FMOD_strcat(char * dest, const char * src);
char * FMOD_strncat(char * dest, const char * src, int count);
char FMOD_tolower(char in);
char * FMOD_strupr(char * string);
int FMOD_strcmp(const char * string1, const char * string2);
int FMOD_strncmp(const char * string1, const char * string2, int len);
int FMOD_stricmp(const char * string1, const char * string2);
int FMOD_strnicmp(const char * string1, const char * string2, int len);
char * FMOD_strchr(const char * s1, int c);
int FMOD_memcmp(const void * cs, const void * ct, int count);
char * FMOD_strstr(const char * s1, const char * s2);
void * FMOD_memmove(void * dest, const void * src, int count);
char * FMOD_strdup(const char * src);
char * FMOD_eatwhite(const char * string);
int FMOD_atoi(const char * s);

#ifdef __cplusplus
}
#endif

#endif
