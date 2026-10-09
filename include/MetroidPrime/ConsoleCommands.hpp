#ifndef _CONSOLECOMMANDS
#define _CONSOLECOMMANDS

#include "types.h"

// Prototype-only remote console commands (ConsoleCommands.cpp). The host sends a command line
// over the broadband adapter; ExecuteConsoleCommand looks up its first word in a sorted table of
// 29 commands (ADVANCEFRAME .. TITLESCREEN). Other units register "debug vars", named pointers
// to their tuning values, which the GETDEBUGVAR, SETDEBUGVAR and GETALLDEBUGVARS commands read
// and write. Neither Echoes nor Prime has this file.

// Guessed name. Clears the debug var list and releases the two signal connections; called from
// CMain::ShutdownSubsystems.
void ShutdownConsoleCommands();

class CInputGenerator;
// Guessed name (0x80209AE8). Remembers the architecture's input generator and connects to CMain's
// post-update signal; called from the CGameArchitectureSupport constructor.
void InitializeConsoleCommands(CInputGenerator* inputGenerator);

class CStateManager;
// Guessed name (0x80209C44). Remembers the state manager and, when it is not null, connects to
// its destroyed signal so it forgets it again; called from the CStateManager constructor.
void InitializeStateManagerConsoleCommands(CStateManager* mgr);

// Guessed name (0x8020A138). Upper-cases the first word of the line, looks it up in the static
// command table and calls its handler with the rest; prints the command list when unknown.
void ExecuteConsoleCommand(const char* command);

// Guessed names. Register a named tuning value for the debug var commands (0x80209EE4 and
// 0x80209E8C); CCameraFilter, CPlayMovie and CScanDisplay call them. A name registered again
// replaces the old entry.
void AddDebugVar(const char* name, float* value);
void AddDebugVar(const char* name, uint* value);

#endif // _CONSOLECOMMANDS
