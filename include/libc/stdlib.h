#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <stddef.h>
#include <wchar.h>

#define RAND_MAX 32767

#ifdef __cplusplus
extern "C" {
#endif

double atof(const char* str);
void srand(unsigned int seed);
int rand(void);
int abs(int n);
#ifdef __MWERKS__
#define abs(n) __abs(n)
#endif
long labs(long n);
void exit(int status);
void abort(void);
void* malloc(size_t size);
void* realloc(void* ptr, size_t size);
double strtod(const char* str, char** end);
unsigned long strtoul(const char* str, char** end, int base);
char* getenv(const char* name);
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
void free(void* ptr);
size_t wcstombs(char* dest, const wchar_t* src, size_t max);

typedef int (*_compare_function)(const void*, const void*);
void qsort(void*, size_t, size_t, _compare_function);

#ifdef __cplusplus
}
#endif

#endif
