#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "get_screen.h"

#define MAX_BUF_SIZE (4<<20)
static struct screen screen;
static char *file_org_buf;
static char *file_new_buf;

static int get_file_size(const char *path);

struct screen* get_screen_data(char *path)
{
	FILE *fp;
	char *p,*q;
	int size, i;
	size = get_file_size(path);
	if(size <= 0){
		printf("get screenshot %s failed\n", path);
		return NULL;
	}
	if(file_org_buf == 0){
		file_org_buf = malloc(MAX_BUF_SIZE);
		file_new_buf = malloc(MAX_BUF_SIZE);
		if(!file_org_buf || !file_new_buf){
			printf("malloc error\n");
			return NULL;
		}
	}
	fp = fopen(path, "rb");
	if(fp != NULL){
		fread(file_org_buf, size, 1, fp);
		fclose(fp);
	}else{
		printf("read sc file failed\n");
		return NULL;
	}
	p = file_new_buf;
	q = file_org_buf;
	for(i=0;i<size-1;i++){
		if(*q == 0x0D && *(q+1) == 0x0A){
			q++;
			continue;
		}
		*p = *q;
		p++;
		q++;
	}
	*p = *q; //last byte
	p++;
	screen.size = p - file_new_buf;
	screen.data = (struct rgba(*)[WIDTH])file_new_buf;
	return &screen;
}

static int get_file_size(const char *path)
{
	unsigned long filesize = -1;
	struct stat statbuff;
	if(stat(path, &statbuff) < 0){
		return filesize;
	}else{
		filesize = statbuff.st_size;
	}
	return filesize;
}
int get_page(struct screen *screen)
{
	int i;
	int exit_flag;
#define IS_COL_LEV_CHG_BK(c) (c.r>40&&c.r<60&&c.g>55&&c.g<75&&c.b>70&&c.b<90)
#define IS_COL_ENT_CFM(c) (c.r==222&&c.g==235&&c.b>=240)
#define IS_COL_HOME_OB(c) (c.r>220&&c.g>160&&c.g<240&&c.b<100)
#define IS_COL_HOME_BK(c) (c.r<40&&c.g>50&&c.g<130&&c.b>100&&c.b<170)
#define IS_COL_BUTTON(c) (c.r==0&&c.g>70&&c.g<90&&c.b>140&&c.b<170)
#define IS_COL_EX(c) (c.r>120&&c.g<50&&c.b>120)
#define IS_COL_LOADING(c) (c.r==25&&c.g==117&&c.b==222)
#define IS_COL_LOADING_BK(c) (c.r==33&&c.g==36&&c.b==33)
	if(IS_COL_LEV_CHG_BK(screen->data[150][10])){
		exit_flag = 0;
		for(i=0;i<60;i++){
			if(!(IS_COL_LEV_CHG_BK(screen->data[150][i]) &&
						IS_COL_LEV_CHG_BK(screen->data[150][WIDTH-i-1]) &&
						!IS_COL_LEV_CHG_BK(screen->data[150][WIDTH/2-30+i]))){
				exit_flag = 1;
				break;
			}
		}
		if(!exit_flag){ return PAGE_LEVEL_CHANGE; }
	}
	if(IS_COL_ENT_CFM(screen->data[192][100])){
		exit_flag = 0;
		for(i=100;i<160;i++){
			if(!(IS_COL_ENT_CFM(screen->data[192][i]) &&
						IS_COL_ENT_CFM(screen->data[192][WIDTH-i-1]) &&
						!IS_COL_ENT_CFM(screen->data[192][WIDTH/2-130+i]))){
				exit_flag = 1;
				break;
			}
		}
		if(!exit_flag){ return PAGE_ENTRY_CONFIRM; }
	}
	if(IS_COL_HOME_BK(screen->data[192][10]) &&
			IS_COL_HOME_BK(screen->data[192][WIDTH-10])){
		exit_flag = 0;
		for(i=0;i<100;i++){
			if(!(IS_COL_HOME_BK(screen->data[192][i]) &&
						IS_COL_HOME_BK(screen->data[192][WIDTH-i-1]))){
				exit_flag = 1;
				break;
			}
		}
		if(exit_flag == 0){
			int tmp = 0;
			for(i=150;i<WIDTH-150;i++){
				if(IS_COL_HOME_BK(screen->data[192][i])){
					tmp++;
				}
			}
			if(tmp < 70){
				exit_flag = 1;
			}
		}
		if(!exit_flag){ return PAGE_HOME; }
	}
	if(IS_COL_BUTTON(screen->data[18][WIDTH/2])){
		exit_flag = 0;
		for(i=WIDTH/2-70;i<WIDTH/2+70;i++){
			if(!(IS_COL_BUTTON(screen->data[18][i]))){
				exit_flag = 1;
				break;
			}
		}
		if(!exit_flag){ return PAGE_BATTLE_DONE; }
	}
	if(IS_COL_EX(screen->data[150][250])){
		exit_flag = 0;
		for(i=230;i<275;i++){
			if(!(IS_COL_EX(screen->data[150][i]))){
				exit_flag = 1;
				break;
			}
		}
		if(!exit_flag){ return PAGE_BATTLE; }
	}
	if(IS_COL_LOADING(screen->data[180][20])){
		exit_flag = 0;
		for(i=30;i<WIDTH-30;i++){
			if(!(IS_COL_LOADING_BK(screen->data[187][i]))){
				exit_flag = 1;
				break;
			}
		}
		if(!exit_flag){ return PAGE_LOADING; }
	}
	//printf("xx : %d %d %d\n", screen->data[192][10].r,
	//		screen->data[192][10].g,
	//		screen->data[192][10].b);
	return PAGE_UNKNOWN;
}
int get_ex_cnt(struct screen *screen)
{
#define START_POS 295
#define EX_WIDTH   75
	int i, end;
	for(i=START_POS;i<WIDTH;i++){
		if(IS_COL_EX(screen->data[150][i])){
			end = i;
		}
	}
	if(end < START_POS){
		return -1;
	}else{
		return (end-START_POS+EX_WIDTH/2)/EX_WIDTH;
	}
}

