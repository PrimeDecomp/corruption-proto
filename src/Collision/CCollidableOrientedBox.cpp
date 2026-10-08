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

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern int lbl_80796BE0;
struct CMaterialFilter;
struct CMaterialList;
struct CMaterialFilter { int Passes(const CMaterialList&) const; };
struct CMaterialList { };
extern "C" void fn_8002C18C(int);

extern "C" int fn_804801DC();
extern "C" int fn_804803A4();
extern "C" void fn_8048043C(int val, int obj, int obj2);
extern "C" void fn_8048087C(int val);

extern "C" int fn_804801DC() {
    return lbl_80796BE0;
}

extern "C" int fn_804803A4() {
    return 0x4f524258;
}

extern "C" void fn_8048043C(int val, int obj, int obj2) {
    if ((unsigned char)((CMaterialFilter*)*(int*)((char*)obj2 + 0x6c))->Passes(*(const CMaterialList*)(obj + 8))) {
        fn_8002C18C(val);
    } else {
        fn_8002C18C(val);
    }
}

extern "C" void fn_8048087C(int val) {
    lbl_80796BE0 = val;
}

