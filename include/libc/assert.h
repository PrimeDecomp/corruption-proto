#ifndef _ASSERT_H_
#define _ASSERT_H_

#ifdef NDEBUG
#define assert(expr) ((void)0)
#else
#include <stdlib.h>
#define assert(expr) ((expr) ? (void)0 : abort())
#endif

#if __STDC_VERSION__ >= 201112L
// The C11 way
#define static_assert(cond, msg) _Static_assert(cond, #msg)
#else
// The old, hacky way
#define static_assert(cond, msg) typedef char static_assertion_##msg[(cond) ? 1 : -1]
#endif

#endif
