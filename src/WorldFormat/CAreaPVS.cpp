/*
 * G2MEAB WorldFormat/CAreaPVS.cpp translation-unit scaffold.
 * .text: 0x805AE030..0x805AE338 (6 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Six-natives: entityID lookup5AE030, GetLightSet5AE040, visoctree getter5AE0F8,
 * MakeAreaSet5AE100 (allocation asserts CAreaPVS.cpp65), holder constructor5AE21C and emitted
 * visoctree copy5AE2A4+94. Stream factory reads six counts and forms entity/light/octree regions
 * before0x64 allocation; light getter uses visibility-reset out-of-bounds path. Both complete
 * CPVSAreaSet reference inventories/source/header corroborate this coherent family. Target
 * assertion filename takes precedence over retail source name. Next5AE338 is independently
 * inspected bitmap test, retained with following rendering octree.
 */
