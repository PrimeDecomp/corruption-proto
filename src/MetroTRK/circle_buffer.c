/*
 * G2MEAB prototype circle_buffer.c translation-unit scaffold.
 * Investigated .text: 0x80657050..0x806572B8 (end exclusive).
 * Functional group investigated from native code and read-only references.
 * Nonfunctional scaffold: native routines and data have not been ported.
 * Keep this object NonMatching until implementation and full verification.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
void fn_806576A8();

void fn_80657260(int obj, int val, int val2);
int fn_806572B0(int obj);

void fn_80657260(int obj, int val, int val2) {
    *(int*)((char*)obj + 0x8) = val;
    *(int*)((char*)obj + 0xc) = val2;
    *(int*)obj = *(int*)((char*)obj + 0x8);
    *(int*)((char*)obj + 0x4) = *(int*)((char*)obj + 0x8);
    *(int*)((char*)obj + 0x10) = 0;
    *(int*)((char*)obj + 0x14) = *(int*)((char*)obj + 0xc);
    fn_806576A8(obj + 24);
}

int fn_806572B0(int obj) {
    return *(int*)((char*)obj + 0x10);
}

