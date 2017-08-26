#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "pthread.h"
#include "unistd.h"
#include "get_screen.h"
#define LOCAL_SH " /tmp/sh.tmp "
#define LOCAL_SC "/tmp/sc.tmp"

#define GET_SCREEN() system(ADB_GET_SC ">" LOCAL_SC);
#define RAND_IDX() (rand()%4)

int key_pending;
static void *adb_press(void*p)
{
	int x,y,*pos = p;
	char b[100];
	x = ((size_t)pos >> 16) & 0xFFFF;
	y = ((size_t)pos >>  0) & 0xFFFF;
	x += rand()%2-1;
	y += rand()%2-1;
	sprintf(b, ADB_SHELL "\"input tap %d %d \"", x, y);
	key_pending++;
	system(b);
	usleep(1200000);
	key_pending--;
	return NULL;
}
#define ADB_PRESS(x,y) do{\
	pthread_t th; \
	int pos = ((x) << 16) + y; \
	pthread_create(&th, NULL, adb_press, (void*)(size_t)pos); \
	pthread_detach(th); \
	usleep((250+rand()%50)*1000); \
}while(0)


enum {
	SYNC_NONE,
	SYNC_FOUND,
	SYNC_ATTACK,
};
struct info {
	int sync_flag;
	int ex_cnt;
	int cur_time;
	int battle_time;
	int card[4];
	int score;
} info;

int page_rec;
int drop_flag;

void battle_proc(struct screen *screen);

int main(int argc, char *argv[])
{
	struct screen *screen;
	system("echo " "\""SC_PICK_SH"\"" ">"LOCAL_SH);
	system(ADB_PUSH  LOCAL_SH  SH_PPATH);
	while(1){
		GET_SCREEN();
		screen = get_screen_data(LOCAL_SC);
		if(!screen){
			printf("get screen failed\n");
			return -1;
		}
		int p = get_page(screen);
		if(p != page_rec){
			printf("page_rec : %d\n", p);
			page_rec = p;
			switch(page_rec){
				case PAGE_HOME :
					memset(&info, 0, sizeof(struct info));
					ADB_PRESS(B_ENTRY_POSX, B_ENTRY_POSY);
					sleep(1);
					break;
				case PAGE_LEVEL_CHANGE :
					ADB_PRESS(B_LEVEL_POSX, B_LEVEL_POSY);
					sleep(1);
					break;
				case PAGE_ENTRY_CONFIRM :
					ADB_PRESS(B_CONFIRM_POSX, B_CONFIRM_POSY);
					sleep(1);
					break;
				case PAGE_BATTLE_DONE :
					ADB_PRESS(B_NONE_POSX, B_EXIT_BT_POSY);
					sleep(1);
					break;
				case PAGE_UNKNOWN:
					ADB_PRESS(B_NONE_POSY, B_EXIT_BT_POSY);
					sleep(1);
					break;
			}
		}
		if(page_rec == PAGE_BATTLE){
			if(argc == 1) {
				battle_proc(screen);
			}else if(argc == 2){
				if(argv[1][0]&1){
					if(!(time(NULL) & (1<<10))){
						battle_proc(screen);
					}
				}else{
					if((time(NULL) & (1<<10))){
						battle_proc(screen);
					}
				}
			}else{
				// Do nothing..
			}
		}else if(page_rec == PAGE_UNKNOWN){
			ADB_PRESS(540, 1919);
		}
	}
}
void battle_proc(struct screen *screen)
{
	static int card_select_idx;
	static int attack_step;
	int i, isQuick;
	info.cur_time = time(NULL);
	info.score = get_1920_1080_score(screen);
	if(info.score > 0 && info.score < 1600){
		printf("Score too low (%d), exit\n", info.score);
		exit(0);
	}else if(info.score > 0 && info.score > 2290){
		drop_flag = 1;
	}else if(info.score > 0 && info.score < 1800){
		drop_flag = 0;
	}
	if(drop_flag){
		printf("Score too HIGH (%d), dropping now\n", info.score);
		return;
	}
	if(info.battle_time == 0){
		printf("score:%d\n", info.score);
		info.battle_time = info.cur_time;
	}
	isQuick = info.cur_time-info.battle_time>55;
	info.ex_cnt = get_ex_cnt(screen);
	int *c = get_cards(screen, 1);
	for(i=0;i<4;i++){
		info.card[i] = c[i];
	}
	printf("sync=%d | ex:%d | c:%d %d %d %d | key(%d)\n",
			info.sync_flag, info.ex_cnt, c[0], c[1], c[2], c[3], key_pending);
	if(key_pending > 0){
		return;
	}
	if(info.sync_flag == SYNC_NONE){
		if((info.card[0]==3||info.card[1]==3||info.card[2]==3||info.card[3]==3)&&
				(info.card[0]==4||info.card[1]==4||info.card[2]==4||info.card[3]==4)){
			if(info.ex_cnt >= 9){
				for(i=0;i<4;i++){
					if(info.sync_flag < 5){
						if(info.card[i] == 3){
							info.sync_flag = SYNC_FOUND;
							ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
							ADB_PRESS(490, 1440);
							break;
						}
					}
				}
			}
		}else{
			if(info.ex_cnt >= 6){
				for(i=0;i<4;i++){
					if(info.card[i] < 3 && info.card[i] > 0){
						ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
						ADB_PRESS(583, 1429);
						info.card[i] = -1;
						break;
					}
				}
			}
		}
	}else if(info.sync_flag == SYNC_FOUND){
		if(info.ex_cnt >= 9){
			for(i=0;i<4;i++){
				if(info.card[i] == 4){
					info.sync_flag = SYNC_ATTACK;
					attack_step = 0;
					ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(490, 1050);
					break;
				}
			}
		}
	}else{ // info.sync_flag == SYNC_ATTACK
		switch((attack_step) % 5){
			case 0:
				if(info.ex_cnt >= 2){
					ADB_PRESS(C_BASE_POSX+RAND_IDX()*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 900+200*isQuick);
					attack_step++;
				}
				break;
			case 1:
				if(info.ex_cnt >= 2){
					ADB_PRESS(C_BASE_POSX+RAND_IDX()*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 900+50*isQuick);
					attack_step++;
				}
				break;
			case 2:
				if(info.ex_cnt >= 2){
					card_select_idx = RAND_IDX();
					ADB_PRESS(C_BASE_POSX+card_select_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 700);
					attack_step++;
				}
				break;
			case 3:
				if(info.ex_cnt >= 8){
					ADB_PRESS(C_BASE_POSX+card_select_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(490, 1440);
					attack_step++;
				}
				break;
			case 4:
				if(info.ex_cnt >= 8){
					ADB_PRESS(C_BASE_POSX+card_select_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(490, 1000+210*isQuick);
					attack_step++;
					sleep(1);
				}
				break;
		}
	}
}
