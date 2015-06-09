#ifndef __DEBUG_H__
#define __DEBUG_H__

#define DBG_CLOSE    0
#define DBG_NULL     1
#define DBG_ERROR    2
#define DBG_WARNING  3
#define DBG_INFO     4

#define DBG_FORCE_ON 1

#if (DEBUG_LEVEL==DBG_CLOSE)
#define dbg_print(level, forced, fmt, ...)
#else
#include <stdarg.h>
void dbg_print(int level, int forced, const char *fmt, ...);
#endif /* (DEBUG_LEVEL==DBG_CLOSE) */

#endif /* __DEBUG_H__ */
