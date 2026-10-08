#ifndef KYOTO_ALLOC_ASSERT_HPP
#define KYOTO_ALLOC_ASSERT_HPP

#include "Kyoto/Alloc/CCallStack.hpp"

extern "C" void rs_log_assert_failure(const CCallStack* stack, const char* source, int line,
                                      const char* kind, const char* condition, const char* message);
extern "C" void rs_debugger_printf(const char* format, ...);
extern "C" void RAssert_TriggerIllegalInstruction();
extern "C" const char kUnknownType[];

#endif
