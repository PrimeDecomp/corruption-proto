// NonMatching translation-unit scaffold.
// G2MEAB .text 0x800F9BB4..0x800FA784 (end exclusive).
// Complete native/helper inventory: 23 functions, all implemented. The TSignal1 template
// functions (connect, list insert, emit, disconnect, connection) are instantiated here.
// The two constructors allocate the rc_ptr count through rs_new, so they reference "??(??)"
// where the binary has "rc_ptr.h(87) : ".

#include "MetroidPrime/CDebugOption.hpp"

#include "Kyoto/Alloc/CMemory.hpp"

CDebugOption::CDebugOption(int a, int b, const rstl::string& name, float value, float min,
                           float max, float step, const CColor& color)
: x0_(a)
, x4_(b)
, mName(name)
, mValue(value)
, mMin(min)
, mMax(max)
, mStep(step)
, mColor(color)
, mValueSignal(new ("CDebugOption.cpp(34) : ", (const char*)0) ValueSignal())
, x34_(0.9f)
, mChoices(nullptr) {}

CDebugOption::CDebugOption(int a, int b, const rstl::string& name, bool value, const CColor& color)
: x0_(a)
, x4_(b)
, mName(name)
, mValue(value ? 1.f : 0.f)
, mMin(0.f)
, mMax(1.f)
, mStep(1.f)
, mColor(color)
, mValueSignal(new ("CDebugOption.cpp(55) : ", (const char*)0) ValueSignal())
, x34_(0.9f)
, mChoices(nullptr) {}

CDebugOption::~CDebugOption() {
  if (mChoices != nullptr) {
    while (mChoices->empty() == false) {
      delete mChoices->front();
      mChoices->erase(mChoices->begin());
    }
    delete mChoices;
  }
}

void CDebugOption::AddChoice(const rstl::string& name, float value) {
  if (mChoices == nullptr) {
    mChoices =
        new ("CDebugOption.cpp(78) : ", (const char*)0) rstl::vector< SChoice* >(20, nullptr);
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

void CDebugOption::SetValue(float value) {
  if (mValue != value) {
    mValueSignal->Emit(this);
  }
  mValue = value;
}

rstl::auto_ptr< IConnection > CDebugOption::Connect(ValueSignal::Functor functor) {
  return mValueSignal->Connect(functor);
}

CDebugOption::SChoice::SChoice(const rstl::string& name, float value)
: mName(name), mValue(value) {}

const rstl::string& CDebugOption::SChoice::GetName() const { return mName; }

float CDebugOption::SChoice::GetValue() const { return mValue; }
