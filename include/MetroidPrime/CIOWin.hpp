#ifndef _CIOWIN
#define _CIOWIN

#include "types.h"

#include "rstl/string.hpp"

class CArchitectureMessage;
class CArchitectureQueue;

class CIOWin {
public:
  enum EMessageReturn {
    kMR_Normal = 0,
    kMR_Exit = 1,
    kMR_RemoveIOWinAndExit = 2,
    kMR_RemoveIOWin = 3,
  };

  CIOWin(const rstl::string& name);
  virtual ~CIOWin();

  // G2MEAB: the CIOWin vtable (0x806B2270) leaves the OnMessage slot empty, so unlike Echoes there
  // is no CIOWin::OnMessage body. The trivial accessors below are emitted at
  // 0x80034A44..0x80034A5C.
  const rstl::string& GetName() const { return mName; }
  virtual EMessageReturn OnMessage(const CArchitectureMessage&, CArchitectureQueue&) = 0;
  virtual bool GetIsContinueDraw() const { return true; }
  virtual void Draw() const {}
  virtual void PreDraw() const {}

private:
  rstl::string mName;
};
CHECK_SIZEOF(CIOWin, 0x14)

#endif // _CIOWIN
