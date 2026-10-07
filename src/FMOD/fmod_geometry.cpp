// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x8060A3C8..0x8060B08C (10 retained native functions).
// inferred descriptive basename; original filename unverified.
// Evidence: Geometry manager ctor0A3C8 clears tree/list fields and enables manager; destructor0A3E8 is called by SystemI destructor0B0D4 for member+FC8. Occlusion query0A458 flushes dirty geometry0A51C then traverses independent octree0DB80 through callback0A42C. Point transform0A57C,bounds transform0A5F8,polygon segment test0A808,dirty polygon rebuild0A9FC and per-geometry traversal0AF3C form complete spatial occlusion implementation.
// Preserve all retained helpers, callback thunks, raw-only natives and inline expansions in target order.
