#ifndef _CONSOLECOMMANDS
#define _CONSOLECOMMANDS

#include "types.h"

// Prototype-only remote console commands (ConsoleCommands.cpp). Minimal: only what main.cpp uses.

// Guessed name. Clears the static command list and releases the two owned static objects;
// called from CMain::ShutdownSubsystems.
void ShutdownConsoleCommands();

class CInputGenerator;
// Guessed name (0x80209AE8). Remembers the architecture's input generator and registers the
// commands; called from the CGameArchitectureSupport constructor.
void InitializeConsoleCommands(CInputGenerator* inputGenerator);

class CStateManager;
// Guessed name (0x80209C44). Remembers the state manager and, when it is not null, registers the
// commands that need it; called from the CStateManager constructor.
void InitializeStateManagerConsoleCommands(CStateManager* mgr);

#endif // _CONSOLECOMMANDS
