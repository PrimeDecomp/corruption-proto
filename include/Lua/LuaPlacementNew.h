#ifndef LUA_PLACEMENT_NEW_H
#define LUA_PLACEMENT_NEW_H

#include <stddef.h>
inline void* operator new(size_t, void* ptr) { return ptr; }
inline void operator delete(void*, void*) {}

#endif
