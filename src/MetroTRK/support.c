#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/support.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msgbuf.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msg.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/serpoll.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msghndlr.h"
#include "stddef.h"
#include "string.h"

typedef struct DSFileRequest {
	u32 length;
	u8 command;
	u8 pad0[3];
	union {
		struct {
			u8 mode;
			u8 pad1[3];
			u16 pathLength;
		} open;
		struct {
			u32 handle;
		} close;
		struct {
			u32 handle;
			u16 length;
		} rw;
		struct {
			u32 handle;
			u32 offset;
			u8 whence;
		} position;
	} u;
	u8 pad2[0x40 - 0x14];
} DSFileRequest;

typedef struct DSFileReply {
	/* 0x00 */ u32 length;
	/* 0x04 */ u8 command;
	/* 0x05 */ u8 pad0[3];
	/* 0x08 */ u32 handle;
	/* 0x0C */ u8 pad1[4];
	/* 0x10 */ u32 ioResult;
	/* 0x14 */ u16 dataLength;
	/* 0x16 */ u8 pad2[2];
	/* 0x18 */ u32 offset;
} DSFileReply;

#define FILE_REPLY(buffer) ((DSFileReply*)(buffer)->data)

extern void MWTRACE(int level, const char* fmt, ...);

// Guessed name: traces a received packet as hex bytes, 16 per line.
#pragma dont_inline on
void TRKTraceBufferHex(u8* data, int len)
{
	int i;
	for (i = 0; i < len; i++) {
		MWTRACE(8, "%02x ", data[i]);
		if (i % 16 == 15) {
			MWTRACE(8, "\n");
		}
	}
	MWTRACE(8, "\n");
}
#pragma dont_inline reset

DSError HandlePositionFileSupportRequest(u32 handle, u32* offset, u8 whence,
                                         DSIOResult* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* reply;
	DSFileRequest req;

	memset(&req, 0, sizeof(req));
	req.command             = DSMSG_PositionFile;
	req.length              = 0x40;
	req.u.position.handle   = handle;
	req.u.position.offset   = *offset;
	req.u.position.whence   = whence;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError) {
		error = TRKAppendBuffer_ui8(buffer, (u8*)&req, 0x40);
	}
	if (error == DS_NoError) {
		*ioResult = 0;
		*offset   = -1;
		error     = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			reply = (TRKBuffer*)TRKGetBuffer(replyBufferId);
			if (reply != NULL) {
				*ioResult = FILE_REPLY(reply)->ioResult;
				*offset   = FILE_REPLY(reply)->offset;
			}
		}
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}

