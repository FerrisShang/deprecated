#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

void init_para(struct data *data)
{
	char *map;
	unsigned int i,j;
	map = data->finishMap;
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
	map = data->nextPosMap;
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
	char (*m)[FIELD_COL];
	m = data->fieldSxMap;
	for(i=0;i<FIELD_COL;i++){
		for(j=0;j<FIELD_ROW;j++){
			m[j][i] = i<j?i:j;
		}
	}
	m = data->fieldSyMap;
	for(i=0;i<FIELD_COL;i++){
		for(j=0;j<FIELD_ROW;j++){
			if(i > j){
				m[j][i] = (FIELD_COL-i-1)+j;
			}else{
				m[j][i] = FIELD_ROW + (j-i);
			}
		}
	}
	m = data->fieldBsxMap;
	for(i=0;i<FIELD_COL;i++){
		for(j=0;j<FIELD_ROW;j++){
			m[j][i] = (FIELD_ROW-j-1)<i?(FIELD_ROW-j-1):i;
		}
	}
	m = data->fieldBsyMap;
	for(i=0;i<FIELD_COL;i++){
		for(j=0;j<FIELD_ROW;j++){
			m[j][i] = i+j;
		}
	}
}
int isColFull(struct data *data, int col)
{
	int id = 2;
	int row = data->nextPosMap[
		(unsigned char)data->field_v[id][col] |
		(unsigned char)data->field_v[op_id(id)][col]
		];
	if(row >= FIELD_ROW) return 1;
	else return 0;
}

int isFull(struct data *data)
{
	if(
			(data->field_h[1][data->field_rows-1] |
			 data->field_h[2][data->field_rows-1]) == ((1<<data->field_columns)-1)){
		return 1;
	}else{
		return 0;
	}
}

int isFinish(struct data *data, int id, int last_col)
{
	unsigned short line;
	int col = last_col;
	int row = data->nextPosMap[
		(unsigned char)data->field_v[id][col] |
		(unsigned char)data->field_v[op_id(id)][col]
		] - 1;
	line = data->field_h[id][row];
	if(data->finishMap[line] != 0){return 1;}
	line = data->field_v[id][col];
	if(data->finishMap[line] != 0){return 1;}
	line = data->field_s[id][data->fieldSyMap[row][col]];
	if(data->finishMap[line] != 0){return 1;}
	line = data->field_bs[id][data->fieldBsyMap[row][col]];
	if(data->finishMap[line] != 0){return 1;}
	return 0;
}

int judgement(struct data *data, int id, int col)
{
	return (rand()&0xFFFF);
}

void update_data(struct data* data, struct command* cmd)
{
	if(cmd->type == TYPE_SETTINGS){
		if(!strcmp(cmd->settings.type, "timebank")){
			data->timebank = atoi(cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "time_per_move")){
			data->time_per_move = atoi(cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "player_names")){
			strcpy(data->player_names, cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "your_bot")){
			strcpy(data->your_bot, cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "your_botid")){
			data->your_botid = atoi(cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "field_columns")){
			data->field_columns = atoi(cmd->settings.value);
		}else if(!strcmp(cmd->settings.type, "field_rows")){
			data->field_rows = atoi(cmd->settings.value);
		}
	}else if(cmd->type == TYPE_UPDATE){
		if(!strcmp(cmd->update.type, "round")){
			data->round = atoi(cmd->update.value);
		}else if(!strcmp(cmd->update.type, "field")){
			int i, j, id, idx;
			char *p = cmd->update.value;
			clear_field(data);
			for(j=data->field_rows-1;j>=0;j--){
				for(i=0;i<data->field_columns;i++){
					idx = 2 * (data->field_columns*j + i);
					id = p[idx] - '0';
					if(id != 0){
						add_field(data, id, i);
					}
				}
			}
		}
	}
}

