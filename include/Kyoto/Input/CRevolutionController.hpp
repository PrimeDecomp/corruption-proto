#ifndef _CREVOLUTIONCONTROLLER
#define _CREVOLUTIONCONTROLLER

#include "Kyoto/Input/IController.hpp"
#include "Kyoto/Math/CVector2f.hpp"

#include "rstl/reserved_vector.hpp"

#include <revolution/kpad.h>

namespace rstl {
RSTL_DECLARE_TRIVIALLY_CONSTRUCTIBLE(EMotorState)
} // namespace rstl

// Controller type FourCCs, in CRevolutionController.cpp's .sdata2 order.
extern const uint kControllerTypeUnknown; // Guessed name
extern const uint kControllerTypeCube;    // Guessed name
extern const uint kControllerTypeRevn;    // Guessed name
extern const uint kControllerTypeChak;    // Guessed name

// Reads up to four Revolution controllers through the wpadEmu KPAD library.
class CRevolutionController : public IController {
public:
  CRevolutionController();
  ~CRevolutionController() override;
  void Poll() override;
  uint GetDeviceCount() const override;
  CControllerGamepadData& GetGamepadData(int controller) override;
  uint GetControllerType(int controller) const override;
  void SetMotorState(EIOPort port, EMotorState state) override;
  int GetTrackingState(int controller) const override;
  uint GetConsecutiveValidTrackingFrames(int controller) const override;
  uint GetConsecutiveInvalidTrackingFrames(int controller) const override;
  CVector2f GetTrackedPosition(int controller) const override;

  bool Initialize();
  float GetAnalogStickMaxValue(EJoyAxis axis) const;

private:
  void ProcessInputData();
  void AdjustTrackedPositionScale(int controller); // Guessed name
  void UpdateTrackedPosition(int controller);      // Guessed name
  void ProcessAxis(int controller, EJoyAxis axis, CControllerButton& negative,
                   CControllerButton& positive);
  void ProcessButtons(int controller);
  void ProcessDigitalButton(int controller, CControllerButton& button, uint mask);
  void ProcessAnalogButton(float value, CControllerAxis& axis, CControllerButton& button);

  rstl::reserved_vector< KPADStatus, 4 > mStatus;
  rstl::reserved_vector< CControllerGamepadData, 4 > mGamepadStates;
  rstl::reserved_vector< EMotorState, 4 > mMotorStates;
  rstl::reserved_vector< uint, 4 > mDeviceTypes;                 // Guessed name
  rstl::reserved_vector< int, 4 > mTrackingStates;               // Guessed name
  rstl::reserved_vector< CVector2f, 4 > mTrackedPositions;       // Guessed name
  rstl::reserved_vector< float, 4 > mTrackedDistances;           // Guessed name
  rstl::reserved_vector< uint, 4 > mConsecutiveValidFrames;      // Guessed name
  rstl::reserved_vector< uint, 4 > mConsecutiveInvalidFrames;    // Guessed name
};

CHECK_SIZEOF(CRevolutionController, 0x688);

#endif // _CREVOLUTIONCONTROLLER
