/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x80482468..0x804829D8; 6 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CGuiFactories.hpp"

#include "GuiSys/CGuiFrame.hpp"
#include "Kyoto/CFactoryMgr.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/CVParamTransfer.hpp"
#include "Kyoto/Streams/CMemoryInStream.hpp"

#include "GuiSys/CAuiBarMeter.hpp"
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
    return CAuiBarMeter::Create(frame, in, pool, version);
  default:
    return nullptr;
  }
}

CFactoryFnReturn RGuiFrameFactoryInGame(const SObjectTag& tag, const rstl::auto_ptr< uchar >& buffer,
                                        int size, const CVParamTransfer& xfer) {
  CMemoryInStream in(buffer.get(), size);
  const rstl::rc_ptr< IVParamObj > obj = xfer.GetObj();
  CSimplePool* pool = static_cast< TObjOwnerParam< CSimplePool* >* >(obj.GetPtr())->GetData();

  return rs_new CGuiFrame(in, pool);
}
