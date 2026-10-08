/*
 * G2MEAB prototype circle_buffer.c
 * Investigated .text: 0x80657050..0x806572B8 (end exclusive).
 * Circular byte buffer used by the ddh/gdev comm layers, reconstructed from native code.
 */
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/circle_buffer.h"

#pragma use_lmw_stmw off

extern void* memcpy(void* dst, const void* src, unsigned long n);

int CircleBufferReadBytes(CircleBuffer* cb, u8* dst, u32 len)
{
	u32 first;

	if (len > cb->readableBytes)
		return -1;

	MWEnterCriticalSection(&cb->critical);

	first = cb->size - (cb->readPtr - cb->start);
	if (len < first) {
		memcpy(dst, cb->readPtr, len);
		cb->readPtr += len;
	} else {
		memcpy(dst, cb->readPtr, first);
		memcpy(dst + first, cb->start, len - first);
		cb->readPtr = cb->start + len - first;
	}

	if (cb->size == (u32)(cb->readPtr - cb->start))
		cb->readPtr = cb->start;

	cb->writableBytes += len;
	cb->readableBytes -= len;

	MWExitCriticalSection(&cb->critical);
	return 0;
}

int CircleBufferWriteBytes(CircleBuffer* cb, const u8* src, u32 len)
{
	u32 first;

	if (len > cb->writableBytes)
		return -1;

	MWEnterCriticalSection(&cb->critical);

	first = cb->size - (cb->writePtr - cb->start);
	if (first >= len) {
		memcpy(cb->writePtr, src, len);
		cb->writePtr += len;
	} else {
		memcpy(cb->writePtr, src, first);
		memcpy(cb->start, src + first, len - first);
		cb->writePtr = cb->start + len - first;
	}

	if (cb->size == (u32)(cb->writePtr - cb->start))
		cb->writePtr = cb->start;

	cb->writableBytes -= len;
	cb->readableBytes += len;

	MWExitCriticalSection(&cb->critical);
	return 0;
}

void CircleBufferInitialize(CircleBuffer* cb, u8* buf, u32 size)
{
	cb->start = buf;
	cb->size = size;
	cb->readPtr = cb->start;
	cb->writePtr = cb->start;
	cb->readableBytes = 0;
	cb->writableBytes = cb->size;
	MWInitializeCriticalSection(&cb->critical);
}

u32 CBGetBytesAvailableForRead(CircleBuffer* cb) { return cb->readableBytes; }
