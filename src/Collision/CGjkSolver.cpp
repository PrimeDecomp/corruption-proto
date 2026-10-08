// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text:0x80480884..0x804816E4 (eight retained native functions).
// Inferred descriptive basename; original class/source name is unproven.
// Closed support-mapped GJK distance solver: six simplex/determinant/witness helpers, main
// iteration and constructor. Both entries are called by oriented-box contact8048048C; complete
// target/caller/reference evidence is recorded externally. Historical separate source versus header
// emissions in CCollidableOrientedBox.cpp remains uncertain. Preserve all retained natives and
// compiler behavior; no extra declarations or bodies are invented.

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" unsigned char CVector3d_EqualComponents(int, int);
extern "C" void fn_80480AF4(int, int, int);
extern "C" unsigned char fn_80480BB0(int, int);
extern "C" void fn_80480C94();

extern "C" bool fn_80480884(int obj, int val);
extern "C" bool fn_804808FC(int obj, int obj2);

extern "C" bool fn_80480884(int obj, int val) {
    int val2;
    int i = 0;
    val2 = obj + 228;
    int val3 = 1;
    do {
        if ((*(int*)((char*)obj + 0x150) & val3) && CVector3d_EqualComponents(val2, val)) {
            return true;
        }
        i++;
        val3 <<= 1;
        val2 += 24;
    } while (i < 4);
    return false;
}

extern "C" bool fn_804808FC(int obj, int obj2) {
    fn_80480C94();
    for (int i = *(int*)((char*)obj + 0x144); i; i--) {
        if (i == (i & *(int*)((char*)obj + 0x144)) && fn_80480BB0(obj, i | *(int*)((char*)obj + 0x14c))) {
            *(int*)((char*)obj + 0x144) = i | *(int*)((char*)obj + 0x14c);
            fn_80480AF4(obj, *(int*)((char*)obj + 0x144), obj2);
            return true;
        }
    }
    if (fn_80480BB0(obj, *(int*)((char*)obj + 0x14c))) {
        *(int*)((char*)obj + 0x144) = *(int*)((char*)obj + 0x14c);
        int val = obj + *(int*)((char*)obj + 0x148) * 24;
        *(double*)obj2 = *(double*)((char*)val + 0xe4);
        *(double*)((char*)obj2 + 0x8) = *(double*)((char*)val + 0xec);
        *(double*)((char*)obj2 + 0x10) = *(double*)((char*)val + 0xf4);
        return true;
    }
    return false;
}

