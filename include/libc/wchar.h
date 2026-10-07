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
int iswalnum(wint_t ch);
int iswalpha(wint_t ch);
int iswcntrl(wint_t ch);
int iswdigit(wint_t ch);
int iswlower(wint_t ch);
int iswprint(wint_t ch);
int iswpunct(wint_t ch);
int iswspace(wint_t ch);
int iswupper(wint_t ch);
int iswxdigit(wint_t ch);
wint_t towlower(wint_t ch);
wint_t towupper(wint_t ch);

#ifdef __cplusplus
}
#endif

#endif
