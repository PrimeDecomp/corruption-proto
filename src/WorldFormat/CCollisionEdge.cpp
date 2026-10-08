/*
 * G2MEAB WorldFormat/CCollisionEdge.cpp translation-unit scaffold.
 * .text: 0x8059CBFC..0x8059CC34 (2 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Two ushort-index constructors: stream59CBFC reads two16-bit values,
 * parameter59CC28 stores the pair. Exact Echoes complete TU; Prime has header-inline constructors
 * and no named linked counterpart. Target parameter-store semantics verified individually despite
 * tiny fingerprint ambiguity. Next59CC34 reads an SAreaSurface bounds and four ushort fields.
 */

#include "WorldFormat/CCollisionEdge.hpp"

CCollisionEdge::CCollisionEdge(CInputStream& in)
: mIndex1(in.Get< ushort >()), mIndex2(in.Get< ushort >()) {}

CCollisionEdge::CCollisionEdge(ushort index1, ushort index2) : mIndex1(index1), mIndex2(index2) {}
