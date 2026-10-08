/*
 * G2MEAB prototype NonMatching translation-unit scaffold.
 * .text 0x804816E4..0x80481E20; 15 retained native bodies.
 * Function and helper inventory is recorded in the external agent workflow.
 */
#include "GuiSys/CAuiMeter.hpp"

#include "Kyoto/Streams/CInputStream.hpp"

CGuiWidget* CAuiMeter::Create(CGuiFrame* frame, CInputStream& in, CSimplePool* pool, uint version) {
  CGuiWidgetParms parms = ReadWidgetHeader(frame, in);
  in.ReadBool();
  const bool noRoundUp = in.ReadBool();
  const int maxCapacity = in.ReadInt32();
  const int workerCount = in.ReadInt32();
  CGuiWidget* widget = rs_new CAuiMeter(parms, noRoundUp, maxCapacity, workerCount);
  widget->ParseBaseInfo(frame, in, parms, version);
  return widget;
}

CAuiMeter::CAuiMeter(const CGuiWidgetParms& parms, const bool noRoundUp, const int maxCapacity,
                     const int workerCount)
: CGuiCompoundWidget(parms)
, mNoRoundUp(noRoundUp)
, mMaxCapacity(maxCapacity)
, mCapacity(mMaxCapacity)
, mValue(0) {
  mWorkers.reserve(workerCount);
}

bool CAuiMeter::AddWorkerWidget(CGuiWidget* worker) {
  short id = worker->GetWorkerId();
  if (id >= mWorkers.size()) {
    for (int i = mWorkers.size(); i <= id; ++i) {
      mWorkers.push_back_unsafe(nullptr);
    }
  }
  mWorkers[id] = worker;
  return true;
}

CGuiWidget* CAuiMeter::GetWorkerWidget(int idx) { return mWorkers[idx]; }

void CAuiMeter::OnVisible() {
  if (GetIsVisible()) {
    UpdateMeterWorkers();
  }
}

void CAuiMeter::UpdateMeterWorkers() {
  int workerCount = mWorkers.size() / 2;
  const float scale = workerCount / float(mMaxCapacity);
  int etankCap = mNoRoundUp ? static_cast< int >(scale * mCapacity)
                            : static_cast< int >(0.5f + scale * mCapacity);
  int etankFill =
      mNoRoundUp ? static_cast< int >(scale * mValue) : static_cast< int >(0.5f + scale * mValue);

  for (int i = 0; i < workerCount; ++i) {
    CGuiWidget* const empty = mWorkers[i * 2];
    CGuiWidget* const filled = mWorkers[i * 2 + 1];

    if (i < etankFill) {
      if (filled)
        filled->SetIsVisible(true);
      if (empty)
        empty->SetIsVisible(false);
    } else if (i < etankCap) {
      if (filled)
        filled->SetIsVisible(false);
      if (empty)
        empty->SetIsVisible(true);
    } else {
      if (filled)
        filled->SetIsVisible(false);
      if (empty)
        empty->SetIsVisible(false);
    }
  }
}

// Native functions without reference source, drafted with mwdec (exact objdiff matches).
// mwdec-drafted
extern "C" int fn_80481768(int obj, int val);
extern "C" int fn_80481768(int obj, int val) {
    if (obj) {
        CMemory::Free((const void*)*(int*)((char*)obj + 0xc));
        if ((short)val > 0) {
            CMemory::Free((const void*)obj);
        }
    }
    return obj;
}

extern "C" int fn_80481DFC();
extern "C" int fn_80481DFC() {
    return 0x4d455452;
}

extern "C" bool fn_80481E08();
extern "C" bool fn_80481E08() {
    return false;
}

extern "C" bool fn_80481E10(int obj);
extern "C" bool fn_80481E10(int obj) {
    return (*(unsigned char*)((char*)obj + 0xba)) >> 6 & 1;
}

extern "C" void fn_80481E1C();
extern "C" void fn_80481E1C() {
}

