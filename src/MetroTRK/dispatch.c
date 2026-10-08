#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/dispatch.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"

void MWTRACE(int level, const char* fmt, ...);

DSError TRKInitializeDispatcher()
{
	return DS_NoError;
}

BOOL TRKDispatchMessage(TRKBuffer* buffer)
{
	DSError error = DS_DispatchError;
	TRKSetBufferPosition(buffer, 0);

	MWTRACE(1, "Dispatch command 0x%08x\n", buffer->data[4]);
	switch (buffer->data[4]) {
	case TRK_DISPATCH_CMD_CONNECT:
		error = TRKDoConnect(buffer);
		break;
	case TRK_DISPATCH_CMD_DISCONNECT:
		error = TRKDoDisconnect(buffer);
		break;
	case TRK_DISPATCH_CMD_RESET:
		error = TRKDoReset(buffer);
		break;
	case TRK_DISPATCH_CMD_OVERRIDE:
		error = TRKDoOverride(buffer);
		break;
	case TRK_DISPATCH_CMD_GETVERSION:
		error = TRKDoVersions(buffer);
		break;
	case TRK_DISPATCH_CMD_GETSUPPORTMASK:
		error = TRKDoSupportMask(buffer);
		break;
	case TRK_DISPATCH_CMD_READMEM:
		error = TRKDoReadMemory(buffer);
		break;
	case TRK_DISPATCH_CMD_WRITEMEM:
		error = TRKDoWriteMemory(buffer);
		break;
	case TRK_DISPATCH_CMD_READREGS:
		error = TRKDoReadRegisters(buffer);
		break;
	case TRK_DISPATCH_CMD_WRITEREGS:
		error = TRKDoWriteRegisters(buffer);
		break;
	case TRK_DISPATCH_CMD_CONTINUE:
		error = TRKDoContinue(buffer);
		break;
	case TRK_DISPATCH_CMD_STEP:
		error = TRKDoStep(buffer);
		break;
	case TRK_DISPATCH_CMD_STOP:
		error = TRKDoStop(buffer);
		break;
	case TRK_DISPATCH_CMD_SETOPTION:
		error = TRKDoSetOption(buffer);
		break;
	}
	MWTRACE(1, "Dispatch complete err = %ld\n", error);
	return error;
}
