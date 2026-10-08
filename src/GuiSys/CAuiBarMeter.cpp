/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x8048E860..0x8048F1CC; 13 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" void fn_8048EE6C(int obj, float f);
extern "C" void fn_8048EE6C(int obj, float f) {
    *(float*)((char*)obj + 0x140) = f;
}

extern "C" void fn_8048EE74(int obj, int obj2);
extern "C" void fn_8048EE74(int obj, int obj2) {
    *(int*)((char*)obj + 0x130) = *(int*)obj2;
}

extern "C" float fn_8048EE80(int obj);
extern "C" float fn_8048EE80(int obj) {
    return *(float*)((char*)obj + 0x13c);
}

extern "C" float fn_8048EE88(int obj);
extern "C" float fn_8048EE88(int obj) {
    return *(float*)((char*)obj + 0x138);
}

extern "C" void fn_8048EE90(int obj, float f);
extern "C" void fn_8048EE90(int obj, float f) {
    *(float*)((char*)obj + 0x138) = f;
}

extern "C" void fn_8048EE98(int obj, float f);
extern "C" void fn_8048EE98(int obj, float f) {
    *(float*)((char*)obj + 0x134) = f;
}

extern "C" int fn_8048F1B8();
extern "C" int fn_8048F1B8() {
    return 0x424d5452;
}

extern "C" int fn_8048F1C4();
extern "C" int fn_8048F1C4() {
    return 3;
}

