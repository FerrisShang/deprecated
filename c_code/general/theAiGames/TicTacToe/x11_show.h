#ifndef __X11_SHOW_H__
#define __X11_SHOW_H__

#include "ai.h"
int x11_show(void);
void draw_m(int x, int y, int col);
void draw_s(int x, int y, int col);
point_t is_inTTT(int x, int y);
void draw_p(int x, int y, int col);

#endif /* __X11_SHOW_H__ */