void dump_data(struct data *data)
{
#if 0
	int i,j;
#if 0
	printf("tb=%d,tp=%d,n=%s,b=%s,\nid=%d,c=%d,r=%d,round=%d\n",
			data->timebank, data->time_per_move,
			data->player_names, data->your_bot,
			data->your_botid, data->field_columns, data->field_rows, data->round
		  );
#endif
	for(j=0;j<data->field_rows;j++){
		for(i=0;i<data->field_columns;i++){
			printf("%d ", data->field[j][i]);
		}
	}
	printf("\n");
#endif
}

void clear_field(struct data *data)
{
	memset(data->field_h,  0, sizeof(data->field_h));
	memset(data->field_v,  0, sizeof(data->field_v));
	memset(data->field_s,  0, sizeof(data->field_s));
	memset(data->field_bs, 0, sizeof(data->field_bs));
}
int add_field(struct data *data, int id, int col)
{
	int tx,ty;
	int row = data->nextPosMap[
		(unsigned char)data->field_v[id][col] |
		(unsigned char)data->field_v[op_id(id)][col]
		];
	if(row >= data->field_rows || col >= data->field_columns || col < 0){
		return -1;
	}else{
		data->field_h[id][row] |= 1<<col;
		data->field_v[id][col] |= 1<<row;
		tx = data->fieldSxMap[row][col];
		ty = data->fieldSyMap[row][col];
		data->field_s[id][ty] |= 1<<tx;
		tx = data->fieldBsxMap[row][col];
		ty = data->fieldBsyMap[row][col];
		data->field_bs[id][ty] |= 1<<tx;
	}
}

void remove_field(struct data *data, int id, int col)
{
	int row = data->nextPosMap[
		(unsigned char)data->field_v[id][col] |
		(unsigned char)data->field_v[op_id(id)][col]
		] - 1;
	int tx,ty;
	data->field_h[id][row] &= ~(1<<col);
	data->field_v[id][col] &= ~(1<<row);
	tx = data->fieldSxMap[row][col];
	ty = data->fieldSyMap[row][col];
	data->field_s[id][ty] &= ~(1<<tx);
	tx = data->fieldBsxMap[row][col];
	ty = data->fieldBsyMap[row][col];
	data->field_bs[id][ty] &= ~(1<<tx);
}
void get_field_str(char *str, struct data *data)
{
	char *p;
	int i, j, idx;
	sprintf(str, "update game field ");
	p = str + strlen("update game field ");
	for(j=0;j<data->field_rows;j++){
		for(i=0;i<data->field_columns;i++){
			idx = 2 * (data->field_columns*(j) + i);
			if((data->field_h[1][data->field_rows-1-j]>>i)&1){
				p[idx] = '1';
			}else if((data->field_h[2][data->field_rows-1-j]>>i)&1){
				p[idx] = '2';
			}else{
				p[idx] = '0';
			}
			p[idx+1] = ',';
		}
	}
	idx = 2 * (data->field_columns*5 + 6);
	p[idx+2] = '\0';
	strcat(p, "\n");
}

void dump_field(struct data *data)
{
	int x=data->field_columns;
	int y=data->field_rows;
	int id = data->your_botid;
	int i, j;
	printf("----");
	for(i=0;i<x;i++)printf("--");
	printf("\n");
	for(j=y-1;j>=0;j--){
		printf("| ");
		for(i=0;i<x;i++){
			if((data->field_h[id][j] & (1<<i)) != 0) printf("★ ");
			else if((data->field_h[op_id(id)][j] & (1<<i)) != 0)printf("☆ ");
			else printf("  ");
		}
		printf(" |\n");
	}
	for(i=0;i<x;i++)printf("--");
	printf("----\n");
}
void test(struct data *data, int col)
{
	add_field(data, op_id(data->your_botid), col);
	//dump_data(data);
	add_field(data, data->your_botid, cal_col(data));
	//dump_data(data);
	dump_field(data);
}

void action_move(struct data* data)
{
	fprintf(stdout,"place_disc %d\n", cal_col(data));
	fflush(stdout);
}
