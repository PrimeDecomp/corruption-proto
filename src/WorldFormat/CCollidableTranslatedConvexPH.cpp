/*
 * G2MEAB WorldFormat/CCollidableTranslatedConvexPH.cpp translation-unit scaffold.
 * .text: 0x805B36EC..0x805B5D10 (30 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Complete30-native translated-convex/plane-polyhedron family starts
 * CastRay5B36EC after TriangleCollisionCache final allocator. Collision and boolean
 * wrappers5B38B8..5B472C, primitive type5B49A8, local/world bounds5B49B4/5B4A18, destructor5B4AB0
 * and constructor5B4B28 share vtable806E2570 and baseCollisionPrimitive; constructor stores
 * nonowned data+14 and owned pointer+10. GetType5B4B80 literally registers
 * CCollidableTranslatedConvexPH with setter5B5D00. Preserve underlying convex/convex test5B4BA0,
 * boolean5B5248, segment-plane clipping5B548C, point/transform tests5B5698/5B56F8, plane-edge
 * lookup5B5788, data getters5B57D8/5B57E0, recursively collected edge indices5B57E8 with three
 * vector.h482 assertions, build planes5B5B30, data destructor5B5C44/constructor5B5CB0, setter5B5D00
 * and table getter5B5D08+8. Data builder uses tree surfaces/triangle edges, plane array and
 * ushortvector; allocator/vector helpers called elsewhere remain external. Next5B5D10 is an
 * adjustor thunk subtracting0x20 and branching to5C2318, unrelated to this primitive family. No
 * same named counterpart in Prime/Echoes; both primitive/OBBTree/collider source/header/native
 * inventories checked. Filename inferred from individually inspected literal, not from generic
 * PointInPlanes fingerprint.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" int fn_805B49A8();
extern "C" int fn_805B49A8() {
    return 0x54435048;
}

extern unsigned char lbl_806E2570[40];
extern "C" void fn_80475E24(int, int);
extern "C" int fn_805B4B28(int obj, int val, int val2);
extern "C" int fn_805B4B28(int obj, int val, int val2) {
    fn_80475E24(obj, val2);
    *(int*)obj = (int)lbl_806E2570;
    *(int*)((char*)obj + 0x10) = 0;
    *(int*)((char*)obj + 0x14) = val;
    return obj;
}

extern "C" int fn_805B5788(int obj, int val, int obj2);
extern "C" int fn_805B5788(int obj, int val, int obj2) {
    int val2 = obj + 536;
    unsigned short val3 = ((unsigned short*)val2)[val];
    if (val == *(int*)obj - 1) {
        *(unsigned short*)obj2 = *(int*)((char*)obj + 0x208) - val3;
    } else {
        int val4 = 1;
        *(unsigned short*)obj2 = ((unsigned short*)val2)[val + val4] - val3;
    }
    return *(int*)((char*)obj + 0x210) + (val3 << 1 & 0x1fffe);
}

extern "C" int fn_805B57D8(int obj);
extern "C" int fn_805B57D8(int obj) {
    return *(int*)((char*)obj + 0x25c);
}

extern "C" int fn_805B57E0(int obj);
extern "C" int fn_805B57E0(int obj) {
    return *(int*)((char*)obj + 0x25c);
}

extern "C" void fn_805B5B30();
extern "C" int fn_805B5CB0(int obj, int val);
extern "C" int fn_805B5CB0(int obj, int val) {
    *(int*)obj = 0;
    *(int*)((char*)obj + 0x208) = 0;
    *(int*)((char*)obj + 0x20c) = 0;
    *(int*)((char*)obj + 0x210) = 0;
    *(int*)((char*)obj + 0x214) = 0;
    *(int*)((char*)obj + 0x258) = 0;
    *(int*)((char*)obj + 0x25c) = val;
    fn_805B5B30();
    return obj;
}

extern int lbl_80796EF0;
extern "C" void fn_805B5D00(int val);
extern "C" void fn_805B5D00(int val) {
    lbl_80796EF0 = val;
}


extern "C" int fn_805B5D08();
extern "C" int fn_805B5D08() {
    return lbl_80796EF0;
}

