#include "Kyoto/CAssetTypesList.hpp"
#include "Kyoto/Alloc/Assert.hpp"

static const unsigned int kAssetTypes[] = {
    'CLSN', 'CSPP', 'CMDL', 'CSKR', 'ANIM', 'CINF', 'TXTR', 'PLTT', 'FONT', 'MADF', 'MLVL',
    'MREA', 'MAPW', 'MAPA', 'SAVW', 'SAVA', 'PART', 'WPSC', 'SWHC', 'DPSC', 'ELSC', 'CRSC',
    'SPSC', 'SRSC', 'USRC', 'BFRC', 'AFSM', 'DCLN', 'AGSC', 'ATBL', 'CSNG', 'STRG', 'SCAN',
    'PATH', 'DGRP', 'HMAP', 'PTLA', 'STLC', 'EGMC', 'RULE', 'FSM2', 'SAND', 'CHAR', 'RSMP',
    'CSMP', 'APRJ', 'RAUD', 'CAUD', 'CTWK', 'FRME', 'HINT', 'MAPU', 'DUMB', 'CSMP',
};

static const uint32 kNumAssetTypes = sizeof(kAssetTypes) / sizeof(kAssetTypes[0]);

unsigned int CAssetTypesList::GetFourCCForIndex(unsigned int index) {
  RS_VERIFY_THROW(419, ((uint32)index) < kNumAssetTypes, false,
                  "Invalid index into asset types list in CAssetTypesList::GetFourCCForIndex.");
  return kAssetTypes[index];
}
