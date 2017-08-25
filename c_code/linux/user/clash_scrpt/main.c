#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "get_screen.h"
#define LOCAL_SH " /tmp/sh.tmp "
#define LOCAL_SC "/tmp/sc.tmp"
#define GET_SCREEN() system(ADB_GET_SC ">" LOCAL_SC);

static int sync_flag, cnt, nclock, leaveClock, card[4], sync_flag_idx ;

void battle_proc(struct screen *screen);
struct {
	int page;
}data = {-1};
void clearall(void)
{
	int i;
	for(i=0;i<4;i++){
		card[i] = -1;
	}
	sync_flag = 0;
}
int main(int argc, char *argv[])
{
	struct screen *screen;
	system("echo " "\""SC_PICK_SH"\"" ">"LOCAL_SH);
	system(ADB_PUSH  LOCAL_SH  SH_PPATH);
	clearall();
	while(1){
		GET_SCREEN();
		screen = get_screen_data(LOCAL_SC);
		if(!screen){
			printf("get screen failed\n");
			return -1;
		}
		int p = get_page(screen);
		if(p != data.page){
			data.page = p;
			switch(data.page){
				case PAGE_HOME :
					ADB_PRESS(B_ENTRY_POSX, B_ENTRY_POSY);
					break;
				case PAGE_LEVEL_CHANGE :
					ADB_PRESS(B_LEVEL_POSX, B_LEVEL_POSY);
					break;
				case PAGE_ENTRY_CONFIRM :
					ADB_PRESS(B_CONFIRM_POSX, B_CONFIRM_POSY);
					break;
				case PAGE_BATTLE_DONE :
					ADB_PRESS(B_NONE_POSX, B_EXIT_BT_POSY);
					break;
				case PAGE_UNKNOWN:
					ADB_PRESS(B_NONE_POSY, B_EXIT_BT_POSY);
					break;
			}
		}
		if(data.page == PAGE_BATTLE){
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
		}else if(data.page == PAGE_UNKNOWN){
			ADB_PRESS(540, 1919);
		}else{
			if(nclock-leaveClock>3){
				leaveClock = time(NULL);
				clearall();
			}
		}
	}
}
void battle_proc(struct screen *screen)
{
	int i;
	nclock = time(NULL);
	cnt = get_ex_cnt(screen);
	int *c = get_cards(screen);
	if(sync_flag<=10){
		int flag = 0;
		for(i=0;i<4;i++){
			if(card[i] == 4){
					flag++;
			}
		}
		if(flag > 1){
			for(i=0;i<4;i++){
				if(card[i] == 4){
					card[i] = -1;
				}
			}
		}
		flag = 0;
		for(i=0;i<4;i++){
			if(card[i] == 3){
					flag++;
			}
		}
		if(flag > 1){
			for(i=0;i<4;i++){
				if(card[i] == 3){
					card[i] = -1;
				}
			}
		}
		for(i=0;i<4;i++){
			if(card[i] < 0 && cnt > 2 && c[i]>0 && c[i]<=4){
				card[i] = c[i];
			}
			if(card[i] < 0 && cnt > 3 && c[i]==0){
				card[i] = 4;
			}
		}
		if((card[0]==3||card[1]==3||card[2]==3||card[3]==3)&&
				(card[0]==4||card[1]==4||card[2]==4||card[3]==4)){
			if(cnt >= 9){
				for(i=0;i<4;i++){
					if(sync_flag < 5){
						if(card[i] == 3){
							sync_flag = 5;
							ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
							ADB_PRESS(490, 1440);
							sync_flag_idx = i;
							break;
						}
					}else{
						if(card[i] == 4){
							sync_flag = 100;
							ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
							ADB_PRESS(490, 1000);
							break;
						}
					}
				}
			}
		}else{
			if(cnt >= 6){
				for(i=0;i<4;i++){
					if(card[i] < 3 && card[i] > 0){
						ADB_PRESS(C_BASE_POSX+i*C_WIDTH_POS, C_BASE_POSY);
						ADB_PRESS(583, 1429);
						card[i] = -1;
						break;
					}
				}
			}
		}
	}else{
		switch((sync_flag-100) % 5){
			case 0:
				if(cnt >= 2){
					ADB_PRESS(C_BASE_POSX+sync_flag_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 900);
					sync_flag++;
				}
				break;
			case 1:
				if(cnt >= 2){
					ADB_PRESS(C_BASE_POSX+sync_flag_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 900);
					sync_flag++;
				}
				break;
			case 2:
				if(cnt >= 2){
					ADB_PRESS(C_BASE_POSX+sync_flag_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(250, 900);
					sync_flag++;
				}
				break;
			case 3:
				if(cnt >= 8){
					ADB_PRESS(C_BASE_POSX+sync_flag_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(490, 1440);
					sync_flag++;
				}
				break;
			case 4:
				if(cnt >= 8){
					ADB_PRESS(C_BASE_POSX+sync_flag_idx*C_WIDTH_POS, C_BASE_POSY);
					ADB_PRESS(490, 1000);
					sync_flag++;
					sleep(1);
				}
				break;
		}
	}
}
