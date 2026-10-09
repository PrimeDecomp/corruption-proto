#ifndef _SCREENCAPTURE
#define _SCREENCAPTURE

#include "types.h"

#include "rstl/auto_ptr.hpp"

// Prototype-only debug screen capture (ScreenCapture.cpp, asserted name). Minimal: only what
// main.cpp uses.

// Guessed name. Reads Key.Num over the broadband adapter, scrambles the frame (the copied pixels,
// or the EFB when null) and writes it to the host; the flag picks the movie-frame file name.
void DumpScreenShot(bool movieFrame, const uchar* pixels); // 0x8020ACA4

// Guessed name. Owns the movie-capture frame buffer between frames: the buffer at 0x4 and the
// frame counter at 0x8.
struct SScreenshotState {
  SScreenshotState() : x8_frameCount(0) {}

  // Guessed names. 0x8020B258 captures the EFB into the buffer (allocated by 0x8020B534) and
  // writes it out once the capture finishes; 0x8020B4D0 tests whether there is no buffer;
  // 0x8020B4E0 frees it and resets the counter.
  void UpdateMovieCapture(bool finished);
  bool IsEmpty() const;
  void Clear();

  rstl::auto_ptr< uchar > x0_buffer;
  uint x8_frameCount;
};

#endif // _SCREENCAPTURE
