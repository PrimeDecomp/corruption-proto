/*
 * G2MEAB Kyoto/Animation/CCECharacterFactoryBuilder.cpp translation-unit scaffold.
 * .text: 0x8058D9D0..0x8058E0C4 (14 native functions, including emitted helpers).
 * NonMatching: implementation has not been reconstructed.
 * Boundary evidence: FirstD9D0 retrieves CHAR factory token; DA7C/DAF8 destroy/construct builder with pool+8. DBD8Build explicitly allocates0x70 atCCECharacterFactoryBuilder.cpp38 after fetching CHAR and SAND resources and callsE3E0 factory constructor. Retain CancelBuildDB34, BuildAsyncDB38, owner transferDDE4/DEBC/DF5C/DF88, embedded dummy constructorE038, CanBuildE058, name lookupE060 and dummy destructorE068+5C. Raw dummy vtable806E2368 proves all five virtual stubs, including Ghidra-missing tiny entries. PreviousD9A0 is independently inspected character virtual forwarding through+88; nextE0C4 changes to factory vtable806E2398 and destroys a0x70 factory, establishing boundary. Both reference builder is inCAssetFactory.cpp but target allocation assertion namesCCECharacterFactoryBuilder.cpp, which takes precedence.
 */
