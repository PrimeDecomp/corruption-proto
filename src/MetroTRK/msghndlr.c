#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubevent.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msg.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/targcont.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/usr_put.h"
#include "TRK_MINNOW_DOLPHIN/ppc/Generic/targimpl.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"
#include <string.h>

void MWTRACE(int level, const char* fmt, ...);
void __TRK_reset(void);
void SetUseSerialIO(u8 enable);
void usr_puts_serial(const char* str);

BOOL IsTRKConnected;

// Layout of a request as it sits in TRKBuffer::data.
typedef struct TRKRequest {
	/* 0x00 */ u32 header;
	/* 0x04 */ u8 command;
	/* 0x05 */ u8 _05[3];
	/* 0x08 */ u8 options;
	/* 0x09 */ u8 _09[3];
	union {
		struct {
			/* 0x0C */ u16 firstRegister;
			/* 0x0E */ u16 _0E;
			/* 0x10 */ u16 lastRegister;
		} regs;
		struct {
			/* 0x0C */ u16 length;
			/* 0x0E */ u16 _0E;
			/* 0x10 */ u32 start;
		} mem;
		struct {
			/* 0x0C */ u8 count;
			/* 0x0D */ u8 _0D[3];
			/* 0x10 */ u32 rangeStart;
			/* 0x14 */ u32 rangeEnd;
		} step;
		struct {
			/* 0x0C */ u8 value;
		} option;
	} u;
} TRKRequest;

// Fixed size reply packet handed to the transport layer.
typedef struct TRKReply {
	/* 0x00 */ u32 length;
	/* 0x04 */ u8 command;
	/* 0x05 */ u8 _05[3];
	/* 0x08 */ u8 error;
	/* 0x09 */ u8 _09[0x37];
} TRKReply;

static inline void TRKSendReply(u8 error)
{
	TRKReply reply;

	memset(&reply, 0, sizeof(reply));
	reply.command = DSMSG_ReplyACK;
	reply.length  = sizeof(reply);
	reply.error   = error;
	TRKWriteUARTN(&reply, sizeof(reply));
}

BOOL GetTRKConnected()
{
	return IsTRKConnected;
}

void SetTRKConnected(BOOL connected)
{
	IsTRKConnected = connected;
}

DSError TRKDoConnect(TRKBuffer* buffer)
{
	IsTRKConnected = TRUE;
	TRKSendReply(DSREPLY_NoError);
	return DS_NoError;
}

DSError TRKDoDisconnect(TRKBuffer* buffer)
{
	TRKEvent event;

	IsTRKConnected = FALSE;
	TRKSendReply(DSREPLY_NoError);
	TRKConstructEvent(&event, 1);
	TRKPostEvent(&event);
	return DS_NoError;
}

DSError TRKDoReset(TRKBuffer* buffer)
{
	TRKSendReply(DSREPLY_NoError);
	__TRK_reset();
	return DS_NoError;
}

DSError TRKDoOverride(TRKBuffer* buffer)
{
	TRKSendReply(DSREPLY_NoError);
	__TRK_copy_vectors();
	return DS_NoError;
}

DSError TRKDoVersions(TRKBuffer* buffer)
{
	return DS_NoError;
}

DSError TRKDoSupportMask(TRKBuffer* buffer)
{
	return DS_NoError;
}

