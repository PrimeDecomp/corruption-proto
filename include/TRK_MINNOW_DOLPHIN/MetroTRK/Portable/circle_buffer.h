#ifndef _TRK_CIRCLE_BUFFER_H
#define _TRK_CIRCLE_BUFFER_H

#include "dolphin/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CircleBuffer {
	u8* readPtr;
	u8* writePtr;
	u8* start;
	u32 size;
	u32 readableBytes;
	u32 writableBytes;
	BOOL critical;
} CircleBuffer;

int CircleBufferReadBytes(CircleBuffer* cb, u8* dst, u32 len);
int CircleBufferWriteBytes(CircleBuffer* cb, const u8* src, u32 len);
void CircleBufferInitialize(CircleBuffer* cb, u8* buf, u32 size);
u32 CBGetBytesAvailableForRead(CircleBuffer* cb);

// CodeWarrior TRK critical section helpers (critical_section.c).
void MWExitCriticalSection(BOOL* cs);
void MWEnterCriticalSection(BOOL* cs);
void MWInitializeCriticalSection(BOOL* cs);

#ifdef __cplusplus
}
#endif

#endif
