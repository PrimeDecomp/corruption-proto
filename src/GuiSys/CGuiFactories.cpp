/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x80482468..0x804829D8; 6 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CGuiFactories.hpp"

#include "GuiSys/CAuiBitmapMeter.hpp"
#include "GuiSys/CAuiEnergyBarT01.hpp"
#include "GuiSys/CAuiImagePane.hpp"
#include "GuiSys/CAuiMeter.hpp"
#include "GuiSys/CGuiCamera.hpp"
#include "GuiSys/CGuiHeadWidget.hpp"
#include "GuiSys/CGuiLight.hpp"
#include "GuiSys/CGuiModel.hpp"
#include "GuiSys/CGuiPane.hpp"
#include "GuiSys/CGuiSliderGroup.hpp"
#include "GuiSys/CGuiTableGroup.hpp"
#include "GuiSys/CGuiTextPane.hpp"
#include "GuiSys/CGuiWidget.hpp"

CGuiWidget* FGuiWidgetFactoryInGame(FourCC type, CGuiFrame* frame, CInputStream& in,
                                    CSimplePool* pool, uint version) {
  switch (type) {
  case 'HWIG':
    return CGuiHeadWidget::Create(frame, in, pool, version);
  case 'BWIG':
    return CGuiWidget::Create(frame, in, pool, version);
  case 'CAMR':
    return CGuiCamera::Create(frame, in, pool, version);
  case 'GRUP':
    return CGuiWidget::CreateGroup(frame, in, pool, version);
  case 'MODL':
    return CGuiModel::Create(frame, in, version);
  case 'SLGP':
    return CGuiSliderGroup::Create(frame, in, pool, version);
  case 'TBGP':
    return CGuiTableGroup::Create(frame, in, pool, version);
  case 'PANE':
    return CGuiPane::Create(frame, in, pool, version);
  case 'TXPN':
    return CGuiTextPane::Create(frame, in, pool, version);
  case 'LITE':
    return CGuiLight::Create(frame, in, pool, version);
  case 'ENRG':
    return CAuiEnergyBarT01::Create(frame, in, pool, version);
  case 'METR':
    return CAuiMeter::Create(frame, in, pool, version);
  case 'IMGP':
    return CAuiImagePane::Create(frame, in, pool, version);
  case 'BMTR':
    return CAuiBitmapMeter::Create(frame, in, pool, version);
  default:
    return nullptr;
  }
}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern unsigned char lbl_806B1B80[12];
extern unsigned char lbl_806B1B8C[12];
extern unsigned char lbl_806CE2A0[16];
extern "C" void fn_80483970(int, int);
extern "C" int fn_8048286C(int obj, int val);
extern "C" int fn_8048286C(int obj, int val) {
    if (obj) {
        *(int*)obj = (int)lbl_806CE2A0;
        if ((unsigned int)*(int*)((char*)obj + 0x4) != 0) {
            fn_80483970(*(int*)((char*)obj + 0x4), 1);
        }
        if ((unsigned int)obj != 0) {
            *(int*)obj = (int)lbl_806B1B8C;
            if ((unsigned int)obj != 0) {
                *(int*)obj = (int)lbl_806B1B80;
            }
        }
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