static int get_1920_1080_num(struct screen *screen, int pos_x, int pos_y);
int* get_cards(struct screen *screen, int abs_flag)
{
#define POS_X_1 354
#define POS_X_2 558
#define POS_X_3 762
#define POS_X_4 964
#define EMPTY_LEVEL 50
#define SELECT_LEVEL 10
#define SELECT_Y_POS 70
#define SE_EX_Y_POS (192-(1920-1736))
#define USE_EX_Y_POS (192-(1920-1767))
#define IS_COL_CARD_BK(c) (c.r>80&&c.r<120&&c.g>60&&c.g<90&&c.b>35&&c.b<65)
#define IS_COL_CARD_EX(c) (c.r>60&&c.r<90&&c.g<20&&c.b>70&&c.b<100)
	int i,j, pos[] = {POS_X_1, POS_X_2, POS_X_3, POS_X_4};
	static int cards[4];
	for(i=0;i<sizeof(cards)/sizeof(cards[0]);i++){
		int flg1 = 0,flg2 = 0;
		for(j=pos[i]-50;j<pos[i]+50;j++){
			if(IS_COL_CARD_BK(screen->data[SELECT_Y_POS][j])){
				flg1++;
			}else{
				flg2++;
			}
		}
		if(flg1>EMPTY_LEVEL && flg2>SELECT_LEVEL){ cards[i] = -1; }
		else if(flg1<EMPTY_LEVEL){ cards[i] = 1; }
		else { cards[i] = 0; }
		if(cards[i]!=0){
			cards[i] *= get_1920_1080_num(screen, pos[i], cards[i]==1?USE_EX_Y_POS:SE_EX_Y_POS);
			if(abs_flag){ cards[i] = cards[i]<0?-cards[i]:cards[i]; }
		}
	}
	return cards;
}
void num_map(char (*num)[64],int x,int y,struct screen *screen, int sx,int sy)
{
#define IS_NUM_COL(c) (c.r>200&&c.g>200&&c.b>200)
	if(x<0 || y<0 || x>63 || y>63 || num[y][x] != 0) { return; }
	if(IS_NUM_COL(screen->data[sy][sx])){
		num[y][x] = 1;
	}else{
		num[y][x] = -1;
		return;
	}
	//printf("%d %d %d\n", screen->data[sy][sx].r,screen->data[sy][sx].g,screen->data[sy][sx].b);
	num_map(num,x,y+1,screen,sx,sy+1);
	num_map(num,x,y-1,screen,sx,sy-1);
	num_map(num,x+1,y,screen,sx+1,sy);
	num_map(num,x-1,y,screen,sx-1,sy);
}
static int get_1920_1080_num(struct screen *screen, int pos_x, int pos_y)
{
	int x = pos_x - 18;
	int y = pos_y + 18;
	int nx = 32; int ny = 10;
	char num[64][64] = {{0}};
	char n[32][24] = {{0}};
	num_map(num,nx,ny,screen,x,y);
	int i,j, t, l;

	for(i=0;i<64;i++){
		for(j=0;j<64;j++){
			if(num[i][j] == 1){
				t = i;
				goto mark_left;
			}
		}
	}
mark_left:
	for(i=0;i<64;i++){
		for(j=0;j<64;j++){
			if(num[j][i] == 1){
				l = i;
				goto mark_done;
			}
		}
	}
mark_done:
	for(i=t;i<t+32&&i<64;i++){
		for(j=l;j<l+24&&j<64;j++){
			if(num[i][j] == 1){ n[i-t][j-l] = num[i][j];}
		}
	}
	for(i=0;i<8;i++){
		for(j=0;j<6;j++){
			int sum = 0,q,w;
			for(q=0;q<4;q++)for(w=0;w<4;w++)sum+=n[i*4+q][j*4+w];
			n[i][j] = (sum+8)/16;
		}
	}

	int n_vec[][48] = {
		{1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0,
			0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 1, 0,
			1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0},
		{1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0,
			0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0},
		{0, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
			1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0},
	};
	int rec = -1, max_dist = -1;
	for(j=0;j<4;j++){
		int distence = 0;
		for(i=0;i<48;i++){
			distence += (n_vec[j][i]==n[i/6][i%6]);
		}
		if(max_dist < distence){
			max_dist = distence;
			rec = j;
		}
	}
	return rec+1;
}

