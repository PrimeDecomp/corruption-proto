#ifndef _WCHAR_H_
#define _WCHAR_H_

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int fwide(FILE* stream, int mode);
int fputws(const wchar_t* str, FILE* stream);
typedef unsigned short wint_t;
size_t wcslen(const wchar_t* str);
double wcstod(const wchar_t* str, wchar_t** end);
unsigned long wcstoul(const wchar_t* str, wchar_t** end, int base);
int swprintf(wchar_t* dest, size_t count, const wchar_t* format, ...);
extern unsigned short __wctype_map[];
static inline int iswalnum(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0xD0 : 0; }
static inline int iswalpha(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0xC0 : 0; }
static inline int iswcntrl(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x03 : 0; }
static inline int iswdigit(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x10 : 0; }
static inline int iswlower(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x40 : 0; }
static inline int iswprint(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0xDC : 0; }
static inline int iswpunct(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x08 : 0; }
static inline int iswspace(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x06 : 0; }
static inline int iswupper(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x80 : 0; }
static inline int iswxdigit(wint_t ch) { return ch < 256 ? __wctype_map[ch] & 0x20 : 0; }
wint_t towlower(wint_t ch);
wint_t towupper(wint_t ch);

#ifdef __cplusplus
}
#endif

#endif
