#ifndef _REVOLUTION_KPAD_H_
#define _REVOLUTION_KPAD_H_

// Revolution SDK KPAD (KPAD.c), Nov 28 2005 build. Member and function names come from
// the matching RELSAB debug build's DWARF, which records these structures anonymously;
// the typedef names follow SDK convention.

#include <dolphin/types.h>
#include <revolution/wpad.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Vec2 {
  f32 x;
  f32 y;
} Vec2;

typedef struct KPADVec {
  f32 x;
  f32 y;
  f32 z;
} KPADVec;

typedef union KPADEXStatus {
  struct {
    Vec2 stick;
    KPADVec acc;
    f32 acc_value;
    f32 acc_speed;
  } fs;

  struct {
    Vec2 stick;
    Vec2 substick;
    f32 ltrigger;
    f32 rtrigger;
  } gc;
} KPADEXStatus;

typedef struct KPADStatus {
  u32 hold;
  u32 trig;
  u32 release;
  KPADVec acc;
  f32 acc_value;
  f32 acc_speed;
  Vec2 pos;
  Vec2 vec;
  f32 speed;
  Vec2 horizon;
  Vec2 hori_vec;
  f32 hori_speed;
  f32 dist;
  f32 dist_vec;
  f32 dist_speed;
  u32 dev_type;
  KPADEXStatus ex_status;
  s8 dpd_valid_fg;
  s8 wpad_err;
} KPADStatus;

void KPADInit(void);
void KPADReset(void);
s32 KPADRead(s32 chan, KPADStatus* samplingBufs, u32 length);
void KPADSetObjInterval(f32 interval);
void KPADSetPosParam(s32 chan, f32 play_radius, f32 sensitivity);
void KPADSetHoriParam(s32 chan, f32 play_radius, f32 sensitivity);
void KPADSetDistParam(s32 chan, f32 play_radius, f32 sensitivity);
void KPADSetAccParam(s32 chan, f32 play_radius, f32 sensitivity);
void KPADSetBtnRepeat(s32 chan, f32 delay_sec, f32 pulse_sec);
s32 KPADCalibrateDPD(s32 chan);
WPADStatus* KPADGetWPADRingBuffer(s32 chan);
WPADFSStatus* KPADGetWPADFSRingBuffer(s32 chan);

#ifdef __cplusplus
}
#endif

#endif // _REVOLUTION_KPAD_H_
