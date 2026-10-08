#ifndef OS_DOLPHIN_DOLPHIN_TRK_GLUE_H
#define OS_DOLPHIN_DOLPHIN_TRK_GLUE_H

#include "dolphin/os.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"
#include "stddef.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*DBCommFunc)(void);
typedef u32 (*DBPollFunc)(void);
typedef void (*DBCommInitFunc)(volatile u8**, __OSInterruptHandler);
typedef int (*DBCommReadFunc)(void*, size_t);
typedef int (*DBCommWriteFunc)(const void*, size_t);

typedef struct DBCommTable {
	/* 0x00 */ DBCommInitFunc initialize_func;
	/* 0x04 */ DBCommFunc init_interrupts_func;
	/* 0x08 */ DBCommFunc shutdown_func;
	/* 0x0C */ DBPollFunc peek_func;
	/* 0x10 */ DBCommReadFunc read_func;
	/* 0x14 */ DBCommWriteFunc write_func;
	/* 0x18 */ DBCommFunc open_func;
	/* 0x1C */ DBCommFunc close_func;
	/* 0x20 */ DBCommFunc pre_continue_func;
	/* 0x24 */ DBCommFunc post_stop_func;
} DBCommTable;

void udp_cc_initialize(volatile u8**, __OSInterruptHandler);
void udp_cc_initinterrupts(void);
void udp_cc_shutdown(void);
u32 udp_cc_peek(void);
int udp_cc_read(void*, size_t);
int udp_cc_write(const void*, size_t);
void udp_cc_open(void);
void udp_cc_close(void);
void udp_cc_pre_continue(void);
void udp_cc_post_stop(void);
void gdev_cc_initialize(volatile u8**, __OSInterruptHandler);
void gdev_cc_initinterrupts(void);
void gdev_cc_shutdown(void);
u32 gdev_cc_peek(void);
int gdev_cc_read(void*, size_t);
int gdev_cc_write(const void*, size_t);
void gdev_cc_open(void);
void gdev_cc_close(void);
void gdev_cc_pre_continue(void);
void gdev_cc_post_stop(void);
void ddh_cc_initialize(volatile u8**, __OSInterruptHandler);
void ddh_cc_initinterrupts(void);
void ddh_cc_shutdown(void);
u32 ddh_cc_peek(void);
int ddh_cc_read(void*, size_t);
int ddh_cc_write(const void*, size_t);
void ddh_cc_open(void);
void ddh_cc_close(void);
void ddh_cc_pre_continue(void);
void ddh_cc_post_stop(void);

void TRKEXICallBack(__OSInterrupt param_0, OSContext* ctx);

DSError TRKInitializeIntDrivenUART(u32 param_0, u32 param_1, u32 param_2,
                                   volatile u8** param_3);

void UnreserveEXI2Port(void);
void ReserveEXI2Port(void);
int TRKPollUART(void);
UARTError TRKReadUARTN(void* bytes, u32 length);
UARTError TRKWriteUARTN(const void* bytes, u32 length);
void TRKLoadContext(OSContext* ctx, u32 r4);
int InitMetroTRKCommTable(int hwId);
void EnableEXI2Interrupts(void);
void TRK_board_display(char* str);

#ifdef __cplusplus
}
#endif

#endif /* OS_DOLPHIN_DOLPHIN_TRK_GLUE_H */
