// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8060F1B8..0x8060F80C (16 retained native functions).
// direct target filename in allocation/free body.
// Evidence: F1B8 fills shared descriptor80755E3C named FMOD NoSound Output and installs seven callbacks. F274/F284/F2BC report one NoSound Driver and capabilities; F30C/F500 allocate/free buffer using fmod_output_nosound.cpp806EE73B lines0xB9/0xDB. Shared buffer fields+204/+208 and clock-position/lock operations close the same output.
// Preserve retained helpers, thunks and inline expansions in observed native order.
