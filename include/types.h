#ifndef CORRUPTION_TYPES_H
#define CORRUPTION_TYPES_H

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned int uint;
typedef unsigned long size_t;
typedef long ptrdiff_t;

#define CHECK_SIZEOF(type, expected) typedef char check_sizeof_##type[(sizeof(type) == (expected)) ? 1 : -1];

#endif
