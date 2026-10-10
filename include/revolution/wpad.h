#ifndef _REVOLUTION_WPAD_H_
#define _REVOLUTION_WPAD_H_

// Revolution SDK wpadEmu (wpadv3.c), Nov 25 2005 build. Structure, member and function
// names come from the matching RELSAB debug build's DWARF. Constant names are guessed;
// their values come from G2MEAB's WPAD code and CRevolutionController's diagnostics.

#include <dolphin/pad.h>
#include <dolphin/types.h>

#ifdef __cplusplus
extern "C" {
#endif

// Guessed names: WPADProbe results
#define WPAD_ERR_NONE 0
#define WPAD_ERR_NO_CONTROLLER -1
#define WPAD_ERR_BUSY -2
#define WPAD_ERR_TRANSFER -3
#define WPAD_ERR_INVALID -4

// Guessed names: WPADProbe device types
#define WPAD_DEV_CORE 1
#define WPAD_DEV_FREESTYLE 2
#define WPAD_DEV_DOLPHIN 3
#define WPAD_DEV_UNKNOWN 255

// Guessed names: WPADSetDataFormat formats
#define WPAD_FMT_CORE 0
#define WPAD_FMT_FREESTYLE 1
#define WPAD_FMT_DOLPHIN 2

// Guessed names: WPADControlMotor commands
#define WPAD_MOTOR_STOP 0
#define WPAD_MOTOR_RUMBLE 1

typedef struct DPDObject {
  s16 x;
  s16 y;
  u16 size;
  u8 traceId;
} DPDObject;

typedef struct WPADStatus {
  u16 button;
  s16 accX;
  s16 accY;
  s16 accZ;
  DPDObject obj[4];
  u8 dev;
  s8 err;
} WPADStatus;

typedef struct WPADFSStatus {
  u16 button;
  s16 accX;
  s16 accY;
  s16 accZ;
  DPDObject obj[4];
  s16 fsAccX;
  s16 fsAccY;
  s16 fsAccZ;
  s8 fsStickX;
  s8 fsStickY;
  u8 dev;
  s8 err;
} WPADFSStatus;

typedef void (*WPADCallback)(s32 chan);

void WPADInit(void);
void WPADSetDataFormat(s32 chan, u32 fmt);
void WPADControlMotor(s32 chan, u32 command);
void WPADSetAutoSamplingBuf(s32 chan, void* buf, u32 length);
u32 WPADGetLatestIndexInBuf(s32 chan);
void WPADRead(s32 chan, void* status);
s32 WPADProbe(s32 chan, u32* type);
// Guessed name: absent from the dead-stripped debug link; captures the current
// stick/trigger input as the calibration origin.
s32 WPADRecalibrate(s32 chan);

#ifdef __cplusplus
}
#endif

#endif // _REVOLUTION_WPAD_H_
