// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8065B4A0..0x8065B4B0 (2 native functions; end exclusive).
// Inferred PAD compatibility fallback emitter; historical basename unproven.
// Terminal text and small-BSS order support separating this family from KPAD.
// __PADSpec placement is supporting evidence only; no data extent is assigned.
// Native helpers: none. No inferred inline bodies, thunks or initializers.

#include <dolphin/pad.h>

// PADRecalibrate compatibility fallback; the reset path in main.cpp still calls it as Echoes
// does, but it does nothing here.
BOOL PADRecalibrate(u32 mask) { return FALSE; }

// __PADDisableRecalibration compatibility fallback; returns false.
BOOL __PADDisableRecalibration(BOOL disable) { return FALSE; }
