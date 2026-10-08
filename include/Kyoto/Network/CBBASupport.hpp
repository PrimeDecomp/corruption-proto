#ifndef _CBBASUPPORT
#define _CBBASUPPORT

#include "types.h"

// Guessed name, from its own "rs_bba_printf: " fallback prefix (0x8053EB74). Formats into
// a 0x800-byte buffer and sends it over the broadband adapter when connected, otherwise
// prints it with rs_debugger_printf.
void rs_bba_printf(const char* format, ...);

#endif // _CBBASUPPORT
