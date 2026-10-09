#ifndef _CINPUTGENERATOR
#define _CINPUTGENERATOR

#include "types.h"

#include "MetroidPrime/CControllerRecorder.hpp"

class COsContext;
class CArchitectureQueue;

// Minimal view of the prototype's CInputGenerator (CInputGenerator.cpp, 0x8002339C..0x80023698).
// Echoes names and constructor arguments; the prototype embeds a controller recorder at 0x10
// (0x50 bytes instead of 0x14).
class CInputGenerator {
public:
  CInputGenerator(COsContext* context, float leftDiv, float rightDiv); // 0x80023614
  ~CInputGenerator();                                                  // 0x800235BC
  bool Update(float dt, CArchitectureQueue& queue);                    // 0x8002339C

  CControllerRecorder& GetRecorder() { return mRecorder; } // Guessed name

private:
  COsContext* mContext;
  bool x4_;
  bool x5_;
  bool x6_;
  bool x7_;
  float mLeftDiv;
  float mRightDiv;
  CControllerRecorder mRecorder;
};
CHECK_SIZEOF(CInputGenerator, 0x50)

#endif // _CINPUTGENERATOR
