#ifndef _CBBASUPPORT
#define _CBBASUPPORT

#include "types.h"

// 0x8053EB74: formats into a 0x800-byte buffer and sends it over the broadband adapter
// when connected, otherwise prints it with rs_debugger_printf ("rs_bba_printf: " prefix).
extern "C" void CBBASupport_Printf(const char* format, ...);

// Host file access over the broadband adapter. Open returns 0 on success and stores the handle;
// CGameDebug passes mode 2 to create a file, 1 to append and 0 to probe for an existing file.
extern "C" int CBBASupport_BBAOpen(const char* path, int mode, int* handle);
extern "C" void CBBASupport_BBAWrite(int handle, const void* data, int size);
extern "C" void CBBASupport_BBAClose(int handle);

#endif // _CBBASUPPORT
