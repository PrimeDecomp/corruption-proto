// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805B67C0..0x805B6C3C (13 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: All13 natives validate opaque channel handles through805BCCA8, then forward to ChannelI stop/pause/volume/frequency/speaker-mix/mute/3D/status/sound operations. Failure paths explicitly clear selected outputs, preserving nontrivial wrappers. First805B67C0 calls stop805BE844; last805B6BE4 calls current-sound805C0330. Next805B6C3C constructs a low-level voice, writes vtable806E25F0 at+74 and never validates an opaque handle. Basename inferred from the complete public wrapper family, not an original assertion.
// Preserve every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty are recorded externally.
