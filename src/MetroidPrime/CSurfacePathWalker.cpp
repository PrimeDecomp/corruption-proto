// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80262A40..0x80264B78 (27 native functions).
// Source identity: inferred descriptive basename; original filename unknown.
// Complete emitted native/helper inventory retained; no speculative declarations.
// 0x80262A40 +0x1E8: coalesce adjacent surface path records,0x50 stride, quaternion/orientation
// tests and reserve64AB0 0x80262C28 +0x860: triangle-surface traversal with barycentric edge
// crossing, adjacent triangle lookup, callback and path sampling 0x80263488 +0x44: construct cached
// surface plus plane; calls634CC and plane extraction804FC784 0x802634CC +0x40: emitted cached
// collision surface constructor; base80081858,ushort triangle index+30 0x8026350C +0x44:
// cached-surface/plane copy wrapper 0x80263550 +0x64: cached-surface/plane copy payload 0x802635B4
// +0x30: optional cached-surface assignment wrapper 0x802635E4 +0x48: optional cached-surface
// assignment; engagement flag+48,copy6362C/64A64 0x8026362C +0x84: cached-surface/plane copy helper
// 0x802636B0 +0x184: orient movement after crossing a triangle edge and project to new surface
// 0x80263834 +0x220: barycentric exit/edge intersection; calls63C84 and63E30
// 0x80263A54 +0x11C: triangle edge relation test; calls63BD8
// 0x80263B70 +0x68: find adjacent triangle usingushort edge/index data
// 0x80263BD8 +0xAC: extract triangle edge vector
// 0x80263C84 +0x1AC: compute barycentric coordinates
// 0x80263E30 +0x11C: choose projection axis from dominant normal component
// 0x80263F4C +0x220: raycast traversal over collision cache and candidate cached triangles
// 0x8026416C +0x278: find nearest admissible cached triangle using AABox-sphere tests
// 0x802643E4 +0x3A0: append0x50 path sample; explicit vector.h482 capacity assertions;64AB0 reserve
// and64784 record ctor 0x80264784 +0xC0: path-record constructor;
// quaternion/transform/position/direction/length payload 0x80264844 +0x9C: test equality/identity
// of cached triangle 0x802648E0 +0xBC: project motion onto plane while preserving speed 0x8026499C
// +0x74: surface-path vector builder constructor; reserve at least4,CRandom16 seed99 0x80264A10
// +0x54: surface-path vector builder destructor/free 0x80264A64 +0x4C: emitted cached-surface copy
// helper 0x80264AB0 +0x98: reserve0x50-stride path-record vector and copy existing payload
// 0x80264B48 +0x30: raw registered static initializer; .ctors8065B900, separate seven SDA constants
