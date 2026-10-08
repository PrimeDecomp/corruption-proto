// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x80610154..0x80610930 (7 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Ctor10154 installs806EE94C and descriptor named FMOD Software Output. 101E4 allocates
// pool14/count-times0x90 software voices, naming fmod_output_software.cpp806EE975
// lines0x53/0x5F;10334 frees them line0x89. 103A8 creates 0x484 SoftwareSample objects via11C3C and
// constructs format/sample buffers (line0xD2). Preserve retained helpers, thunks and inline
// expansions in observed native order.
