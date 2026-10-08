/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x80482334..0x80482468; 4 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CGuiCompoundWidget.hpp"

CGuiCompoundWidget::CGuiCompoundWidget(const CGuiWidgetParms& parms) : CGuiWidget(parms) {}

void CGuiCompoundWidget::OnActivate() {
  CGuiWidget* widget = static_cast< CGuiWidget* >(ChildObject());
  while (widget != nullptr) {
    widget->SetIsActive(GetIsActive());
    widget = static_cast< CGuiWidget* >(widget->NextSibling());
  }

  CGuiWidget::OnActivate();
}

void CGuiCompoundWidget::OnVisible() {
  CGuiWidget* widget = static_cast< CGuiWidget* >(ChildObject());
  while (widget != nullptr) {
    widget->SetIsVisible(GetIsVisible());
    widget = static_cast< CGuiWidget* >(widget->NextSibling());
  }

  CGuiWidget::OnVisible();
}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" int fn_80482460();
extern "C" int fn_80482460() {
    return -1;
}

