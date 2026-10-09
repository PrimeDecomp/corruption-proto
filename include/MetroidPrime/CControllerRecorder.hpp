#ifndef _CCONTROLLERRECORDER
#define _CCONTROLLERRECORDER

#include "types.h"

#include "Kyoto/SObjectTag.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

class CStateManager;

// Minimal view of the prototype's demo/controller recorder (CControllerRecorder.cpp,
// 0x80120D74..0x8012478C), embedded in CInputGenerator at 0x10. Neither Echoes nor Prime has
// it. Only what the architecture tick reads is placed; the rest is padding. Guessed names: the
// recorder prints "Game Speed %.2f", and the tick runs exactly one normal frame when the step
// flag is set, then clears it.
class CControllerRecorder {
public:
  float GetGameSpeed() const { return mGameSpeed; }
  bool GetStepFrame() const { return mStepFrame; }
  void ClearStepFrame(); // 0x8012124C
  // Guessed names. The console's ADVANCEFRAME sets the step flag (0x80121260) and GAMESPEED sets
  // the speed, printing "Game Speed %.2f" when it changes (0x80121680).
  void SetStepFrame();
  void SetGameSpeed(float speed);
  // Guessed name. 0x80121450: switches the console window, the video filter and the game speed
  // for a movie capture and back, and starts the capture; the console's CAPTUREMOVIE passes true.
  void SetCaptureMode(bool capture);

  // Guessed name. 0x801239B8 builds a capture name from the state manager (0x801236F8) and
  // hands it to CGameDebug (0x8003C9B0); CStateManager's update calls it while a movie capture
  // runs without a name.
  static void SetMovieCaptureName(CStateManager& mgr);

  // Guessed name. Set by the recorder's profiling switch (0x80121A14); main keeps the GX
  // performance counters running while it is set.
  static bool sProfiling;
  // Guessed names. Store the latest CPU and GPU draw time (0x80121274, 0x80121280) in the
  // profiling statistics the recorder prints as "CpuDrawTime..." and "GpuDrawTime...". The
  // statistics hold doubles; whether the parameter is float or double is not provable.
  static void SetCpuDrawTime(double time);
  static void SetGpuDrawTime(double time);

  // Guessed names. A demo file found on the host (0x78 bytes): its file name, the header read
  // from the file and the length in seconds computed from the file size. CGameDebug's "Demo" page
  // lists them as "<file> <world type> <x10> <x28> <length>s".
  struct SDemoInfo {
    rstl::string mFileName;
    rstl::string x10_;
    int x20_;
    int x24_;
    rstl::string x28_;
    int x38_;
    int x3c_;
    int x40_;
    FourCC x44_type; // Printed with SObjectTag::Type2Text
    int x48_;
    int x4c_;
    rstl::string x50_;
    rstl::string x60_;
    float mLength;
    bool x74_valid;
  };
  // Guessed names. 0x80122F98 reads the headers of the "Demo%03d.dat" files on the host;
  // 0x80122E14 returns the first unused "Demo%03d.dat" name, or an empty string when all 32 are
  // taken or the BBA server is not running.
  static rstl::vector< SDemoInfo > GetDemoList();
  static rstl::string GetNextDemoFileName();

private:
  uchar x0_[0x30];
  float mGameSpeed;
  bool mStepFrame : 1;
  uchar x35_[0xb];
};
CHECK_SIZEOF(CControllerRecorder, 0x40)

#endif // _CCONTROLLERRECORDER
