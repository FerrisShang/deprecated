#ifndef __GET_SCREEN_H__
#define __GET_SCREEN_H__

#include "screen_info.h"

enum {
	PAGE_UNKNOWN,
	PAGE_HOME,
	PAGE_LEVEL_CHANGE,
	PAGE_ENTRY_CONFIRM,
	PAGE_LOADING,
	PAGE_BATTLE_DONE,
	PAGE_BATTLE,
};
struct rgba {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};
struct screen {
	int size;
	struct rgba (*data)[WIDTH];
};

struct screen* get_screen_data(char *path);
int get_page(struct screen *screen);
int get_ex_cnt(struct screen *screen);
int* get_cards(struct screen *screen);

#endif /* __GET_SCREEN_H__ */
