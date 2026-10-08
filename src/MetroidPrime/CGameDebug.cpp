// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x8003B898..0x800492C8 (93 native functions).
// Source identity: asserted target basename; absent from both retail source inventories.
// Complete native/helper/callback inventory retained; no speculative declarations.
// 0x8003B898 +0x24: emitted debug TSignal1 disconnect thunk; member-pointer806B2464..246C targets
// this exact native 0x8003B8BC +0xC8: debug signal-tree dispatch/clear over object+9F54 and
// 0x18-stride signal array 0x8003B984 +0x6C: emitted debug signal callback dispatch 0x8003B9F0
// +0x3C: debug option-change callback inserts changed index in local tree 0x8003BA2C +0x6C: debug
// signal callback connection wrapper 0x8003BA98 +0x130: emitted TSignal1 connector; original
// TSignal1.h allocation assertion line56 0x8003BBC8 +0x40: emitted signal-list insertion wrapper
// 0x8003BC08 +0x70: emitted signal-list link helper
// 0x8003BC78 +0x90: emitted 0x24-byte callback-list node allocator
// 0x8003BD08 +0x230: debug category-name switch, Debug/Cheats/Powerups/Hyper Mode/Renderer etc
// 0x8003BF38 +0xB4: debug text color selector from option field
// 0x8003BFEC +0x88: debug logging mode reset
// 0x8003C074 +0x44: debug log buffer free/clear
// 0x8003C0B8 +0xD0: debug log dump to C:\FIO\debuglog.txt; local clear helper
// 0x8003C188 +0x188: debug log appender; CGameDebug.cpp allocations2539/2554
// 0x8003C310 +0x8C: debug menu reset; local vector assignments/optional menu reset
// 0x8003C39C +0xB4: emitted vector<string> assignment
// 0x8003C450 +0xC8: emitted 12-byte-entry vector assignment
// 0x8003C518 +0x44: emitted 12-byte-entry vector clear
// 0x8003C55C +0x40: retained native/emitted helper; exact historical name/type unresolved
// 0x8003C59C +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x8003C5BC +0xA0: CGameDebug AddDebugOption overload
// 0x8003C65C +0xC0: CGameDebug AddDebugOption overload
// 0x8003C71C +0x88: debug option addition with explicit color
// 0x8003C7A4 +0xC8: debug numeric option addition with explicit color
// 0x8003C86C +0xC: debug capture scalar setter
// 0x8003C878 +0x138: debug movie-capture name getter and numbered-file selection
// 0x8003C9B0 +0xD4: debug movie-capture name sanitization
// 0x8003CA84 +0xD8: emitted string find helper used by movie name sanitization
// 0x8003CB5C +0x80: emitted string iterator search helper
// 0x8003CBDC +0x5ABC: CGameDebug AddDebugOptions; complete prototype debug option registration
// 0x80042698 +0x163C: apply debug-option state to engine systems
// 0x80043CD4 +0x14D8: populate debug-option values from engine/tweak state
// 0x800451AC +0x880: debug menu input and selected option handling
// 0x80045A2C +0xD8: debug menu/timing update
// 0x80045B04 +0x33C: debug text/menu rendering with CFont and log appenders
// 0x80045E40 +0x5F8: debug text drawing helper
// 0x80046438 +0x154: retained native/emitted helper; exact historical name/type unresolved
// 0x8004658C +0xB4: retained native/emitted helper; exact historical name/type unresolved
// 0x80046640 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x800466E4 +0xD58: debug menu construction from category/options; vector.h assertion482
// 0x8004743C +0x48: retained native/emitted helper; exact historical name/type unresolved
// 0x80047484 +0xA8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004752C +0x18C: retained native/emitted helper; exact historical name/type unresolved
// 0x800476B8 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x8004775C +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x8004777C +0x28: retained native/emitted helper; exact historical name/type unresolved
// 0x800477A4 +0xA4: retained native/emitted helper; exact historical name/type unresolved
// 0x80047848 +0x174: retained native/emitted helper; exact historical name/type unresolved
// 0x800479BC +0x8C: retained native/emitted helper; exact historical name/type unresolved
// 0x80047A48 +0x68: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AB0 +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AD0 +0x28: retained native/emitted helper; exact historical name/type unresolved
// 0x80047AF8 +0xBC: retained native/emitted helper; exact historical name/type unresolved
// 0x80047BB4 +0x84: retained native/emitted helper; exact historical name/type unresolved
// 0x80047C38 +0x38: retained native/emitted helper; exact historical name/type unresolved
// 0x80047C70 +0x50: retained native/emitted helper; exact historical name/type unresolved
// 0x80047CC0 +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x80047CE0 +0x24: retained native/emitted helper; exact historical name/type unresolved
// 0x80047D04 +0x9C: retained native/emitted helper; exact historical name/type unresolved
// 0x80047DA0 +0xF0: emitted vector push-back helper; vector.h assertion482
// 0x80047E90 +0x98: debug option-name/value text building
// 0x80047F28 +0x2C4: debug option value formatting
// 0x800481EC +0xA4: debug powerup values from player state
// 0x80048290 +0x74: CGameDebug AddDebugOption textual overload
// 0x80048304 +0x17C: debug option installation and local change-callback registration
// 0x80048480 +0x54: emitted nonstatic member callback adapter, directly installed by48304
// 0x800484D4 +0x30: retained native/emitted helper; exact historical name/type unresolved
// 0x80048504 +0x48: retained native/emitted helper; exact historical name/type unresolved
// 0x8004854C +0xD0: retained native/emitted helper; exact historical name/type unresolved
// 0x8004861C +0x20: retained native/emitted helper; exact historical name/type unresolved
// 0x8004863C +0x28: retained native/emitted helper; exact historical name/type unresolved
// 0x80048664 +0xB8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004871C +0x1D0: CGameDebug constructor; 0x16C options and target 0xA198-scale state
// initialization 0x800488EC +0x40: emitted fixed debug signal-array construction helper 0x8004892C
// +0x3C: retained native/emitted helper; exact historical name/type unresolved 0x80048968 +0x38:
// emitted fixed optional-debug-option-array construction helper 0x800489A0 +0x6C: retained
// native/emitted helper; exact historical name/type unresolved 0x80048A0C +0x20: retained
// native/emitted helper; exact historical name/type unresolved 0x80048A2C +0x28: retained
// native/emitted helper; exact historical name/type unresolved 0x80048A54 +0x40: retained
// native/emitted helper; exact historical name/type unresolved 0x80048A94 +0x18C: debug
// unlock-music/map rewards operation 0x80048C20 +0x11C: debug option-to-player-item translation
// switch 0x80048D3C +0xC: tiny signal-state pointer reset; raw native retained despite missing
// Ghidra entry 0x80048D48 +0x94: debug TSignal1 connection destructor; invokes registered local
// disconnect thunk 0x80048DDC +0x50: emitted signal-list rc_ptr release 0x80048E2C +0x8C: emitted
// signal callback-list destructor 0x80048EB8 +0xB8: emitted 12-byte-entry vector reserve 0x80048F70
// +0x44: emitted 12-byte-entry vector uninitialized copy 0x80048FB4 +0x74: emitted callback-list
// node erase, called by leading debug disconnect thunk 0x80049028 +0x1FC: emitted changed-option
// integer red-black-tree insertion 0x80049224 +0x74: emitted signal-list node destructor 0x80049298
// +0x30: registered CGameDebug static initializer; raw native and .ctors8065B508
