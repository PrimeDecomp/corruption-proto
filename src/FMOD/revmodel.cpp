// G2MEAB prototype NonMatching translation-unit scaffold; function bodies are empty placeholders.
// .text: 0x806259D8..0x806262F4 (16 native functions).
// Split out of the former fmod_reverb_model scaffold; original basename from the 4.06 reference library object.
// 0x806259D8 +0x310: stereo reverb model constructor with embedded delay buffers
// 0x80625CE8 +0x9C: retained native; no unsupported symbol identity assigned
// 0x80625D84 +0x30C: stereo reverb model processing
// 0x80626090 +0x10C: reverb coefficient update
// 0x8062619C +0x34: retained native; no unsupported symbol identity assigned
// 0x806261D0 +0x18: retained native; no unsupported symbol identity assigned
// 0x806261E8 +0x2C: retained native; no unsupported symbol identity assigned
// 0x80626214 +0x10: retained native; no unsupported symbol identity assigned
// 0x80626224 +0x2C: retained native; no unsupported symbol identity assigned
// 0x80626250 +0x10: retained native; no unsupported symbol identity assigned
// 0x80626260 +0x10: retained native; no unsupported symbol identity assigned
// 0x80626270 +0x10: retained native; no unsupported symbol identity assigned
// 0x80626280 +0x24: retained native; no unsupported symbol identity assigned
// 0x806262A4 +0x8: retained native; no unsupported symbol identity assigned
// 0x806262AC +0x24: retained native; no unsupported symbol identity assigned
// 0x806262D0 +0x24: retained native; no unsupported symbol identity assigned

// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#include "revmodel.h"

void revmodel::setdry(float value)
{
}

float revmodel::getmode()
{
}

void revmodel::mute()
{
}

void revmodel::processreplace(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip)
{
}

void revmodel::processmix(float * inputL, float * inputR, float * outputL, float * outputR, long numsamples, int skip)
{
}

void revmodel::update()
{
}

void revmodel::setmode(float value)
{
}

void revmodel::setwidth(float value)
{
}

void revmodel::setwet(float value)
{
}

void revmodel::setdamp(float value)
{
}

void revmodel::setroomsize(float value)
{
}

revmodel::revmodel()
{
}

float revmodel::getroomsize()
{
}

float revmodel::getdamp()
{
}

float revmodel::getwet()
{
}

float revmodel::getdry()
{
}

float revmodel::getwidth()
{
}
