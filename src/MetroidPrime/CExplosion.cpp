// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80049FE8..0x8004BE78 (46 native functions).
// Source identity: asserted target basename and both reference source/class correspondence.
// Complete native/helper/callback inventory retained; no speculative declarations.
// 0x80049FE8 +0x130: explosion collision-response notification method; three callers pass newly
// constructed CExplosion directly, exact original name unresolved 0x8004A118 +0x30: explosion
// particle-generator method forwarder 0x8004A148 +0x54: explosion generator scaling method
// 0x8004A19C +0x198: explosion render-bounds update from generator optional bounds
// 0x8004A334 +0x2A4: explosion script-message handling; CExplosion.cpp allocations440/454
// 0x8004A5D8 +0x260: explosion Think; generator tick, signal lifetime, light update and termination
// 0x8004A838 +0x11C: explosion PreRender method
// 0x8004A954 +0x30: explosion Render particle-generator forwarder
// 0x8004A984 +0x54: explosion AddToRenderer method
// 0x8004A9D8 +0xB4: explosion destructor; collision helper/generator/effect destruction
// 0x8004AA8C +0x68: explosion three-component CEXT update helper
// 0x8004AAF4 +0x30: explosion CEXT forwarding wrapper
// 0x8004AB24 +0x84: particle CEXT value forwarding helper
// 0x8004ABA8 +0x138: explosion generator transform update
// 0x8004ACE0 +0x194: electric explosion constructor; CExplosion.cpp256, generator allocation0x460
// 0x8004AE74 +0x60: emitted CEffect base destructor in explosion TU; not moved to later effect TU
// 0x8004AED4 +0x21C: particle explosion constructor; CExplosion.cpp222, generator allocation0x350
// 0x8004B0F0 +0x68: explosion collision debug rendering helper
// 0x8004B158 +0xC4: explosion collision signal binding wrapper
// 0x8004B21C +0x130: emitted TSignal4 connector; TSignal4.h assertion58
// 0x8004B34C +0x40: emitted signal-list insertion wrapper
// 0x8004B38C +0x70: emitted signal-list link helper
// 0x8004B3FC +0x90: emitted callback-list node allocator
// 0x8004B48C +0x84: emitted TSignal4 member callback adapter
// 0x8004B510 +0x17C: explosion particle collision-cache refresh; CExplosion.cpp169 allocation
// 0x8004B68C +0x48: emitted collision-cache single_ptr assignment
// 0x8004B6D4 +0x58: emitted collision-cache destructor
// 0x8004B72C +0x98: emitted locked-allocator ushort-vector destructor
// 0x8004B7C4 +0x288: explosion particle collision callback; literal member-pointer806B2590..2598
// 0x8004BA4C +0x48: explosion collision-helper transforms update
// 0x8004BA94 +0x58: explosion collision-helper destructor
// 0x8004BAEC +0x58: emitted collision-cache single_ptr destructor
// 0x8004BB44 +0xA0: explosion collision-helper constructor
// 0x8004BBE4 +0x3C: emitted optional bounds assignment
// 0x8004BC20 +0xC: emitted optional bounds default reset
// 0x8004BC2C +0x4: retained native/emitted helper; exact historical name/type unresolved
// 0x8004BC30 +0x8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004BC38 +0x8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004BC40 +0x8: retained native/emitted helper; exact historical name/type unresolved
// 0x8004BC48 +0x54: emitted CParticleGen ShouldDraw; strong Echoes CExplosion TU match
// 0x8004BC9C +0xC: retained native/emitted helper; exact historical name/type unresolved
// 0x8004BCA8 +0x94: emitted TSignal4 connection destructor
// 0x8004BD3C +0x74: emitted TSignal4 callback-list destructor
// 0x8004BDB0 +0x24: emitted signal disconnect thunk
// 0x8004BDD4 +0x74: emitted signal-list node erase
// 0x8004BE48 +0x30: registered CExplosion static initializer; raw native and .ctors8065B510
