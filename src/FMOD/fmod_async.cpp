// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805B60A0..0x805B67C0 (12 retained native functions).
// Original basename directly named by target allocation/free evidence.
// Evidence: Allocation805B65F8 names fmod_async.cpp line415 and frees805B6338 with the same file line236. Entry805B60A0 forwards to worker805B6468; ctor805B60C0 establishes thread, nested intrusive list, lock and flags in0x158 bytes;805B6258 starts FMOD_NONBLOCKING thread. Shared global list807176E4 and mutex8079B7E0 close shutdown/reap/assign paths; final805B6764 registers list destructor805B6184. Next805B67C0 changes to channel-handle validation and forwarding wrappers. Source basename is direct target allocation evidence, not the version/thread string alone.
// Preserve every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty are recorded externally.
