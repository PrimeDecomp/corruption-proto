// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80273624..0x80274DB4 (17 native functions).
// Source identity: reference-corroborated source family/path; original target filename not asserted.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80273624 +0x14: player hypermode state query at3288
// 0x80273638 +0xD8: drain player hypermode capacity329C bydt and tweak rates; reset73710 on exhaustion
// 0x80273710 +0xB8: reset player hypermode fields3288..32A0,inventory/HUD/audio state
// 0x802737C8 +0x7E4: player hypermode input/state machine; capacity/corruption/energy,HUD/camera and death handling
// 0x80273FAC +0x1CC: DisableControls; create named Player Hint disabled controls via72D70
// 0x80274178 +0x10: reset alternating left/right recovery input state311C/count3120
// 0x80274188 +0xC8: update alternating left/right recovery input counter
// 0x80274250 +0x4C: player command24 digital-input wrapper
// 0x8027429C +0x4C: player command6 pressed-input wrapper
// 0x802742E8 +0x4C: player command6 held-input wrapper
// 0x80274334 +0x4C: player command7 pressed-input wrapper
// 0x80274380 +0x4C: player command7 held-input wrapper
// 0x802743CC +0x32C: SetAreaPlayerHint; applyflags148 to movement/visor/morph/control direction and disabled-controls hints
// 0x802746F8 +0x11C: ResetPlayerHintState; restore player flags and remove prior control hint
// 0x80274814 +0x3E4: CalculatePlayerControlDirection; camera-to-player/override normalization and flat direction
// 0x80274BF8 +0x18C: UpdatePlayerControlDirection; preserve old direction,calculate74814 then morphball interpolation
// 0x80274D84 +0x30: registered static initializer; .ctors8065B91C,seven independent SDA constants
