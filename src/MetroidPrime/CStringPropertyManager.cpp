// G2MEAB prototype NonMatching translation-unit scaffold.
// .text: 0x80308934..0x80309050 (16 native functions).
// Source identity: inferred descriptive filename for a string-property/listener manager; original
// class spelling and basename unproven. Provisional emitting boundary: complete native interval;
// historical standalone placement unproven. Retain every emitted helper; source inline declarations
// and compiler settings remain uncertain. 0x80308934 +0xE8: set/insert string property, then notify
// listeners 0x80308A1C +0x7C: notify listeners via dynamic callback plus context and argument
// 0x80308A98 +0x28: property-list insertion wrapper
// 0x80308AC0 +0x98: allocate/link node with two owned strings
// 0x80308B58 +0x88: mutable linear string-key lookup
// 0x80308BE0 +0x24: string-key ordering/equality helper
// 0x80308C04 +0xC8: lookup value or lazily initialized static empty string
// 0x80308CCC +0x88: const linear string-key lookup
// 0x80308D54 +0x24: manager serialization wrapper
// 0x80308D78 +0x78: serialize property count and list
// 0x80308DF0 +0x74: serialize one string pair
// 0x80308E64 +0x54: manager stream constructor: empty listeners and load properties
// 0x80308EB8 +0xB4: property-list stream constructor
// 0x80308F6C +0x20: read string pair wrapper
// 0x80308F8C +0x8C: read/copy two strings and destroy temporaries
// 0x80309018 +0x38: default constructor: initialize both list headers
