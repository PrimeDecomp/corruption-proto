/*
 * G2MEAB Collision/CCharacterPrimitiveData.cpp translation-unit scaffold.
 * .text: 0x8047E860..0x8047F4A0 (26 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Factory7E860 explicitly allocates0x20 with CCharacterPrimitiveData.cpp29.
 * Preserve factory return/owner/auto_ptr helpers and two-vector stream constructor7ECD4, vector
 * stream/push routines with vector.h482 assertions, sphere records stride0x20 and box records
 * stride0x50, both reserve/uninitialized-copy chains through7F438+68. Echoes guessed
 * CSpatialPrimitive.cpp/header is the semantic counterpart; target asserted filename takes
 * precedence. Prime has no standalone counterpart (primitive sphere/box/material interfaces
 * checked). Next7F4A0 is separately inspected 2D triangulation/edge processing, not character
 * primitive data.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" void fn_8047ED24(int, int, int*);
extern "C" void fn_8047EF50(int, int, unsigned char*);
extern "C" void fn_8047EED4();
extern "C" void fn_8047F10C();
extern "C" void fn_8047F178();
extern "C" void fn_8047F214();

extern "C" int fn_8047ECD4(int val, int val2);
extern "C" void fn_8047EEB4();
extern "C" void fn_8047F0EC();
extern "C" void fn_8047F158();
extern "C" void fn_8047F1F4();

extern "C" int fn_8047ECD4(int val, int val2) {
    unsigned char val3[12];
    int val4;
    fn_8047EF50(val, val2, val3);
    fn_8047ED24(val + 16, val2, &val4);
    return val;
}

extern "C" void fn_8047EEB4() {
    fn_8047EED4();
}

extern "C" void fn_8047F0EC() {
    fn_8047F10C();
}

extern "C" void fn_8047F158() {
    fn_8047F178();
}

extern "C" void fn_8047F1F4() {
    fn_8047F214();
}

