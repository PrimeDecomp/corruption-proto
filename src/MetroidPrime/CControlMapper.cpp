// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x800105C4..0x80011658 (14 native functions).
// Source identity: inferred from complete class behavior and corroborated by both reference source paths.
// Complete emitted/native helper inventory retained; no speculative declarations.
// 0x800105C4 +0x34: CControlMapper Reset; calls ResetCommandFilters and ResetCommandOverrides
// 0x800105F8 +0x144: CControlMapper ResetCommandOverrides; 90-byte command mask and eight-entry mapping vector
// 0x8001073C +0xAC: CControlMapper GetMapping; override pairs or external tweak mapping provider
// 0x800107E8 +0xD4: CControlMapper RestoreCommandMapping; diagnostic command descriptions and local erase helper
// 0x800108BC +0x80: emitted reserved-vector pair erase helper; shifts 8-byte override entries
// 0x8001093C +0x10C: CControlMapper OverrideCommandMapping; eight-entry capacity and command-name diagnostics
// 0x80010A48 +0xC: CControlMapper command-enabled byte setter; exact historical method name unresolved
// 0x80010A54 +0x110: CControlMapper ResetCommandFilters; initializes 90 command-enabled bytes
// 0x80010B64 +0x17C: CControlMapper boolean-input evaluator A; external input getter8051A374
// 0x80010CE0 +0x17C: CControlMapper boolean-input evaluator B; external input getter8051A384
// 0x80010E5C +0x130: CControlMapper analog-input evaluation; external CFinalInput analog getter8051A604
// 0x80010F8C +0xCC: CControlMapper constructor; two 90-command masks, override vector, then local Reset
// 0x80011058 +0x5D0: CControlMapper GetDescriptionForCommand; complete 90-command target switch
// 0x80011628 +0x30: registered mapper static initializer; .ctors8065B4C8, native bytes retained despite missing Ghidra entry
