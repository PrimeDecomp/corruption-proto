// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x805BAB64..0x805BC8B8 (34 retained native functions).
// Inferred basename; original source filename is unproven.
// Evidence: 805BAB64 constructs a0xB4-byte low-level aggregate voice with child count+4, child
// slots+8, base table+74, intrusive lists+78/+A0 and distinct vtable806E2A4C. SystemI8061E5B4
// embeds two such aggregates at+FE0/+1094;80621038 allocates0xB4 from a freelist and calls the same
// constructor, independently proving lifecycle closure. Native dispatch methods iterate child
// voices through their+74 vtables, retaining one-child exceptions and first-error handling;
// prepare/start/position/mode/stop methods share that object. Final805BC8B0 is an8-byte -0x78
// destructor adjustor to805BC7C0. Next805BC8B8 operates on a different high-level channel-group
// object and explicitly asserted channelgroupi allocation/free. Inferred basename; group here is
// the low-level aggregate voice rather than a claim of identical public API class naming. Preserve
// every retained stub, emitted helper and adjustor thunk; full inventory and inlining uncertainty
// are recorded externally.
