#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"

int isInRange(struct data *data, int x, int y)
{
	if(x<0 || x>=data->field_columns)return 0;
	if(y<0 || y>=data->field_rows)return 0;
	return 1;
}

int isFinish(struct data *data, int last_col)
{
	int player_id;
	char (*f)[MAX_FIELD] = data->field;
	int x = last_col, y;
	int cnt, tx, ty;
	for(y=0;y<data->field_rows;y++){
		if(f[y][x] != 0) break;
	}
	player_id = f[y][x];
	if(player_id == 0) return 0;
	//horizontal
	cnt = 1;
	tx = x;
	while(isInRange(data, --tx,y) && f[y][tx] == player_id){
		cnt++;
	}
	tx = x;
	while(isInRange(data, ++tx,y) && f[y][tx] == player_id){
		cnt++;
	}
	if(cnt >= 4) return 1;
	//vertical
	cnt = 1;
	ty = y;
	while(isInRange(data, x,--ty) && f[ty][x] == player_id){
		cnt++; }
	ty = y;
	while(isInRange(data, x,++ty) && f[ty][x] == player_id){
		cnt++;
	}
	if(cnt >= 4) return 1;
	//sideling
	cnt = 1;
	tx = x; ty = y;
	while(isInRange(data, --tx,--ty) && f[ty][tx] == player_id){
		cnt++;
	}
	tx = x; ty = y;
	while(isInRange(data, ++tx,++ty) && f[ty][tx] == player_id){
		cnt++;
	}
	if(cnt >= 4) return 1;

	cnt = 1;
	tx = x; ty = y;
	while(isInRange(data, --tx,++ty) && f[ty][tx] == player_id){
		cnt++;
	}
	tx = x; ty = y;
	while(isInRange(data, ++tx,--ty) && f[ty][tx] == player_id){
		cnt++;
	}
	if(cnt >= 4) return 1;
	return 0;
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
			int i,j;
			char *p = cmd->update.value;
			for(j=0;j<data->field_rows;j++){
				for(i=0;i<data->field_columns;i++){
					data->field[j][i] = (*p - '0');
					p += 2;
				}
			}
		}
	}
}

void dump_data(struct data *data)
{
	int i,j;
	/*
	printf("tb=%d,tp=%d,n=%s,b=%s,\nid=%d,c=%d,r=%d,round=%d\n",
			data->timebank, data->time_per_move,
			data->player_names, data->your_bot,
			data->your_botid, data->field_columns, data->field_rows, data->round
		  );
	*/
	for(j=0;j<data->field_rows;j++){
		for(i=0;i<data->field_columns;i++){
			printf("%d ", data->field[j][i]);
		}
	}
	printf("\n");
}

void update_field(struct data *data, int id, int col)
{
	char (*f)[MAX_FIELD] = data->field;
	int x = col, y;
	if(id >=0 && f[0][x] != 0) return;
	if(id < 0){
		for(y=0;y<data->field_rows;y++){
			if(f[y][x] != 0) break;
		}
		f[y][x] = 0;
	}else{
		for(y=0;y<data->field_rows-1;y++){
			if(f[y+1][x] != 0) break;
		}
		f[y][x] = id;
	}
}
void dump_field(struct data *data)
{
	int x=data->field_columns;
	int y=data->field_rows;
	char (*f)[MAX_FIELD] = data->field;
	int i, j;
	printf("----");
	for(i=0;i<x;i++)printf("--");
	printf("\n");
	for(j=0;j<y;j++){
		printf("| ");
		for(i=0;i<x;i++){
			if(data->field[j][i] == 0) printf("  ");
			else if(data->field[j][i] == data->your_botid) printf("★ ");
			else printf("☆ ");
		}
		printf(" |\n");
	}
	for(i=0;i<x;i++)printf("--");
	printf("----\n");
}
void test(struct data *data, int col)
{
	update_field(data, 3-data->your_botid, col);
	//dump_data(data);
	update_field(data, data->your_botid, cal_col(data));
	//dump_data(data);
	dump_field(data);
}

void action_move(struct data* data)
{
	fprintf(stdout,"place_disc %d\n", cal_col(data));
	fflush(stdout);
}


