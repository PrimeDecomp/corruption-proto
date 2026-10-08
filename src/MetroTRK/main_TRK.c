#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/main_TRK.h"
#include "TRK_MINNOW_DOLPHIN/MetroTRK/Portable/nubinit.h"

void MWTRACE(int level, const char* fmt, ...);

static DSError TRK_mainError_8077C1F0;

DSError TRK_main(void)
{
	MWTRACE(1, "TRK_Main \n");
	TRK_mainError_8077C1F0 = TRKInitializeNub();

	if (TRK_mainError_8077C1F0 == DS_NoError) {
		TRKNubWelcome();
		TRKNubMainLoop();
	}

	TRK_mainError_8077C1F0 = TRKTerminateNub();
	return TRK_mainError_8077C1F0;
}
