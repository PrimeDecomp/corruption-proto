/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x80484540..0x80484644; 4 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CGuiHeadWidget.hpp"

#include "GuiSys/CGuiFrame.hpp"

CGuiWidget* CGuiHeadWidget::Create(CGuiFrame* frame, CInputStream& in, CSimplePool* pool,
                                   uint version) {
  CGuiWidgetParms parms = ReadWidgetHeader(frame, in);
  CGuiHeadWidget* widget = rs_new CGuiHeadWidget(parms);
  frame->SetHeadWidget(widget);
  widget->ParseBaseInfo(frame, in, parms, version);
  return widget;
}

CGuiHeadWidget::CGuiHeadWidget(const CGuiWidgetParms& parms) : CGuiWidget(parms) {}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" bool fn_8048454C();
extern "C" bool fn_8048454C() {
    return false;
}

