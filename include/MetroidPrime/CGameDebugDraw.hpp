#ifndef _CGAMEDEBUGDRAW
#define _CGAMEDEBUGDRAW

#include "types.h"

class CAABox;

// Debug drawing helpers from CGameDebugDraw.cpp (the TU name is itself inferred).

// Guessed name. 0x80084DF0: draws a box with the identity transform (it forwards to 0x80084E18
// with CTransform4f::sIdentity).
void DrawDebugAABox(const CAABox& box, float r, float g, float b, float a);

#endif // _CGAMEDEBUGDRAW
