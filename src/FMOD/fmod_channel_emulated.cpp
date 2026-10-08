// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805B6C3C..0x805B6E70 (4 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: Ctor805B6C3C calls base805B6E70 and installs vtable806E25F0; external emulated-output
// initializer8060EF40 allocates param2*0x78 bytes and constructs these voices, independently
// proving emulated family identity. Vtable differs from base at advance slot+24 (805B6C78) and
// capability slot+80 (805B6E48). Retained805B6E68 is shared by other low-level vtables, not
// discarded as generic li0/blr. Final8-byte native ends805B6E70; next begins shared base
// constructor installing806E270C. Preserve every retained stub, emitted helper and adjustor thunk;
// full inventory and inlining uncertainty are recorded externally.
