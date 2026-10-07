/*
 * G2MEAB Kyoto/Basics/CStopwatch.cpp translation-unit scaffold.
 * .text: 0x80490000..0x804900E0 (3 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Wait, InitGlobalTimer and static initialization occur consecutively. Wait and static initializer have both-reference structural fingerprints; InitGlobalTimer initializes CSWData and OSGetTime, with an extra target logging call. Static initialization clears CSWData and stores global timer, ending900E0. Nearby assert logger and float-bit helper are unresolved ownership and separately flagged.
 */
