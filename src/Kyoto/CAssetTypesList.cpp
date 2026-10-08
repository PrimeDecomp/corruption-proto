#include "Kyoto/CAssetTypesList.hpp"
#include "Kyoto/Alloc/Assert.hpp"

static const unsigned int kAssetTypes[] = {
    'CLSN', 'CSPP', 'CMDL', 'CSKR', 'ANIM', 'CINF', 'TXTR', 'PLTT', 'FONT', 'MADF',
    'MLVL', 'MREA', 'MAPW', 'MAPA', 'SAVW', 'SAVA', 'PART', 'WPSC', 'SWHC', 'DPSC',
    'ELSC', 'CRSC', 'SPSC', 'SRSC', 'USRC', 'BFRC', 'AFSM', 'DCLN', 'AGSC', 'ATBL',
    'CSNG', 'STRG', 'SCAN', 'PATH', 'DGRP', 'HMAP', 'PTLA', 'STLC', 'EGMC', 'RULE',
    'FSM2', 'SAND', 'CHAR', 'RSMP', 'CSMP', 'APRJ', 'RAUD', 'CAUD', 'CTWK', 'FRME',
    'HINT', 'MAPU', 'DUMB', 'CSMP',
};

unsigned int CAssetTypesList::GetFourCCForIndex(unsigned int index) {
  if (index >= sizeof(kAssetTypes) / sizeof(kAssetTypes[0])) {
    CCallStack stack(0, "CAssetTypesList.cpp(419) : ", kUnknownType);
    rs_log_assert_failure(&stack, "CAssetTypesList.cpp", 419, "Verify",
                          "((uint32)index) < kNumAssetTypes",
                          "Invalid index into asset types list in CAssetTypesList::GetFourCCForIndex.");
    rs_debugger_printf("Would have thrown exception: %s\n", "false");
    RAssert_TriggerIllegalInstruction();
  }
  return kAssetTypes[index];
}
