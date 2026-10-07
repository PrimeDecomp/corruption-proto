/*
 * NonMatching translation-unit scaffold; no implementation supplied.
 * G2MEAB .text 0x80525418..0x80525EB0: 14 native functions.
 * Starts80525418 FStringTableFactory, directly asserts CStringTable.cpp(313), creates memory stream and allocates18-byte CStringTable, calls25DB8 and factory return helper25498. Includes token/derived object helpers2555C/25588 and SReloadData destructor25638; excluding them at256C8 would incorrectly truncate original source. Model ends25418, independently confirmed by graphics agent against both references. Native Load257E4 asserts CStringTable.cpp84/88/91/94 with header87654321, version0..1 and language count; subsequent destructor/ctor/language/lower_bound/sinit complete14-function Echoes-like sequence. Last static initializer25EA0+10 ends25EB0; next is CEmitterElement (graphics agent independently confirmed and vector math native body inspected).
 * Full native/helper and inlining inventory is recorded in the external workflow research.
 */
