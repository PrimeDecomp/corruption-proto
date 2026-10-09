#ifndef _CGAMEDEBUGDRAW
#define _CGAMEDEBUGDRAW

#include "types.h"

class CAABox;
class CTransform4f;

// Debug drawing helpers from CGameDebugDraw.cpp (the TU name is itself inferred).

// Guessed name. 0x80084DF0: draws a box with the identity transform (it forwards to 0x80084E18
// with CTransform4f::sIdentity).
void DrawDebugAABox(const CAABox& box, float r, float g, float b, float a);
// Guessed name, after the overload above. 0x80084E18 draws the box under the transform;
// CPFAreaOctree::Render passes its area's transform.
void DrawDebugAABox(const CAABox& box, const CTransform4f& xf, float r, float g, float b, float a);

#endif // _CGAMEDEBUGDRAW