DSError HandleCloseFileSupportRequest(u32 handle, DSIOResult* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* reply;
	DSFileRequest req;

	memset(&req, 0, sizeof(req));
	req.command        = DSMSG_CloseFile;
	req.length         = 0x40;
	req.u.close.handle = handle;

	error = TRKGetFreeBuffer(&bufferId, &buffer);
	if (error == DS_NoError) {
		error = TRKAppendBuffer_ui8(buffer, (u8*)&req, 0x40);
	}
	if (error == DS_NoError) {
		*ioResult = 0;
		error     = TRKRequestSend(buffer, &replyBufferId, 3, 3, 0);
		if (error == DS_NoError) {
			reply = (TRKBuffer*)TRKGetBuffer(replyBufferId);
		}
		if (error == DS_NoError) {
			*ioResult = FILE_REPLY(reply)->ioResult;
		}
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}


DSError HandleOpenFileSupportRequest(const char* path, u8 mode, u32* handle,
                                     DSIOResult* ioResult)
{
	DSError error;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	TRKBuffer* reply;
	DSFileRequest req;

	memset(&req, 0, sizeof(req));
	*handle      = 0;
	req.command  = DSMSG_OpenFile;
	req.length   = strlen(path) + 0x41;
	req.u.open.mode = mode;
	req.u.open.pathLength = strlen(path) + 1;

	TRKGetFreeBuffer(&bufferId, &buffer);
	error = TRKAppendBuffer_ui8(buffer, (u8*)&req, 0x40);
	if (error == DS_NoError) {
		error = TRKAppendBuffer_ui8(buffer, (u8*)path, strlen(path) + 1);
	}
	if (error == DS_NoError) {
		*ioResult = 0;
		error     = TRKRequestSend(buffer, &replyBufferId, 7, 3, 0);
		if (error == DS_NoError) {
			reply = (TRKBuffer*)TRKGetBuffer(replyBufferId);
		}
		*ioResult = FILE_REPLY(reply)->ioResult;
		*handle   = FILE_REPLY(reply)->handle;
		TRKReleaseBuffer(replyBufferId);
	}
	TRKReleaseBuffer(bufferId);
	return error;
}


DSError TRKRequestSend(TRKBuffer* msgBuf, int* bufferId, u32 p1, u32 p2, int p3)
{
	int error = DS_NoError;
	TRKBuffer* buffer;
	u32 timer;
	int tries;
	u8 msg_command;
	u8 msg_error;
	BOOL badReply = TRUE;

	*bufferId = -1;

	for (tries = p2 + 1; tries != 0 && *bufferId == -1 && error == DS_NoError;
	     tries--) {
		MWTRACE(1, "Calling MessageSend\n");
		error = TRKMessageSend((TRK_Msg*)msgBuf);
		if (error == DS_NoError) {
			if (p3) {
				timer = 0;
			}

			while (TRUE) {
				do {
					*bufferId = TRKTestForPacket();
					if (*bufferId != -1)
						break;
				} while (!p3 || ++timer < 79999980);

				if (*bufferId == -1)
					break;

				badReply = FALSE;

				buffer = TRKGetBuffer(*bufferId);
				TRKSetBufferPosition(buffer, 0);
				TRKTraceBufferHex(buffer->data, buffer->length);
				msg_command = buffer->data[4];
				MWTRACE(1, "msg_command : 0x%02x hdr->cmdID 0x%02x\n",
				        msg_command, msg_command);

				if (msg_command >= DSMSG_ReplyACK)
					break;

				TRKProcessInput(*bufferId);
				*bufferId = -1;
			}

			if (*bufferId != -1) {
				if (buffer->length < 0x40) {
					badReply = TRUE;
				}
				if (error == DS_NoError && !badReply) {
					msg_error = buffer->data[8];
					MWTRACE(1, "msg_error : 0x%02x\n", msg_error);
				}
				if (error == DS_NoError && !badReply) {
					if ((int)msg_command != DSMSG_ReplyACK
					    || (int)msg_error != DSREPLY_NoError) {
						MWTRACE(8,
						        "RequestSend : Bad ack or non ack received "
						        "msg_command : 0x%02x msg_error 0x%02x\n",
						        msg_command, msg_error);
						badReply = TRUE;
					}
				}
				if (error != DS_NoError || badReply) {
					TRKReleaseBuffer(*bufferId);
					*bufferId = -1;
				}
			}
		}
	}

	if (*bufferId == -1) {
		error = DS_Error800;
	}

	return error;
}

DSError TRKSuppAccessFile(u32 file_handle, u8* data, size_t* count,
                          DSIOResult* io_result, BOOL need_reply, BOOL read)
{
	u16 replyLength;
	TRKBuffer* replyBuffer;
	u8 replyIOResult;
	DSError error;
	BOOL exit;
	u32 done;
	u32 length;
	int replyBufferId;
	int bufferId;
	TRKBuffer* buffer;
	DSFileRequest req;

	if (data == NULL || *count == 0) {
		return DS_ParameterError;
	}

	exit       = FALSE;
	*io_result = DS_IONoError;
	done       = 0;
	error      = DS_NoError;
	while (!exit && done < *count && error == DS_NoError
	       && *io_result == DS_IONoError) {
		memset(&req, 0, sizeof(req));
		if (*count - done > 0x800) {
			length = 0x800;
		} else {
			length = *count - done;
		}

		req.command = read ? DSMSG_ReadFile : DSMSG_WriteFile;
		req.length  = read ? 0x40 : length + 0x40;
		req.u.rw.handle = file_handle;
		req.u.rw.length = length;

		TRKGetFreeBuffer(&bufferId, &buffer);
		error = TRKAppendBuffer_ui8(buffer, (u8*)&req, 0x40);
		if (!read && error == DS_NoError) {
			error = TRKAppendBuffer_ui8(buffer, data + done, length);
		}

		if (error == DS_NoError) {
			if (need_reply) {
				error = TRKRequestSend(buffer, &replyBufferId, 5, 3,
				                       !(read && file_handle == 0));
				if (error == DS_NoError) {
					replyBuffer = (TRKBuffer*)TRKGetBuffer(replyBufferId);
				}
				replyIOResult = FILE_REPLY(replyBuffer)->ioResult;
				replyLength   = FILE_REPLY(replyBuffer)->dataLength;

				if (read && error == DS_NoError && replyLength <= length) {
					TRKSetBufferPosition(replyBuffer, 0x40);
					error = TRKReadBuffer_ui8(replyBuffer, data + done,
					                          replyLength);
					if (error == 0x302) {
						error = DS_NoError;
					}
				}

				if (replyLength != length) {
					length = replyLength;
					exit   = TRUE;
				}

				*io_result = (DSIOResult)replyIOResult;
				TRKReleaseBuffer(replyBufferId);
			} else {
				error = TRKMessageSend((TRK_Msg*)buffer);
			}
		}

		TRKReleaseBuffer(bufferId);
		done += length;
	}

	*count = done;
	return error;
}
