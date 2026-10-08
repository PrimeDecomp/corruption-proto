// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80271294..0x80272394 (15 native functions).
// Source identity: asserted target basename; reference-consistent ScriptObjects placement.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80271294 +0x88: movie actor destructor; vtable806B9F18, movieplayer and name string cleanup
// 0x8027131C +0x32C: StreamedMovie tagged loader; direct CScriptStreamedMovie.cpp542
// allocation0x1A8, calls721BC 0x80271648 +0x104: movie audio volume/fade adjustment through
// player8052C2D4 0x8027174C +0x58: unload movieplayer and clear active global807996EC 0x802717A4
// +0x38: reset loaded movieplayer through8052C744 0x802717DC +0x5C: stop movie playback and clear
// active/fade state 0x80271838 +0xD8: start loaded movie playback and set active global; diagnostic
// if not loaded 0x80271910 +0x108: load movie; direct CScriptStreamedMovie.cpp327 allocation0x120
// and movieplayer ctor8052ED14 0x80271A18 +0xD4: advance connected actor/camera transform along
// movie playback 0x80271AEC +0x114: render movie with graphics/depth state preservation 0x80271C00
// +0x54: register movie category with manager 0x80271C54 +0x1C0: script dispatch
// LOAD/PLAY/STOP/RSET/ULOD/XDEL and ARRV notification 0x80271E14 +0x3A8: playback update:
// loop/range/fade/pause state and completion notifications 0x802721BC +0x1A8: movie actor
// constructor; vtable806B9F18,name170,playback fields180..1A0,DVD existence check 0x80272364 +0x30:
// registered static initializer; .ctors8065B90C,seven independent SDA constants
