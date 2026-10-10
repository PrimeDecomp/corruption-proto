#ifndef _INPUTTYPES
#define _INPUTTYPES

#include <dolphin/pad.h>

enum EIOPort {
  kIOP_Player1 = PAD_CHAN0,
  kIOP_Player2 = PAD_CHAN1,
  kIOP_Player3 = PAD_CHAN2,
  kIOP_Player4 = PAD_CHAN3,
};

enum EMotorState {
  kMS_Stop = PAD_MOTOR_STOP,
  kMS_Rumble = PAD_MOTOR_RUMBLE,
  kMS_StopHard = PAD_MOTOR_STOP_HARD,
};

// G2MEAB's ten axes. CRevolutionController feeds 0-3 from the nunchuk stick and the pointer
// (or a GameCube pad's C-stick) and 6-9 from a GameCube pad's sticks; nothing in it feeds 4-5.
enum EJoyAxis {
  kJA_LeftX,
  kJA_LeftY,
  kJA_RightX,
  kJA_RightY,
  kJA_Axis4,          // Guessed name
  kJA_Axis5,          // Guessed name
  kJA_GcLeftX,        // Guessed name
  kJA_GcLeftY,        // Guessed name
  kJA_GcRightX,       // Guessed name
  kJA_GcRightY,       // Guessed name
  kJA_MAX,
};

// G2MEAB's 51 buttons, numbered by CRevolutionController's KPAD hold-mask table and
// CFinalInput::GetDigitalValue. Core and nunchuk buttons are named by their wpadEmu hold bit;
// GameCube buttons by the SI report bit that WPAD's Dolphin format maps to that hold bit.
// Stick directions follow GetDigitalValue (Up/Right read the positive axis value).
enum EButton {
  kBU_None,             // Guessed name
  kBU_Core0001,         // Guessed name
  kBU_Core0002,         // Guessed name
  kBU_Core0004,         // Guessed name
  kBU_Core0008,         // Guessed name
  kBU_Core0010,         // Guessed name
  kBU_Core0020,         // Guessed name
  kBU_Core0100,         // Guessed name
  kBU_Core0200,         // Guessed name
  kBU_Core0400,         // Guessed name
  kBU_Core0800,         // Guessed name
  kBU_Core1000,         // Guessed name
  kBU_LeftStickUp,      // Guessed name
  kBU_LeftStickDown,    // Guessed name
  kBU_LeftStickRight,   // Guessed name
  kBU_LeftStickLeft,    // Guessed name
  kBU_Nunchuk2000,      // Guessed name
  kBU_Nunchuk4000,      // Guessed name
  kBU_RightStickUp,     // Guessed name
  kBU_RightStickDown,   // Guessed name
  kBU_RightStickRight,  // Guessed name
  kBU_RightStickLeft,   // Guessed name
  kBU_A,
  kBU_B,
  kBU_X,
  kBU_Y,
  kBU_Start,
  kBU_Z,
  kBU_L,
  kBU_R,
  kBU_Up,
  kBU_Right,
  kBU_Down,
  kBU_Left,
  kBU_LTrigger,         // Guessed name
  kBU_RTrigger,         // Guessed name
  kBU_GcLeftStickUp,    // Guessed name
  kBU_GcLeftStickDown,  // Guessed name
  kBU_GcLeftStickRight, // Guessed name
  kBU_GcLeftStickLeft,  // Guessed name
  kBU_GcRightStickUp,   // Guessed name
  kBU_GcRightStickDown, // Guessed name
  kBU_GcRightStickRight, // Guessed name
  kBU_GcRightStickLeft, // Guessed name
  kBU_Axis5Up,          // Guessed name
  kBU_Axis5Down,        // Guessed name
  kBU_Axis4Right,       // Guessed name
  kBU_Axis4Left,        // Guessed name
  kBU_Unused48,         // Guessed name
  kBU_Unused49,         // Guessed name
  kBU_Unused50,         // Guessed name
  kBU_MAX,
};

enum EAnalogButton { kBA_Left, kBA_Right, kBA_MAX };

#endif // _INPUTTYPES
