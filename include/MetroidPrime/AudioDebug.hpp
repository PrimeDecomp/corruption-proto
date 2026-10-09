#ifndef _AUDIODEBUG
#define _AUDIODEBUG

#include "types.h"

// Prototype-only audio debugging (AudioDebug.cpp). Minimal: only what main.cpp uses.

// Guessed name. The first function of the unit; RsMain calls it every drawn frame. It runs six
// helpers of the unit, among them an audio capture to the BBA and a debug overlay draw.
void UpdateAudioDebug();

#endif // _AUDIODEBUG
