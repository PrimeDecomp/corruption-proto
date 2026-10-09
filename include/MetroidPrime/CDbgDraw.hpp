#ifndef _CDBGDRAW
#define _CDBGDRAW

#include "types.h"

#include "rstl/list.hpp"

class CStateManager;

// Minimal declaration. The class name comes from CDbgDraw.cpp. CGameGlobalObjects owns two
// instances (at 0xA2C8 and 0xA35C) and publishes them at 0x80797110 and 0x80797114; each keeps
// timed debug primitives in six lists, one per primitive kind.
class CDbgDraw {
public:
  // Guessed names. Only the offset of each kind's remaining time is established (from Update);
  // the primitive data and the element sizes are not modelled yet.
  struct SPrimitive0 {
    float mTimeLeft;
  };
  struct SPrimitive1 {
    uchar x0_[0x2c];
    float mTimeLeft;
  };
  struct SPrimitive2 {
    uchar x0_[0x28];
    float mTimeLeft;
  };
  struct SPrimitive3 {
    uchar x0_[0x20];
    float mTimeLeft;
  };
  struct SPrimitive4 {
    uchar x0_[0x3c];
    float mTimeLeft;
  };
  struct SPrimitive5 {
    uchar x0_[0x24];
    float mTimeLeft;
  };

  // 0x80030284. CGameGlobalObjects builds one instance with false and one with true.
  explicit CDbgDraw(bool persistent);

  // Guessed name. 0x8002EF58 ages the timed primitives by the frame time and drops the expired
  // ones, unless the instance is persistent; CStateManager's update calls it while the game runs.
  void Update(float dt, CStateManager& mgr);
  // Prime's name (DrawDebugStuff's callee in Prime's map). 0x8002EC74 draws the primitives;
  // CRenderManager's debug draw calls it on gpDbgDraw, then on gpPersistentDbgDraw.
  void Draw();

private:
  bool mPersistent; // Guessed name
  rstl::list< SPrimitive0 > mPrimitives0;
  rstl::list< SPrimitive1 > mPrimitives1;
  rstl::list< SPrimitive2 > mPrimitives2;
  rstl::list< SPrimitive3 > mPrimitives3;
  rstl::list< SPrimitive4 > mPrimitives4;
  rstl::list< SPrimitive5 > mPrimitives5;
};
CHECK_SIZEOF(CDbgDraw, 0x94)

extern CDbgDraw* gpDbgDraw;           // Guessed name
extern CDbgDraw* gpPersistentDbgDraw; // Guessed name

#endif // _CDBGDRAW
