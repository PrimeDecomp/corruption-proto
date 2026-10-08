/*
 * G2MEAB WorldFormat/CGroupedFloatData.cpp translation-unit scaffold.
 * .text: 0x805AE940..0x805AEF30 (14 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Fourteen contiguous natives form a bounded nested-vector serialization/range
 * family: min/max5AE940 scans firstfloat of16-byte records; stream/default group
 * constructors5AE9B0/5AE9E4; recordgroup stream5AE9F8 reads ushortID and vector; four-word record
 * constructor5AEA44 reads two floats and two words; outer vector stream5AEABC and asserted
 * push5AEB5C use stride0x14 groups; inner vector stream5AEC34/asserted push5AECD0 use stride0x10
 * records; reserve5AEDC4, construction wrappers5AEE7C/5AEE9C, inner record uninitialized copy5AEEC4
 * and getter wrapper5AEF10 close family. Constructor/get-range external callers8020E0E0/8020F060
 * use this data; group copy/destruction helpers8020E520/8020E7D8 and outer reserve8020F538 remain
 * emitted externally. Previous RenderingOctree constructor closes5AE940; next5AEF30 begins
 * independently corroborated portal query using0x50portal data layout. No filename assertion or
 * named direct counterpart found in either reference; selected descriptive name is provisional and
 * does not assert float-coordinate meaning. Both complete nearby WorldFormat/PVS/portal native
 * inventories checked; generic stream/vector fingerprints are not class identities.
 */

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" void fn_805AE940(int obj, int obj2);
extern "C" void fn_805AE940(int obj, int obj2) {
    unsigned int val;
    float f;
    int i = *(int*)((char*)obj2 + 0xc);
    float f2 = 3.4028235e38f;
    float f3 = 1.1754944e-38f;
    int val2 = i + *(int*)((char*)obj2 + 0x4) * 20;
    while (i != (unsigned int)val2) {
        int i2 = *(int*)((char*)i + 0x10);
        val = i2 + (*(int*)((char*)i + 0x8) << 4);
        while (i2 != val) {
            f = *(float*)i2;
            if (f < f2) {
                f2 = f;
            }
            if (f3 < f) {
                f3 = f;
            }
            i2 += 16;
        }
        i += 20;
    }
    *(float*)obj = f2;
    *(float*)((char*)obj + 0x4) = f3;
}

extern "C" void fn_805AE9E4(int obj);
extern "C" void fn_805AE9E4(int obj) {
    *(int*)((char*)obj + 0x4) = 0;
    *(int*)((char*)obj + 0x8) = 0;
    *(int*)((char*)obj + 0xc) = 0;
}

struct CInputStream;
struct CInputStream { float ReadFloat(); };
extern "C" int fn_805AEA44(int obj, int obj2);
extern "C" int fn_805AEA44(int obj, int obj2) {
    *(float*)obj = ((CInputStream*)obj2)->ReadFloat();
    *(float*)((char*)obj + 0x4) = ((CInputStream*)obj2)->ReadFloat();
    int val = *(int*)((char*)obj2 + 0x8);
    *(int*)((char*)obj2 + 0x8) = val + 4;
    *(int*)((char*)obj + 0x8) = *(int*)val;
    int val2 = *(int*)((char*)obj2 + 0x8);
    *(int*)((char*)obj2 + 0x8) = val2 + 4;
    *(int*)((char*)obj + 0xc) = *(int*)val2;
    return obj;
}

extern "C" void fn_805AEE9C();
extern "C" void fn_805AEE7C();
extern "C" void fn_805AEE7C() {
    fn_805AEE9C();
}

extern "C" void fn_805AE9F8();
extern "C" void fn_805AEF10();
extern "C" void fn_805AEF10() {
    fn_805AE9F8();
}

