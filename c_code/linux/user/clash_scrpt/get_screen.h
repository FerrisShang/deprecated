#ifndef __GET_SCREEN_H__
#define __GET_SCREEN_H__

#include "screen_info.h"
#if defined(__WIN32__)
#define LOCAL_SH " d:\\tmp\\sh.tmp "
#define LOCAL_SC "d:\\tmp\\sc.tmp"
#else
#define LOCAL_SH " /tmp/sh.tmp "
#define LOCAL_SC "/tmp/sc.tmp"
#endif
#define KILL_APP "am force-stop com.tencent.tmgp.supercell.clashroyale "
#define PHONE_HOME " input keyevent 3 "
#define PHONE_UNLOCK " input keyevent 82 "
#define START_APP  " am start -n com.tencent.tmgp.supercell.clashroyale/.GameAppTencent "
#define PHONE_POWER " input keyevent 26 "

enum {
	PAGE_UNKNOWN,
	PAGE_HOME,
	PAGE_LEVEL_CHANGE,
	PAGE_ENTRY_CONFIRM,
	PAGE_LOADING,
	PAGE_BATTLE_DONE,
	PAGE_BATTLE,
	PAGE_LOGIN_1,
	PAGE_LOGIN_2,
};
struct rgba {
	unsigned char r;
	unsigned char g;
	unsigned char b;
	unsigned char a;
};
struct screen {
	int size;
	int idx;
	struct rgba (*data)[WIDTH];
};

struct screen* get_screen_data(char *path);
int get_page(struct screen *screen);
float get_ex_cnt(struct screen *screen);
int* get_cards(struct screen *screen, int abs_flag);
int get_1920_1080_score(struct screen *screen);
void GET_SCREEN(void);

#endif /* __GET_SCREEN_H__ */
