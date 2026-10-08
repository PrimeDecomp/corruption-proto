#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/msg.h"
#include "PowerPC_EABI_Support/MetroTRK/trk.h"
#include "TRK_MINNOW_DOLPHIN/Os/dolphin/dolphin_trk_glue.h"

void MWTRACE(int level, const char* fmt, ...);

DSError TRKMessageSend(TRK_Msg* msg)
{
    int err = TRKWriteUARTN(msg->m_msg, msg->m_msgLength);
    MWTRACE(1, "MessageSend : cc_write returned %ld\n", err);
    return DS_NoError;
}
