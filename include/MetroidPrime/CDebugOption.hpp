#ifndef _CDEBUGOPTION
#define _CDEBUGOPTION

#include "types.h"

#include "Kyoto/Graphics/CColor.hpp"

#include "rstl/string.hpp"
#include "rstl/vector.hpp"

// One option of the in-game debug menu that CGameDebug builds ("Show Framerate", "Bloom",
// "PVS", "Wireframe", ...). The class name comes from the "CDebugOption.cpp(NN) : " allocation
// strings. An option holds a float value with a range and a step; boolean options use 0..1.
// Options with named values ("OFF"/"ON"/"DEBUG", "Enabled"/"Reversed", ...) keep a table of
// choices, and changes to the value are broadcast through a TSignal1 (TSignal1.h asserts in the
// connector emitted into this unit). No Echoes/Prime equivalent is known.
class CDebugOption {
public:
  // Guessed name. A named value of an option ("OFF", "ON", ...), 0x14 bytes.
  class SChoice {
  public:
    SChoice(const rstl::string& name, float value);

    // Guessed names. Both are emitted out of line (0x800F9BD8, 0x800F9BE0).
    float GetValue() const;
    const rstl::string& GetName() const;

  private:
    rstl::string mName;
    float mValue;
  };

  // Line 55 (bool options: range 0..1, step 1) and line 34 (float options). The two leading
  // integers are small constants supplied by CGameDebug (for example 0x19 and 0xFC); their
  // meaning is unknown.
  CDebugOption(int, int, const rstl::string& name, bool value, const CColor& color);
  CDebugOption(int, int, const rstl::string& name, float value, float min, float max, float step,
               const CColor& color);
  ~CDebugOption();

  // Guessed names.
  void SetValue(float value);
  const rstl::string* GetChoiceName(float value);
  void AddChoice(const rstl::string& name, float value);

  // Guessed names, used by main.cpp. ClearMessages is emitted there (0x80005BF0) and empties the
  // string vector at 0x38 once per frame.
  float GetValue() const { return mValue; }
  void ClearMessages() { x38_.clear(); }

private:
  int x0_;
  int x4_;
  rstl::string mName;
  float mValue;
  float mMin;
  float mMax;
  float mStep;
  CColor mColor;
  // rstl::rc_ptr to the TSignal1 notified by SetValue; its release (0x80048DDC) and the signal
  // type are emitted in CGameDebug, and no TSignal header exists yet.
  uchar x2c_signal[8];
  float x34_; // Always 0.9f
  rstl::vector< rstl::string > x38_;
  rstl::vector< SChoice* >* mChoices; // Allocated by the first AddChoice
};
CHECK_SIZEOF(CDebugOption, 0x4c)

#endif // _CDEBUGOPTION
