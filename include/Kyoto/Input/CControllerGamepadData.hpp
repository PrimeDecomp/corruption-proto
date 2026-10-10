#ifndef _CCONTROLLERGAMEPADDATA
#define _CCONTROLLERGAMEPADDATA

#include "Kyoto/Input/CControllerAxis.hpp"
#include "Kyoto/Input/CControllerButton.hpp"
#include "Kyoto/Input/InputTypes.hpp"

#include "types.h"

class CControllerGamepadData {
public:
  void SetDeviceIsPresent(bool present) { mPresent = present; }
  bool DeviceIsPresent() const { return mPresent; }

  void SetDeviceJustDisconnected(bool disconnected) { mJustDisconnected = disconnected; }
  bool DeviceJustDisconnected() const { return mJustDisconnected; }

  const CControllerAxis& GetAxis(EJoyAxis axis) const { return mAxes[axis]; }
  CControllerAxis& GetAxis(EJoyAxis axis) { return mAxes[axis]; }

  const CControllerButton& GetButton(EButton button) const { return mButtons[button]; }
  CControllerButton& GetButton(EButton button) { return mButtons[button]; }

  const CControllerAxis& GetAnalogButton(EAnalogButton button) const { return mTriggers[button]; }
  CControllerAxis& GetAnalogButton(EAnalogButton button) { return mTriggers[button]; }

private:
  bool mPresent;
  bool mJustDisconnected;
  CControllerAxis mAxes[kJA_MAX];
  CControllerAxis mTriggers[kBA_MAX];
  CControllerButton mButtons[kBU_MAX];
};

CHECK_SIZEOF(CControllerGamepadData, 0x100);

#endif // _CCONTROLLERGAMEPADDATA
