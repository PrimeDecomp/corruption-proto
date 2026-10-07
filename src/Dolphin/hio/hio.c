/*
 * NonMatching translation-unit scaffold; no implementation is supplied.
 * G2MEAB .text: 0x8043CCCC..0x8043DC80 (14 native functions).
 * Evidence: Starts with ExtHandler resetting device/channel and unregistering EXI callback, followed by ExiHandler, PI DbgHandler, TX/RX completion, device enumeration, initialization, mailbox read/write, sync and async transfer, final status read. Complete retained native sequence agrees with SDK hio.c. First5 helper callbacks are not assigned to previous AI TU. Final read-status18-byte EXI sequence ends3DC80; next function is MCC stream event callback, not another HIO API. HIOInit additionally has console/device checks, EXI attach/device1010000 and target version/digest strings; version alone is not used for ownership.
 * Function identities, helper inventory and unresolved data/compiler ownership are recorded in the external workflow research.
 */