DSError TRKDoReadMemory(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	DSError error;
	DSReplyError replyError;
	u8 tmpBuffer[0x820] __attribute__((aligned(32)));
	u32 start;
	u16 msgLength;
	u32 length;
	u8 options;
	BOOL aram;

	start     = req->u.mem.start;
	msgLength = req->u.mem.length;
	options   = req->options;
	MWTRACE(1, "ReadMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", req->command,
	        start, msgLength, options);

	if (options & 2) {
		TRKSendReply(DSREPLY_UnsupportedOptionError);
		return DS_NoError;
	}

	aram   = options & 0x40;
	length = msgLength;
	if (aram) {
		error = TRKTargetAccessARAM((u32)tmpBuffer, start, &length, TRUE);
	} else {
		error = TRKTargetAccessMemory(
		    tmpBuffer, start, &length,
		    (options & 8) ? MEMACCESS_UserMemory : MEMACCESS_DebuggerMemory,
		    TRUE);
	}

	TRKResetBuffer(buffer, FALSE);
	if (error == DS_NoError) {
		TRKReply reply;

		memset(&reply, 0, sizeof(reply));
		reply.error   = error;
		reply.length  = length + sizeof(reply);
		reply.command = DSMSG_ReplyACK;
		error         = TRKAppendBuffer(buffer, &reply, sizeof(reply));
		if (aram) {
			error = TRKAppendBuffer(buffer, tmpBuffer + (start & 0x1F), length);
		} else {
			error = TRKAppendBuffer(buffer, tmpBuffer, length);
		}
	}

	if (error != DS_NoError) {
		switch (error) {
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidMemory:
			replyError = DSREPLY_InvalidMemoryRange;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
			break;
		}
		TRKSendReply(replyError);
		return DS_NoError;
	}

	MWTRACE(1, "SendACK : Calling MessageSend\n");
	error = TRKMessageSend((TRK_Msg*)buffer);
	MWTRACE(1, "MessageSend err : %ld\n", error);
	return error;
}

DSError TRKDoWriteMemory(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	DSError error;
	DSReplyError replyError;
	u8 tmpBuffer[0x820] __attribute__((aligned(32)));
	u32 start;
	u16 msgLength;
	u32 length;
	u8 options;

	start     = req->u.mem.start;
	msgLength = req->u.mem.length;
	options   = req->options;
	MWTRACE(1, "WriteMemory (0x%02x) : 0x%08x 0x%08x 0x%08x\n", req->command,
	        start, msgLength, options);

	if (options & 2) {
		TRKSendReply(DSREPLY_UnsupportedOptionError);
		return DS_NoError;
	}

	length = msgLength;
	TRKSetBufferPosition(buffer, 0x40);
	if (options & 0x40) {
		TRKReadBuffer(buffer, tmpBuffer + (start & 0x1F), length);
		error = TRKTargetAccessARAM((u32)tmpBuffer, start, &length, FALSE);
	} else {
		TRKReadBuffer(buffer, tmpBuffer, length);
		error = TRKTargetAccessMemory(
		    tmpBuffer, start, &length,
		    (options & 8) ? MEMACCESS_UserMemory : MEMACCESS_DebuggerMemory,
		    FALSE);
	}

	TRKResetBuffer(buffer, FALSE);
	if (error == DS_NoError) {
		TRKReply reply;

		memset(&reply, 0, sizeof(reply));
		reply.length  = sizeof(reply);
		reply.command = DSMSG_ReplyACK;
		reply.error   = error;
		error         = TRKAppendBuffer(buffer, &reply, sizeof(reply));
	}

	if (error != DS_NoError) {
		switch (error) {
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidMemory:
			replyError = DSREPLY_InvalidMemoryRange;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
			break;
		}
		TRKSendReply(replyError);
		return DS_NoError;
	}

	MWTRACE(1, "SendACK : Calling MessageSend\n");
	error = TRKMessageSend((TRK_Msg*)buffer);
	MWTRACE(1, "MessageSend err : %ld\n", error);
	return error;
}

