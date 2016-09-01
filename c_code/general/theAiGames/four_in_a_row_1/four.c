#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "four.h"


static void four_map_init(struct four *four)
{
	char *map;
	unsigned int i,j;
	map = four->finishMap;
	for(i=0;i<1<<16;i++){
		unsigned short num = i;
		while(num >= 0xF){
			if((num & 0xF) == 0xF){
				map[i] = 1;
				break;
			}else{
				num >>= 1;
			}
		}
	}
	map = four->nextPosMap;
	for(i=0;i<1<<8;i++){
		unsigned char num = i;
		for(j=0;j<8;j++){
			if((0x80 & num) != 0){
				map[i] = 8-j;
				break;
			}
			num <<= 1;
		}
	}
	char (*m)[FOUR_COL];
	m = four->fieldSxMap;
	for(i=0;i<FOUR_COL;i++){
		for(j=0;j<FOUR_ROW;j++){
			m[j][i] = i<j?i:j;
		}
	}
	m = four->fieldSyMap;
	for(i=0;i<FOUR_COL;i++){
		for(j=0;j<FOUR_ROW;j++){
			if(i > j){
				m[j][i] = (FOUR_COL-i-1)+j;
			}else{
				m[j][i] = FOUR_ROW + (j-i);
			}
		}
	}
	m = four->fieldBsxMap;
	for(i=0;i<FOUR_COL;i++){
		for(j=0;j<FOUR_ROW;j++){
			m[j][i] = (FOUR_ROW-j-1)<i?(FOUR_ROW-j-1):i;
		}
	}
	m = four->fieldBsyMap;
	for(i=0;i<FOUR_COL;i++){
		for(j=0;j<FOUR_ROW;j++){
			m[j][i] = i+j;
		}
	}
}

struct four* four_create(void)
{
	struct four *four;
	four = malloc(sizeof(struct four));
	if(!four){
		return NULL;
	}
	memset(four, 0, sizeof(struct four));
	four_map_init(four);
	return four;
}

void four_destory(struct four *four)
{
	free(four);
}

void four_reset(struct four *four)
{
	memset(four->field_h,  0, sizeof(four->field_h));
	memset(four->field_v,  0, sizeof(four->field_v));
	memset(four->field_s,  0, sizeof(four->field_s));
	memset(four->field_bs, 0, sizeof(four->field_bs));
}

int four_add(struct four *four, int id, int col)
{
	int tx,ty;
	int row = four->nextPosMap[
		(unsigned char)four->field_v[id][col] |
		(unsigned char)four->field_v[four_op_id(id)][col]
		];
	if(row >= FOUR_ROW || col >= FOUR_COL || col < 0){
		return -1;
	}else{
		four->field_h[id][row] |= 1<<col;
		four->field_v[id][col] |= 1<<row;
		tx = four->fieldSxMap[row][col];
		ty = four->fieldSyMap[row][col];
		four->field_s[id][ty] |= 1<<tx;
		tx = four->fieldBsxMap[row][col];
		ty = four->fieldBsyMap[row][col];
		four->field_bs[id][ty] |= 1<<tx;
		return 0;
	}
}

int four_remove(struct four *four, int id, int col)
{
	int row = four->nextPosMap[
		(unsigned char)four->field_v[id][col] |
		(unsigned char)four->field_v[four_op_id(id)][col]
		] - 1;
	int tx,ty;
	four->field_h[id][row] &= ~(1<<col);
	four->field_v[id][col] &= ~(1<<row);
	tx = four->fieldSxMap[row][col];
	ty = four->fieldSyMap[row][col];
	four->field_s[id][ty] &= ~(1<<tx);
	tx = four->fieldBsxMap[row][col];
	ty = four->fieldBsyMap[row][col];
	four->field_bs[id][ty] &= ~(1<<tx);
	return 0;
}

int four_isFinish(struct four *four, int id, int last_col)
{
	unsigned short line;
	int col = last_col;
	int row = four->nextPosMap[
		(unsigned char)four->field_v[id][col] |
		(unsigned char)four->field_v[four_op_id(id)][col]
		] - 1;
	line = four->field_h[id][row];
	if(four->finishMap[line] != 0){return 1;}
	line = four->field_v[id][col];
	if(four->finishMap[line] != 0){return 1;}
	line = four->field_s[id][(unsigned char)four->fieldSyMap[row][col]];
	if(four->finishMap[line] != 0){return 1;}
	line = four->field_bs[id][(unsigned char)four->fieldBsyMap[row][col]];
	if(four->finishMap[line] != 0){return 1;}
	return 0;
}

int four_set_field(struct four *four, char field[FOUR_ROW][FOUR_COL])
{
	int i, j, id;
	four_reset(four);
	for(j=0;j<FOUR_ROW;j++){
		for(i=0;i<FOUR_COL;i++){
			id = field[j][i];
			if(id != 0){
				four_add(four, id, i);
			}
		}
	}
	return 0;
}
int four_get_field(struct four *four, char field[FOUR_ROW][FOUR_COL])
{
	return 0;
	//unfinished
}
