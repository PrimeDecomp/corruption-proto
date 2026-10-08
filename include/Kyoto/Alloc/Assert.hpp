#ifndef KYOTO_ALLOC_ASSERT_HPP
#define KYOTO_ALLOC_ASSERT_HPP

#include "Kyoto/Alloc/CCallStack.hpp"

extern "C" void rs_log_assert_failure(const CCallStack* stack, const char* source, int line,
                                      const char* kind, const char* condition, const char* message);
extern "C" void rs_debugger_printf(const char* format, ...);
extern "C" void RAssert_TriggerIllegalInstruction();
extern "C" const char kUnknownType[];

// Guessed name. 0x80796BF0: initialized to rs_debugger_printf.
extern void (*gpfnWarningPrintf)(const char* format, ...);

#define RS_STRINGIZE_IMPL(x) #x
#define RS_STRINGIZE(x) RS_STRINGIZE_IMPL(x)

// The failure path every G2MEAB "Verify" assert expands to. The exception is only stringized:
// this build logs "Would have thrown exception" and traps instead of throwing.
// The original macros use __LINE__; the line is explicit here so the original line numbers
// survive in the strings and the rs_log_assert_failure argument. The macro names are guessed.
#define RS_VERIFY_FAILURE(line, conditionText, exceptionText, message)                          \
  {                                                                                            \
    CCallStack stack(0, __FILE__ "(" RS_STRINGIZE(line) ") : ", kUnknownType);                 \
    rs_log_assert_failure(&stack, __FILE__, line, "Verify", conditionText, message);           \
    rs_debugger_printf("Would have thrown exception: %s\n", exceptionText);                    \
    RAssert_TriggerIllegalInstruction();                                                       \
  }

#define RS_VERIFY_THROW(line, condition, exception, message)                                    \
  if ((condition) == false)                                                                    \
  RS_VERIFY_FAILURE(line, #condition, #exception, message)

#endif
