#ifndef _CSCRIPTCINEMATICCAMERA
#define _CSCRIPTCINEMATICCAMERA

#include "types.h"

#include "MetroidPrime/CActor.hpp"

// Minimal declaration. Guessed class: the "cinematic script object" a CCinematicCamera plays,
// which it and CStateManager::FrameBegin look up through its id and cast with type id 0x2C
// (TypesMatch.cpp, 0x801D7794). The class is not proven: the type ids follow the class names
// alphabetically and 0x2C falls between CScriptCameraHint (0x29) and CScriptControlHint (0x2E),
// among which CScriptCinematicCamera.cpp is the cinematic one.
class CScriptCinematicCamera : public CActor {};

#endif // _CSCRIPTCINEMATICCAMERA
