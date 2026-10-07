// NonMatching translation-unit scaffold; no implementation is supplied.
// G2MEAB .text: 0x806083B8..0x8060A3C8 (55 retained native functions).
// directly named by target allocation/free body.
// Evidence: Thread entry083B8/pump083D8,Thread ctor0845C,start084F4,release085AC and default worker allocation08640 share worker list8075427C and per-file queues. File ctor08720/reset08788,open08884/close08A54,async pump08B80,buffer/read08D64/08E30/090F8,ten typed reads095C8..09878,seek/tell/subfile/buffer/name methods098C4..09CA4 form complete File lifecycle. Allocation/free/reallocation names fmod_file.cpp806EDF51. Two dtors09CF4/09D58 and list initializer09DD8 close regular methods.
// Preserve all retained helpers, callback thunks, raw-only natives and inline expansions in target order.
