#ifndef _CERROROUTPUTWINDOW
#define _CERROROUTPUTWINDOW

#include "types.h"

#include "MetroidPrime/CIOWin.hpp"

class CErrorOutputWindow : public CIOWin {
public:
  enum EFlag {
    kF_Zero,
    kF_One,
  };

  CErrorOutputWindow(EFlag);
  ~CErrorOutputWindow() override {}

  EMessageReturn OnMessage(const CArchitectureMessage&, CArchitectureQueue&) override;
  bool GetIsContinueDraw() const override;
  void Draw() const override;

  void UpdateWindow();

private:
  enum EState {
    kS_Zero,
    kS_One,
  };

  void SetState(EState);
  void DrawError() const;

  EState mState;
  bool x18_24_ : 1;            // CMoviePlayer audio state saved while the error is shown
  bool x18_25_ : 1;            // streamed music unmute state saved while the error is shown
  bool x18_26_ : 1;            // streamed sfx unmute state saved while the error is shown
  bool x18_27_ : 1;            // set for kF_Zero: mute audio while the error is shown
  const wchar_t* mErrorString; // "mErrorString != NULL" assert text
};
CHECK_SIZEOF(CErrorOutputWindow, 0x20)

#endif // _CERROROUTPUTWINDOW
