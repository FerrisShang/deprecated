#ifndef __LIBC_H_INCLUDED__
#define __LIBC_H_INCLUDED__

#define INLINE inline
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979
#endif

#define libc_malloc  malloc
#define libc_calloc  calloc
#define libc_realloc realloc
#define libc_free    free

#define libc_memset  memset
#define libc_memcpy  memcpy
#define libc_memmove memmove

#define libc_frexp   frexp
#define libc_exp     exp
#define libc_pow     pow

#endif//__LIBC_H_INCLUDED__
