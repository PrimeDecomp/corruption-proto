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