DSError TRKDoReadRegisters(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	DSError error;
	DSReplyError replyError;
	TRKReply reply;
	u32 registersLength;

	if (req->u.regs.firstRegister > req->u.regs.lastRegister) {
		TRKSendReply(DSREPLY_InvalidRegisterRange);
		return DS_NoError;
	}

	reply.command = DSMSG_ReplyACK;
	reply.length  = 0x468;
	TRKResetBuffer(buffer, FALSE);
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);
	TRKAppendBuffer_ui8(buffer, (u8*)&reply, sizeof(reply));
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);

	error = TRKTargetAccessDefault(0, 0x24, buffer, &registersLength, TRUE);
	MWTRACE(4, "DoReadRegisters : Error reading  default regs 0x%08x\n", error);
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);

	if (error == DS_NoError) {
		error = TRKTargetAccessFP(0, 0x21, buffer, &registersLength, TRUE);
	}
	MWTRACE(4, "DoReadRegisters : Error FP regs 0x%08x\n", error);
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);

	if (error == DS_NoError) {
		error = TRKTargetAccessExtended1(0, 0x60, buffer, &registersLength,
		                                 TRUE);
	}
	MWTRACE(4, "DoReadRegisters : Error extended1 regs 0x%08x\n", error);
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);

	if (error == DS_NoError) {
		error = TRKTargetAccessExtended2(0, 0x1F, buffer, &registersLength,
		                                 TRUE);
	}
	MWTRACE(4, "DoReadRegisters : Error extended2 regs 0x%08x\n", error);
	MWTRACE(4, "DoReadRegisters : Buffer length 0x%08x\n", buffer->length);

	if (error != DS_NoError) {
		switch (error) {
		case DS_UnsupportedError:
			replyError = DSREPLY_UnsupportedOptionError;
			break;
		case DS_InvalidRegister:
			replyError = DSREPLY_InvalidRegisterRange;
			break;
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
			break;
		}
		TRKSendReply(replyError);
		return DS_NoError;
	}

	MWTRACE(1, "SendACK : Calling MessageSend\n");
	error = TRKMessageSend((TRK_Msg*)buffer);
	MWTRACE(1, "MessageSend err : %ld\n", error);
	return error;
}

DSError TRKDoWriteRegisters(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	DSError error;
	DSReplyError replyError;
	u8 options;
	u16 firstRegister;
	u16 lastRegister;
	u32 registersLength;

	options       = req->options;
	firstRegister = req->u.regs.firstRegister;
	lastRegister  = req->u.regs.lastRegister;
	TRKSetBufferPosition(buffer, 0);

	if (firstRegister > lastRegister) {
		TRKSendReply(DSREPLY_InvalidRegisterRange);
		return DS_NoError;
	}

	TRKSetBufferPosition(buffer, 0x40);
	switch (options) {
	case DSREG_Default:
		error = TRKTargetAccessDefault(firstRegister, lastRegister, buffer,
		                               &registersLength, FALSE);
		break;
	case DSREG_FP:
		error = TRKTargetAccessFP(firstRegister, lastRegister, buffer,
		                          &registersLength, FALSE);
		break;
	case DSREG_Extended1:
		error = TRKTargetAccessExtended1(firstRegister, lastRegister, buffer,
		                                 &registersLength, FALSE);
		break;
	case DSREG_Extended2:
		error = TRKTargetAccessExtended2(firstRegister, lastRegister, buffer,
		                                 &registersLength, FALSE);
		break;
	default:
		error = DS_UnsupportedError;
		break;
	}

	TRKResetBuffer(buffer, FALSE);
	if (error == DS_NoError) {
		TRKReply reply;

		memset(&reply, 0, sizeof(reply));
		reply.length  = sizeof(reply);
		reply.command = DSMSG_ReplyACK;
		reply.error   = error;
		error         = TRKAppendBuffer(buffer, &reply, sizeof(reply));
	}

	if (error != DS_NoError) {
		switch (error) {
		case DS_UnsupportedError:
			replyError = DSREPLY_UnsupportedOptionError;
			break;
		case DS_InvalidRegister:
			replyError = DSREPLY_InvalidRegisterRange;
			break;
		case DS_MessageBufferReadError:
			replyError = DSREPLY_PacketSizeError;
			break;
		case DS_CWDSException:
			replyError = DSREPLY_CWDSException;
			break;
		case DS_InvalidProcessID:
			replyError = DSREPLY_InvalidProcessID;
			break;
		case DS_InvalidThreadID:
			replyError = DSREPLY_InvalidThreadID;
			break;
		case DS_OSError:
			replyError = DSREPLY_OSError;
			break;
		default:
			replyError = DSREPLY_CWDSError;
			break;
		}
		TRKSendReply(replyError);
		return DS_NoError;
	}

	MWTRACE(1, "SendACK : Calling MessageSend\n");
	error = TRKMessageSend((TRK_Msg*)buffer);
	MWTRACE(1, "MessageSend err : %ld\n", error);
	return error;
}

