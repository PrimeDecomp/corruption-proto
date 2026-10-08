// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8060CA08..0x8060D428 (13 retained native functions).
// inferred descriptive source basename; original filename unverified.
// Evidence: Tracker playback reset, voice allocation/release/start, tempo and order/row position
// form one shared SystemI music-state family. CA08 resets fields+9EC/+A00/+A08 and module voice
// lists+518/+540; CD9C sets tempo/tick rate; D0C0 uses sixteen note-frequency constants. Callers
// span several module decoders rather than output plugins. Preserve retained helpers, thunks and
// inline expansions in observed native order.
