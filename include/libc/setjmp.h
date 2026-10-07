#ifndef _SETJMP_H_
#define _SETJMP_H_

/* G2MEAB luaD_throw: buffer at +4, status at +0x194. */
typedef unsigned long jmp_buf[100];

#ifdef __cplusplus
extern "C" {
#endif
int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int value);
#ifdef __cplusplus
}
#endif

#endif
