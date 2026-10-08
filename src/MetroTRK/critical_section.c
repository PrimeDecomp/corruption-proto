/*
 * G2MEAB prototype critical_section.c
 * Investigated .text: 0x80657654..0x806576AC (end exclusive).
 * Interrupt-disable based critical section helpers used by the circle buffer.
 */
#include "dolphin/os.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/circle_buffer.h"

void MWExitCriticalSection(BOOL* cs) { OSRestoreInterrupts(*cs); }

void MWEnterCriticalSection(BOOL* cs) { *cs = OSDisableInterrupts(); }

void MWInitializeCriticalSection(BOOL* cs) { }
