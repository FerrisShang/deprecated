#ifndef __SCREEN_H__
#define __SCREEN_H__

#include <stdarg.h>

int getkey(void);
void screen_clear(void);
void screen_go(int x, int y);
void screen_print(const char *fmt, ...);

#endif /* __SCREEN_H__ */
