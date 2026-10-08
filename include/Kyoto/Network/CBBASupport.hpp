#ifndef _CBBASUPPORT
#define _CBBASUPPORT

#include "types.h"

// 0x8053EB74: formats into a 0x800-byte buffer and sends it over the broadband adapter
// when connected, otherwise prints it with rs_debugger_printf ("rs_bba_printf: " prefix).
extern "C" void CBBASupport_Printf(const char* format, ...);

#endif // _CBBASUPPORT
