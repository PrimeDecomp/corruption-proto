// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800F9BB4..0x800FA784 (end exclusive).
// Complete native/helper inventory: 23 functions. Not yet implemented:
// Boundary evidence is retained outside this repository in the agent workflow.
// 0x800F9BB4 +0x24: TSignal1 disconnect thunk (list erase, 0x800FA710)
// 0x800F9C24 +0x64, 0x800F9C88 +0x130, 0x800F9DB8 +0x40, 0x800F9DF8 +0x70, 0x800F9E68 +0x90:
//   TSignal1 connect path; allocates the 0x18-byte connection ("TSignal1.h(56) : ") and inserts
//   it into the signal's rstl::list
// 0x800F9EF8 +0x58: SetValue; emits the signal (0x800F9F50) when the value changes
// 0x800F9F50 +0x6C: TSignal1 emit; calls every connection's functor with the option
// 0x800FA224 +0xF8: destructor; deletes the choices, then x38_, the signal rc_ptr (released by
//   0x80048DDC in CGameDebug) and the name
// 0x800FA31C +0x54, 0x800FA370 +0x4C, 0x800FA3BC +0x64: rstl::vector< SChoice* > deleting
//   destructor and erase, only used by the destructor
// 0x800FA420 +0x114: bool constructor (line 55); allocates the signal ("rc_ptr.h(87) : ")
// 0x800FA534 +0x13C: float constructor (line 34)
// 0x800FA670 +0xC, 0x800FA67C +0x94: TSignal1 connection (vtable 0x806B3EE4)
// 0x800FA710 +0x74: TSignal1 rstl::list node erase
// All of the above need the TSignal1 template (and rc_ptr of it), which has no header yet.

#include "MetroidPrime/CDebugOption.hpp"

#include "Kyoto/Alloc/CMemory.hpp"

void CDebugOption::AddChoice(const rstl::string& name, float value) {
  if (mChoices == nullptr) {
    mChoices = new ("CDebugOption.cpp(78) : ", (const char*)0) rstl::vector< SChoice* >(20, nullptr);
    mChoices->clear();
  }
  mChoices->push_back(new ("CDebugOption.cpp(82) : ", (const char*)0) SChoice(name, value));
}

// Returns the name of the choice whose value is exactly `value`, or null when the option has
// no named values or none matches.
const rstl::string* CDebugOption::GetChoiceName(float value) {
  if (mChoices == nullptr) {
    return nullptr;
  }
  for (int i = 0; i < mChoices->size(); ++i) {
    const SChoice* choice = (*mChoices)[i];
    if (value == choice->GetValue()) {
      return &choice->GetName();
    }
  }
  return nullptr;
}

CDebugOption::SChoice::SChoice(const rstl::string& name, float value)
: mName(name), mValue(value) {}

const rstl::string& CDebugOption::SChoice::GetName() const { return mName; }

float CDebugOption::SChoice::GetValue() const { return mValue; }
