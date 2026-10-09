#ifndef _CONSOLECOMMANDS
#define _CONSOLECOMMANDS

#include "types.h"

// Prototype-only remote console commands (ConsoleCommands.cpp). Minimal: only what main.cpp uses.

// Guessed name. Clears the static command list and releases the two owned static objects;
// called from CMain::ShutdownSubsystems.
void ShutdownConsoleCommands();

#endif // _CONSOLECOMMANDS
