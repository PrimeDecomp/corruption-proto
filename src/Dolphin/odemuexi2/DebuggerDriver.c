/*
 * NonMatching translation-unit scaffold; no implementation is supplied.
 * G2MEAB .text: 0x80441590..0x80442010 (14 native functions).
 * Evidence: MP4 DebuggerDriver.c EN0 source branch retains exactly this source order: DBClose/Open, Write/Read/QueryData, InitInterrupts/Comm, DBGHandler/MWCallback, ReadStatus/Write/Read/ReadMailbox/EXIImm. Target DBWrite alternates SendCount bit into1C000/1D000 and emits1F000000 mailbox; DBRead uses1E000/1F000. Status/readmail commands40000000/60000000, EXI registersCC006828/34/38, interrupt19/PI1000 and byte packing establish the complete family independently of names. Last EXIImm size298 ends42010 where separate weak Hu_IsStub returns0. Prime/Echoes have no corresponding original driver TU.
 * Function identities, helper inventory and unresolved data/compiler ownership are recorded in the external workflow research.
 */
