// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805F5914..0x805F69AC (3 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: 5914 initializes0x2000 cosine lookup values in FFT workspace+20000;59DC performs complete bit-reversal/butterfly transform using this table;5E04 windows ring-buffer PCM and writes magnitude spectrum. All three share the same workspace/layout and math constants rather than effect descriptors or DSP graph links. Channel spectrum helper805BA860 calls5914 at805BA89C.
// Preserve all retained helpers, callback thunks, raw-only natives and inline expansions in target order.
