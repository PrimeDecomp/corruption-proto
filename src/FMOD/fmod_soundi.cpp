// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8061606C..0x80619D8C (52 retained native functions).
// direct target filename in allocation/free body.
// Evidence: Leading1606C validates public handle/out pointer (errors20/21), called all four public Sound wrappers; SoundI ctor16098 installs806EF4DC, shared list+2C8 and sync-point list+1A8. Subsound creation16938, codec read/seek16B5C/17138, PCM skip171D4, clear175A8, full defaults/3D/subsound/sentence/name/format/tag/sync-point/mode/loop/userdata APIs share this layout.
// Preserve retained helpers, thunks and inline expansions in observed native order.
