// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8060E934..0x8060EEEC (8 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Allocation/free wrapperEA38 directly names fmod_output.cpp at806EE680 line0x42;
// constructorE934 installs Output vtable806EE658, captures globals+18/+1C, and initializes embedded
// plugin descriptor+3C/list+70. EA70 handles sample-format callback conversion, ED74 chooses
// emulated/software voice pools. Preserve retained helpers, thunks and inline expansions in
// observed native order.
