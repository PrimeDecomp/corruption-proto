/*
 * G2MEAB prototype critical_section.c translation-unit scaffold.
 * Investigated .text: 0x80657654..0x806576AC (end exclusive).
 * Functional group investigated from native code and read-only references.
 * Nonfunctional scaffold: native routines and data have not been ported.
 * Keep this object NonMatching until implementation and full verification.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
void OSRestoreInterrupts();
int OSDisableInterrupts();

void fn_80657654(int obj);
void fn_80657678(int obj);
void fn_806576A8();

void fn_80657654(int obj) {
    OSRestoreInterrupts(*(int*)obj);
}

void fn_80657678(int obj) {
    *(int*)obj = OSDisableInterrupts();
}

void fn_806576A8() {
}

