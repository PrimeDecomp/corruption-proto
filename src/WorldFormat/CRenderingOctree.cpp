/*
 * G2MEAB WorldFormat/CRenderingOctree.cpp translation-unit scaffold.
 * .text: 0x805AE338..0x805AE940 (8 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: Preserve two separately emitted identical TestBit functions5AE338/5AE364 (individually decompiled; distinct external callers802A8D78/80579CB8), followed by recursive bitmap overlaps5AE390, raw/vector overlap wrappers5AE4B8/5AE4F4, NodeBounds5AE568, ChildCount5AE868 and constructor5AE880+C0. NodeBounds explicitly asserts CRenderingOctree.cpp266 and CRenderingOctree::CNode Asking for children on a leaf node; target extra assertion accounts for size300 versus smaller reference function. Constructor bitmap/node counts and serialized offset tables match both complete reference inventories/interfaces. Vector resize helper8004F08C is emitted elsewhere and not migrated. Next5AE940 iterates a nested float-record vector with different data layout. Both duplicate bit tests retained as target native emissions, without inventing overload names.
 */
