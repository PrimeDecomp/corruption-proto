#include "Kyoto/Audio/CAudioSys.hpp"
#include "Kyoto/Audio/CAIInterruptManager.hpp"
#include "Kyoto/Basics/CInterruptGuard.hpp"
#include "rstl/algorithm.hpp"
#include "rstl/reserved_vector.hpp"
#include <dolphin/ai.h>

static rstl::reserved_vector< FAudioCallback, 4 > sCallbacks;
static bool sDMACallbackInstalled ATTRIBUTE_ALIGN(8) = false;
static FAudioCallback sOldDMACallback = nullptr;

void CAIInterruptManager::InstallAICallback() {
  bool old = CAudioSys::IsAICallbackEnabled();
  CAudioSys::EnableAICallback(true);

  if (!sDMACallbackInstalled && sCallbacks.size() != 0) {
    sOldDMACallback = AIRegisterDMACallback(AICallback);
    sDMACallbackInstalled = true;
  } else if (sDMACallbackInstalled && sCallbacks.size() == 0) {
    AIRegisterDMACallback(sOldDMACallback);
    sOldDMACallback = 0;
    sDMACallbackInstalled = false;
  }

  CAudioSys::EnableAICallback(old);
}

void CAIInterruptManager::AICallback() {
  sOldDMACallback();

  for (int i = 0; i < sCallbacks.size(); ++i) {
    sCallbacks[i]();
  }
}

void CAIInterruptManager::RunDMACallback(const FAudioCallback callback) {
  CInterruptGuard interrupts;
  const rstl::reserved_vector< FAudioCallback, 4 >::iterator it =
      rstl::find(sCallbacks.begin(), sCallbacks.end(), callback);
  if (it == sCallbacks.end()) {
    sCallbacks.push_back(callback);
  }

  InstallAICallback();
}

void CAIInterruptManager::CancelDMACallback(FAudioCallback callback) {
  CInterruptGuard interrupts;

  const rstl::reserved_vector< FAudioCallback, 4 >::iterator it =
      rstl::find(sCallbacks.begin(), sCallbacks.end(), callback);
  if (it != sCallbacks.end()) {
    sCallbacks.erase(it);
  }

  InstallAICallback();
}
