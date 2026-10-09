/*
 * G2MEAB CIOWin.cpp translation-unit scaffold (NonMatching).
 * .text: 0x80034A5C..0x80034B04 (2 retained native functions).
 * Both-reference source identity; target constructor/destructor and vtable closure.
 * Original object extent inferred; implementation and data ownership unreconstructed.
 */
// The trivial GetName/GetIsContinueDraw/Draw/PreDraw bodies (0x80034A44..0x80034A5C) sit just
// before this range, inside the configured CIOWinManager.cpp split; here they are header inlines.
#include "MetroidPrime/CIOWin.hpp"

CIOWin::CIOWin(const rstl::string& name) : mName(name) {}

CIOWin::~CIOWin() {}
