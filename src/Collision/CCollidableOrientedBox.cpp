/*
 * G2MEAB Collision/CCollidableOrientedBox.cpp translation-unit scaffold.
 * .text: 0x804801DC..0x80480884 (12 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Twelve-native coherent primitive family: table getter801DC, support-point
 * function801E4, primitive type803A4, world-center803B0, localAABox803E0, Transform8043C,
 * contact8048C, boolean8069C, destructor8078C, constructor807EC (baseCollisionPrimitive
 * +vtable806CE1B8, transform+10 and extents+40), GetType8085C and setter8087C+8. GetType literally
 * registers CCollidableOrientedBox and supplies setter8087C; InternalColliders74888 registers
 * contact8048C/boolean8069C. Contact uses transformed boxes and external GJK helpers80481070/814B0;
 * boolean crosschecks twoSAT implementations and prints Collide::OBBox_OBBox_Bool result mismatch.
 * These external helpers remain where emitted. Next80884 tests four per-object AABoxes at+E4.. with
 * bitmask+150, a distinct larger-object family. Both Prime/Echoes primitive/OBBox interfaces and
 * full native inventories consulted; neither has this named standalone class/TU.
 */
