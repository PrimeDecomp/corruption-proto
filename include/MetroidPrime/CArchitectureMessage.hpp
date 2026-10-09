#ifndef _CARCHITECTUREMESSAGE
#define _CARCHITECTUREMESSAGE

#include "types.h"

#include "rstl/rc_ptr.hpp"

enum EArchMsgTarget {
  kAMT_IOWinManager,
  kAMT_Game,
};

enum EArchMsgType {
  kAM_RemoveIOWin = 0,
  kAM_CreateIOWin = 1,
  kAM_ChangeIOWinPriority = 2,
  kAM_RemoveAllIOWins = 3,
  kAM_TimerTick = 4,
  kAM_UserInput = 5,
  kAM_SetGameState = 6,
  kAM_ControllerStatus = 7,
  kAM_QuitGameplay = 8,
  kAM_FrameBegin = 10,
  kAM_FrameEnd = 11,
};

struct IArchitectureMessageParm {
  virtual ~IArchitectureMessageParm() {}
};

// Minimal declaration; the timer tick parameter (Decode.cpp).
class CArchMsgParmReal32 : public IArchitectureMessageParm {
public:
  float GetReal() const { return mVal; }

private:
  float mVal;
};

class CArchitectureMessage {
public:
  CArchitectureMessage(EArchMsgTarget target, int type,
                       const rstl::rc_ptr< IArchitectureMessageParm >& parm)
  : mTarget(target), mType(static_cast< EArchMsgType >(type)), mParm(parm) {}

  EArchMsgType GetType() const { return mType; }
  const IArchitectureMessageParm* GetParm() const { return mParm.GetPtr(); }
  EArchMsgTarget GetTarget() const { return mTarget; }

private:
  EArchMsgTarget mTarget;
  EArchMsgType mType;
  rstl::rc_ptr< IArchitectureMessageParm > mParm;
};

namespace MakeMsg {
const CArchMsgParmReal32& GetParmTimerTick(const CArchitectureMessage& msg);
// Echoes names; the message types (4, 0xA, 0xB) confirm them (Decode.cpp).
CArchitectureMessage CreateTimerTick(EArchMsgTarget target, const float& dt);
CArchitectureMessage CreateFrameBegin(EArchMsgTarget target, const int& frame);
CArchitectureMessage CreateFrameEnd(EArchMsgTarget target, const int& frame);
} // namespace MakeMsg

#endif // _CARCHITECTUREMESSAGE
