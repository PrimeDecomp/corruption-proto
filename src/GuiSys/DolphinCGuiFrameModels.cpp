/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x8048F1CC..0x8048FF38; 33 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" int fn_8048F1CC(int obj, int val);
extern "C" int fn_8048F1CC(int obj, int val) {
    return *(int*)((char*)(*(int*)((char*)obj + 0x14)) + 4 + (val << 3));
}

extern "C" void fn_8048F3D8();
extern "C" void fn_8048F3B8();
extern "C" void fn_8048F3B8() {
    fn_8048F3D8();
}

extern "C" void fn_8048F770();
extern "C" void fn_8048F750();
extern "C" void fn_8048F750() {
    fn_8048F770();
}

extern "C" void fn_8048F954();
extern "C" void fn_8048F934();
extern "C" void fn_8048F934() {
    fn_8048F954();
}

extern "C" void fn_8048F460(int, int);
extern "C" int fn_8048FCDC(int obj, int obj2);
extern "C" int fn_8048FCDC(int obj, int obj2) {
    if (obj2 != (unsigned int)obj) {
        if (*(unsigned char*)obj) {
            fn_8048F460(*(int*)((char*)obj + 0x4), 1);
        }
        *(unsigned char*)obj = *(unsigned char*)obj2;
        *(int*)((char*)obj + 0x4) = *(int*)((char*)obj2 + 0x4);
        *(unsigned char*)obj2 = 0;
    }
    return obj;
}

extern "C" void fn_8048FE24(int, int);
extern "C" void fn_8048FDB8(int val, int val2, int val3);
extern "C" void fn_8048FDB8(int val, int val2, int val3) {
    int val4 = val;
    int i = 0;
    while (i < val2) {
        fn_8048FE24(val4, val3);
        i++;
        val4 += 16;
    }
}

extern "C" void fn_80058108();
extern "C" void fn_8048FE44(int val);
extern "C" void fn_8048FE44(int val) {
    if ((unsigned int)val != 0) {
        fn_80058108();
    }
}

extern "C" int fn_8048FF00(int obj, int obj2);
extern "C" int fn_8048FF00(int obj, int obj2) {
    int result;
    int val = *(int*)*(int*)obj2;
    if (val) {
        result = *(int*)obj;
        *(int*)obj = result + val;
    } else {
        result = 0;
    }
    *(int*)obj2 = *(int*)obj2 + 4;
    return result;
}

