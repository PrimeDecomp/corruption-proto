/*
 * G2MEAB prototype ddh_cc.c
 * Investigated .text: 0x80656D0C..0x80657050 (end exclusive).
 * DDH (EXI2) comm channel, reconstructed from native code. The EXI2_*
 * functions are the AmcExi2Stubs stubs.
 */
#include "dolphin/types.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/circle_buffer.h"

extern BOOL gDDHIsInitialized;
extern CircleBuffer gDDHRecvCB;
extern u8 gDDHRecvBuf[];

void MWTRACE(int level, const char* fmt, ...);
void EXI2_Init(int* a, int b);
void EXI2_EnableInterrupts(void);
int EXI2_Poll(void);
int EXI2_ReadN(void* buf, int n);
int EXI2_WriteN(void* buf, int n);
void EXI2_Reserve(void);
void EXI2_Unreserve(void);

int ddh_cc_initinterrupts(void)
{
	EXI2_EnableInterrupts();
	return 0;
}

int ddh_cc_peek(void)
{
	u8 buf[2048];
	int n = EXI2_Poll();

	if (n <= 0) {
		return 0;
	}
	if (EXI2_ReadN(buf, n) == 0) {
		CircleBufferWriteBytes(&gDDHRecvCB, buf, n);
	} else {
		return -0x2719;
	}
	return n;
}

int ddh_cc_post_stop(void)
{
	EXI2_Reserve();
	return 0;
}

int ddh_cc_pre_continue(void)
{
	EXI2_Unreserve();
	return 0;
}

int ddh_cc_write(void* data, int size)
{
	int n;

	if (gDDHIsInitialized == 0) {
		MWTRACE(8, "cc not initialized\n");
		return -0x2711;
	}

	MWTRACE(8, "cc_write : Output data 0x%08x %ld bytes\n", data, size);

	while (size > 0) {
		MWTRACE(1, "cc_write sending %ld bytes\n", size);
		n = EXI2_WriteN(data, size);
		if (n == 0) {
			break;
		}
		data = (u8*)data + n;
		size -= n;
	}
	return 0;
}

int ddh_cc_read(void* data, int size)
{
	int n;
	u32 err = 0;
	u8 buf[2048];

	if (gDDHIsInitialized == 0) {
		return -0x2711;
	}

	MWTRACE(1, "Expected packet size : 0x%08x (%ld)\n", size, size);

	while (CBGetBytesAvailableForRead(&gDDHRecvCB) < (u32)size) {
		err = 0;
		n = EXI2_Poll();
		if (n != 0) {
			err = EXI2_ReadN(buf, n);
			if (err == 0) {
				CircleBufferWriteBytes(&gDDHRecvCB, buf, n);
			}
		}
	}

	if (err == 0) {
		CircleBufferReadBytes(&gDDHRecvCB, data, size);
	} else {
		MWTRACE(8, "cc_read : error reading bytes from EXI2 %ld\n", err);
	}
	return err;
}

int ddh_cc_close(void) { return 0; }

int ddh_cc_open(void)
{
	if (gDDHIsInitialized != 0) {
		return -0x2715;
	}
	gDDHIsInitialized = 1;
	return 0;
}

int ddh_cc_shutdown(void) { return 0; }

int ddh_cc_initialize(int* inputPendingPtr, int cb)
{
	MWTRACE(1, "CALLING EXI2_Init\n");
	EXI2_Init(inputPendingPtr, cb);
	MWTRACE(1, "DONE CALLING EXI2_Init\n");
	CircleBufferInitialize(&gDDHRecvCB, gDDHRecvBuf, 0x800);
	return 0;
}
