// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x80610930..0x80610968 (1 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Only retained native is plugin free wrapper10930 naming fmod_plugin.cpp806EE9A0
// line0x1E. Plugin vtable806EE990 points to shared destructor805C706C and10930; constructor/base
// list setup is inlined into derived Output/DSP/Codec objects, so one retained body is credible
// rather than a missing inventory. Preserve retained helpers, thunks and inline expansions in
// observed native order.
