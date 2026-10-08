#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/serpoll.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"

void MWTRACE(int level, const char* fmt, ...);

static TRKFramingState gTRKFramingState;

void* gTRKInputPendingPtr;

DSError TRKTerminateSerialHandler(void) { return DS_NoError; }

DSError TRKInitializeSerialHandler(void)
{
    gTRKFramingState.msgBufID     = -1;
    gTRKFramingState.receiveState = DSRECV_Wait;
    gTRKFramingState.isEscape     = FALSE;

    MWTRACE(1, "TRK_Packet_Header \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadMemory     %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_WriteMemory    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_Connect \t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReplyAck\t    %ld bytes\n", 0x40);
    MWTRACE(1, "TRK_CMD_ReadRegisters\t%ld bytes\n", 0x40);

    return DS_NoError;
}

void TRKProcessInput(int bufferIdx)
{
    TRKEvent event;

    TRKConstructEvent(&event, NUBEVENT_Request);
    gTRKFramingState.msgBufID = -1;
    event.msgBufID            = bufferIdx;
    TRKPostEvent(&event);
}

void TRKGetInput(void)
{
    MessageBufferID id;

    id = TRKTestForPacket();
    if (id != -1) {
        TRKGetBuffer(id);
        TRKProcessInput(id);
    }
}

MessageBufferID TRKTestForPacket(void)
{
    int err;
    int bufferId;
    TRKBuffer* buffer;
    u8 header[0x40];
    u8 payload[0x880];

    if (TRKPollUART() <= 0) {
        return -1;
    }

    err = TRKGetFreeBuffer(&bufferId, &buffer);
    MWTRACE(4, "TestForPacket : FreeBuffer is  %ld\n", err);
    TRKSetBufferPosition(buffer, 0);

    if (TRKReadUARTN(header, 0x40) == 0) {
        TRKAppendBuffer_ui8(buffer, header, 0x40);
        err = bufferId;
        if (*(int*)header - 0x40 > 0) {
            MWTRACE(1, "Reading payload %ld bytes\n", *(int*)header - 0x40);
            if (TRKReadUARTN(payload, *(int*)header - 0x40) == 0) {
                TRKAppendBuffer_ui8(buffer, payload, *(int*)header);
            } else {
                MWTRACE(8, "TestForPacket : Invalid size of packet hdr.size\n");
                TRKReleaseBuffer(err);
                err = -1;
            }
        }
    } else {
        MWTRACE(8, "TestForPacket : Invalid size of packet\n");
        TRKReleaseBuffer(err);
        err = -1;
    }
    MWTRACE(1, "TestForPacket returning %ld\n", err);
    return err;
}
