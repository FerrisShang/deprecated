#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ttt.h"

static void reset_field(struct ttt *ttt);
static void init_maps(struct ttt *ttt);
struct ttt* ttt_create(void)
{
	struct ttt *ttt;
	ttt = malloc(sizeof(struct ttt));
	if(!ttt){
		return NULL;
	}
	memset(ttt, 0, sizeof(struct ttt));
	init_maps(ttt);
	return ttt;
}

void ttt_destory(struct ttt *ttt)
{
	free(ttt);
}
int ttt_add(struct ttt *ttt, int id, point_t p)
{
	int blockNum = ttt->blockMap[p];
	int blockOffNum = ttt->blockOffMap[p];
	int posOffNum = ttt->posOffMap[p];
	int status;
	if(GB1(ttt->m_field, blockOffNum) != NORMAL){
		return -1;
	}
	if(GB1(ttt->s_field[blockNum], posOffNum) != EMPTY){
		return -1;
	}else{
	}
	SB1(ttt->s_field[blockNum], id, posOffNum);

	status = ttt_isSFinish(ttt, p);
	SB1(ttt->m_field, status, blockOffNum);

	return 0;
}

int ttt_remove(struct ttt *ttt, point_t p)
{
	int blockNum = ttt->blockMap[p];
	int blockOffNum = ttt->blockOffMap[p];
	int posOffNum = ttt->posOffMap[p];

	CB1(ttt->s_field[blockNum], posOffNum);
	CB1(ttt->m_field, blockOffNum);

	return 0;
}

int ttt_isSFinish(struct ttt *ttt, point_t last_point)
{
	int blockNum = ttt->blockMap[last_point];
	return ttt->finishMap[ttt->s_field[blockNum]];
}
int ttt_isMFinish(struct ttt *ttt)
{
	return ttt->finishMap[ttt->m_field];
}

int ttt_set_field(struct ttt *ttt, char field[][9])
{
	int x, y;
	reset_field(ttt);
	for(y=0;y<9;y++){
		for(x=0;x<9;x++){
			int id = field[y][x];
			if(id == ID_P1 || id == ID_P2){
				point_t p = POINT(x,y);
				ttt_add(ttt, id, p);
			}
		}
	}
	return 0;
}
int ttt_set_mboard(struct ttt *ttt, int *m_board)
{
	memcpy(ttt->macroboard, m_board, sizeof(ttt->macroboard));
	return 0;
}
char* ttt_dump_field(struct ttt *ttt)
{
	static char field[2*9*9+1];
	char f[9][9];
	int i,j;
	ttt_get_field(ttt, f);
	for(j=0;j<9;j++){
		for(i=0;i<9;i++){
			int idx = i+j*9;
			field[idx*2] = f[j][i] + '0';
			if(i==9-1){
				field[idx*2+1] = '\n';
			}else{
				field[idx*2+1] = ',';
			}
		}
	}
	return field;
}
int ttt_get_field(struct ttt *ttt, char field[][9])
{
	int x, y;
	memset(field, 0, 9*9);
	for(y=0;y<9;y++){
		for(x=0;x<9;x++){
			int id, blockNum, posOffNum;
			blockNum = ttt->blockMap[POINT(x,y)];
			posOffNum = ttt->posOffMap[POINT(x,y)];
			id = GB1(ttt->s_field[blockNum], posOffNum);
			field[y][x] = id;
		}
	}
	return 0;
}
static void reset_field(struct ttt *ttt)
{
	memset(ttt->s_field, 0, sizeof(ttt->s_field));
	memset(&ttt->m_field, 0, sizeof(ttt->m_field));
}
static void init_maps(struct ttt *ttt)
{
	int *map;
	int i, j, id;
	//init point to block number map
	map = ttt->blockMap;
	j = 1<<(sizeof(point_t)*8);
	memset(map, -1, j);
	for(i=0;i<j;i++){
		map[i] = POINT_X(i)/3 + (POINT_Y(i)/3)*3;
	}
	//init point to block offset number map
	for(i=0;i<1<<(sizeof(point_t)*8);i++){
		ttt->blockOffMap[i] = ttt->blockMap[i]*2;
	}

	//init point to position number map
	map = ttt->posMap;
	j = 1<<(sizeof(point_t)*8);
	memset(map, -1, j);
	for(i=0;i<j;i++){
		map[i] = POINT_X(i)%3 + (POINT_Y(i)%3)*3;
	}
	//init point to offset number map
	for(i=0;i<1<<(sizeof(point_t)*8);i++){
		ttt->posOffMap[i] = ttt->posMap[i]*2;
	}
	//init finish map
	map = ttt->finishMap;
	memset(map, 0, 1<<TTT_BLK_NUM*2);
	for(i=0;i<(1<<TTT_BLK_NUM*2);i++){//judgement of win
		for(id=1;id<=2;id++){
			if(GB2(i, 2)==id && GB2(i, 4)==id && GB2(i, 6)==id){
				map[i] = id; continue;
			}
			if(GB2(i, 0)==id && GB2(i, 4)==id && GB2(i, 8)==id){
				map[i] = id; continue;
			}
			for(j=0;j<=2;j++){
				if(GB2(i, j+0)==id && GB2(i, j+3)==id && GB2(i, j+6)==id){
					map[i] = id; break;
				}
			}
			if(map[i]==1) continue;
			for(j=0;j<9;j+=3){
				if(GB2(i, j+0)==id && GB2(i, j+1)==id && GB2(i, j+2)==id){
					map[i] = id; break;
				}
			}
			if(map[i]==1) continue;
		}
	}
	for(i=0;i<1<<TTT_BLK_NUM*2;i++){//judgement of draw
		int id;
		if(map[i] != NORMAL){
			continue;
		}
		map[i] = DRAW;
		for(id=1;id<=2;id++){
			int tmp = i;
			for(j=0;j<TTT_BLK_NUM;j++){
				if(GB2(tmp, j) == EMPTY){
					SB2(tmp, id, j);
				}
			}
			if(map[tmp] == id){
				map[i] = NORMAL;
				break;
			}
		}
	}
	//init block & offset to point map
	for(i=0;i<TTT_BLK_NUM;i++){
		for(j=0;j<TTT_BLK_NUM;j++){
			ttt->bo2poMap[i][j] = POINT(
					(3*(i%3)+(j%3)),(3*(i/3)+(j/3)));
		}
	}
}
#if 0
void dump_ttt(struct ttt *ttt)
{
	char field[9][9]={{0}};
	int i,j;
	ttt_get_field(ttt, field);
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			printf("%d ", field[i][j]);
		}
		printf("\n");
	}
	for(i=0;i<9;i++){
		printf("%d ", GB2(ttt->m_field, i));
	}
	printf(" <<<\n%d\n", ttt_isBFinish(ttt));
}
void ttt_test(struct ttt *ttt)
{
	char field[9][9]={{0}};
	int i,j,s;
	struct {int id;int x;int y;}a[]= {
		{1,1,1},{1,2,2},{1,2,4},{2,0,2},{2,2,1},{2,0,1},{1,1,0},{2,1,2},{1,1,1},{1,0,0},{1,0,0},
	},b[]={
		{1,0,2},{1,1,2},{1,1,3},{1,2,4},
	};
	ttt_set_field(ttt, field);
	dump_ttt(ttt);
	for(i=0;i<sizeof(a)/sizeof(a[0]);i++){
		ttt_add(ttt, a[i].id, POINT(a[i].x, a[i].y));
		dump_ttt(ttt);
	}
	ttt_remove(ttt, POINT(b[0].x, b[0].y));
	dump_ttt(ttt);
}
#endif