static DSError TRKDoFlushCache(TRKBuffer* buffer)
{
	MWTRACE(1, "DoFlushCache unimplemented!!!\n");
	TRKSendReply(DSREPLY_UnsupportedCommandError);
	return DS_NoError;
}

DSError TRKDoContinue(TRKBuffer* buffer)
{
	MWTRACE(1, "DoContinue\n");
	if (TRKTargetStopped() == FALSE) {
		TRKSendReply(DSREPLY_NotStopped);
		return DS_NoError;
	}

	TRKSendReply(DSREPLY_NoError);
	return TRKTargetContinue();
}

DSError TRKDoStep(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	u8 options;
	u8 count;
	u32 rangeStart;
	u32 rangeEnd;
	u32 pc;
	DSError error;

	TRKSetBufferPosition(buffer, 0);
	options    = req->options;
	rangeStart = req->u.step.rangeStart;
	rangeEnd   = req->u.step.rangeEnd;

	switch (options) {
	case DSSTEP_IntoCount:
	case DSSTEP_OverCount:
		count = req->u.step.count;
		if (count < 1) {
			TRKSendReply(DSREPLY_ParameterError);
			return DS_NoError;
		}
		break;
	case DSSTEP_IntoRange:
	case DSSTEP_OverRange:
		pc = TRKTargetGetPC();
		if (pc < rangeStart || pc > rangeEnd) {
			TRKSendReply(DSREPLY_ParameterError);
			return DS_NoError;
		}
		break;
	default:
		TRKSendReply(DSREPLY_UnsupportedOptionError);
		return DS_NoError;
	}

	if (TRKTargetStopped() == FALSE) {
		TRKSendReply(DSREPLY_NotStopped);
		return DS_NoError;
	}

	TRKSendReply(DSREPLY_NoError);
	error = DS_NoError;
	switch (options) {
	case DSSTEP_IntoCount:
	case DSSTEP_OverCount:
		error = TRKTargetSingleStep(count, options == DSSTEP_OverCount);
		break;
	case DSSTEP_IntoRange:
	case DSSTEP_OverRange:
		error = TRKTargetStepOutOfRange(rangeStart, rangeEnd,
		                                options == DSSTEP_OverRange);
		break;
	}
	return error;
}

DSError TRKDoStop(TRKBuffer* buffer)
{
	DSReplyError replyError;

	switch (TRKTargetStop()) {
	case DS_NoError:
		replyError = DSREPLY_NoError;
		break;
	case DS_InvalidProcessID:
		replyError = DSREPLY_InvalidProcessID;
		break;
	case DS_InvalidThreadID:
		replyError = DSREPLY_InvalidThreadID;
		break;
	case DS_OSError:
		replyError = DSREPLY_OSError;
		break;
	default:
		replyError = DSREPLY_Error;
		break;
	}

	TRKSendReply(replyError);
	return DS_NoError;
}

DSError TRKDoSetOption(TRKBuffer* buffer)
{
	TRKRequest* req = (TRKRequest*)buffer->data;
	u8 option = req->options;
	u8 value  = req->u.option.value;

	if (option == 1) {
		usr_puts_serial("\nMetroTRK Option : SerialIO - ");
		if (value != 0) {
			usr_puts_serial("Enable\n");
		} else {
			usr_puts_serial("Disable\n");
		}
		SetUseSerialIO(value);
	}

	TRKSendReply(DSREPLY_NoError);
	return DS_NoError;
}
