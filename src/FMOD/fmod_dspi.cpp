// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x80606C50..0x806083B8 (36 retained native functions).
// directly named by target allocation/free body.
// Evidence: Leading recursive reachability06C50 calls own input getter06E2C and is used by graph
// connect073E0 for cycle checks. Descriptor install06D14,input/output connection
// getters06E2C/06EC8,graph reset06F98/07008 and ctor070B8 share lists+28/+3C, counts+50/+54 and
// descriptor+74. Release071AC,graph buffer allocation072E4,connect073E0/disconnect076D0 name
// fmod_dspi.cpp806EDF00. Preserve all retained helpers, callback thunks, raw-only natives and
// inline expansions in target order.
