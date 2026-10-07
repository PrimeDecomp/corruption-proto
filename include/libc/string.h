#ifndef _STRING_H_
#define _STRING_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma section code_type ".init"
void* memcpy(void* dst, const void* src, size_t n);
void* memset(void* dst, int val, size_t n);
void __fill_mem(void* dst, int val, size_t n);
#pragma section code_type

size_t strlen(const char* s);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, size_t num);
char* strcat(char* dst, const char* src);
char* strchr(const char* str, int chr);
int memcmp(const void* a, const void* b, size_t n);
void* memmove(void* dst, const void* src, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
char* strncat(char* dest, const char* src, size_t n);
char* strerror(int error);
int strcoll(const char* lhs, const char* rhs);
char* strstr(const char* str, const char* sub);
char* strpbrk(const char* str, const char* accept);
void* memchr(const void* ptr, int ch, size_t n);
size_t strspn(const char* str, const char* accept);
size_t strcspn(const char* str, const char* reject);

#ifdef __cplusplus
}
#endif

#endif
