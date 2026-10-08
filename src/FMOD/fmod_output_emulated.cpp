// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8060EEEC..0x8060F1B8 (5 retained native functions).
// direct target filename in allocation/free body.
// Evidence: CtorEEEC calls Output ctor and installs806EE690. EF40 allocates pool14 and
// count-times-0x78 emulated voices, directly naming fmod_output_emulated.cpp806EE6A0
// lines0x47/0x53; F080 releases voices/pool atline0x7E and calls base freeEA38. Preserve retained
// helpers, thunks and inline expansions in observed native order.