static void score_map(char score[30][100],int x1,int y1,struct screen *screen,int x2,int y2)
{
#define IS_SCORE_COL(c) (c.r>200&&c.g>200&&c.b>200)
	if(x1<0 || y1<0 || x1>=100 || y1>=30 || score[y1][x1] != 0) { return; }
	if(IS_SCORE_COL(screen->data[y2][x2])){
		score[y1][x1] = 1;
	}else{
		score[y1][x1] = -1;
		return;
	}
	score_map(score,x1,y1+1,screen,x2,y2+1);
	score_map(score,x1,y1-1,screen,x2,y2-1);
	score_map(score,x1+1,y1,screen,x2+1,y2);
	score_map(score,x1-1,y1,screen,x2-1,y2);
}
static int get_score_num(char score[30][100],int l,int t, int w,int h)
{
	for(int i=t;i<t+h;i++){
		for(int j=l;j<l+w;j++){
			if(score[i][j] == 1){ printf("1"); }else{ printf("0"); }
		}
		printf("\n");
	}
	printf("\n");
	return 0;
}
int get_1920_1080_score(struct screen *screen)
{
#define SC_POS_X_S 99
#define SC_POS_X_E 198
#define SC_POS_OFFSET (192+1+5)
#define SC_POS_Y   (SC_POS_OFFSET+30/2)
#define SC_HEIGHT  (24)
	int i, j;
	char sc[30][100] = {{0}};
	for(i=SC_POS_X_S;i<SC_POS_X_E;i++){
		if(IS_SCORE_COL(screen->data[SC_POS_Y][i])){
			score_map(sc, i-SC_POS_X_S, SC_POS_Y-SC_POS_OFFSET, screen, i, SC_POS_Y);
		}
	}
	int num_idx = 0, found, left = 0,right, n[4];
	while(num_idx < 4){
		found = 0;
		for(i=left;i<100;i++){
			for(j=0;j<30;j++){ if(sc[j][i] == 1){ found = 1; break;} }
			if(found) break;
		}
		if(!found){break;}else{
			left = i;
			for(i=left;i<100;i++){
				found = 0;
				for(j=0;j<30;j++){ if(sc[j][i] == 1){ found = 1; break;} }
				if(found==0) break;
			}
			if(i == 100) break;
			right = i;
		}
		n[num_idx] = get_score_num(sc, left, 0, right-left, SC_HEIGHT);
		num_idx++;
		left = right+1;
	}
	if(num_idx == 4){
		return n[0]*1000+n[1]*100+n[2]*10+n[3]*1;
	}else{
		return -1;
	}
}
