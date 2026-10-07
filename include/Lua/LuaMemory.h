#ifndef LUA_MEMORY_H
#define LUA_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

void* luaHelper_ReallocFunction(void* ptr, int oldsize, int size, void* data,
                              const char* allocName, unsigned int allocFlags);
void luaHelper_FreeFunction(void* ptr, int oldsize, void* data);

#ifdef __cplusplus
}
#endif

#endif
