#include <stdio.h>
#include <string.h>

#define STR_SETTINGS   "settings"
#define STR_UPDATE     "update"
#define STR_ACTION     "action"
#define STR_PLAY_TEST  "test"
#define MAX_STRING_LEN 1024
#define MAX_FIELD 10

#define DEBUG 0
enum{
	TYPE_SETTINGS,
	TYPE_UPDATE,
	TYPE_ACTION,
	TYPE_PLAY_TEST,
};

struct settings {
	char type[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
};
struct action {
	char type[MAX_STRING_LEN];
	char time[MAX_STRING_LEN];
};
struct update {
	char player[MAX_STRING_LEN];
	char type[MAX_STRING_LEN];
	char value[MAX_STRING_LEN];
};
struct play_test {
	char value[MAX_STRING_LEN];
};
struct command {
	int type;
	union {
		struct settings settings;
		struct action action;
		struct update update;
		struct play_test play_test;
	};
};
struct data {
	int timebank;
	int time_per_move;
	char player_names[MAX_STRING_LEN];
	char your_bot[MAX_STRING_LEN];
	int your_botid;
	int field_columns;
	int field_rows;
	int round;
	char field[MAX_FIELD][MAX_FIELD];
};

void get_lines(struct command* cmd)
{
	char cmd_type[16];
	scanf("%s", cmd_type);
	if(!strcmp(cmd_type, STR_SETTINGS)){
		cmd->type = TYPE_SETTINGS;
		scanf("%s %s", cmd->settings.type, cmd->settings.value);
	}else if(!strcmp(cmd_type, STR_UPDATE)){
		cmd->type = TYPE_UPDATE;
		scanf("%s %s %s", cmd->update.player, cmd->update.type, cmd->update.value);
	}else if(!strcmp(cmd_type, STR_ACTION)){
		cmd->type = TYPE_ACTION;
		scanf("%s %s", cmd->action.type, cmd->action.time);
	}else if(!strcmp(cmd_type, STR_PLAY_TEST)){
		cmd->type = TYPE_PLAY_TEST;
		scanf("%s", cmd->play_test.value);
	}
}

int cal_col(struct data *data);
void action(struct data* data)
{
	fprintf(stdout,"place_disc %d\n", cal_col(data));
	fflush(stdout);
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
	printf("tb=%d,tp=%d,n=%s,b=%s,\nid=%d,c=%d,r=%d,round=%d\n",
			data->timebank, data->time_per_move,
			data->player_names, data->your_bot,
			data->your_botid, data->field_columns, data->field_rows, data->round
		  );
	for(j=0;j<data->field_rows;j++){
		for(i=0;i<data->field_columns;i++){
			printf("%d ", data->field[j][i]);
		}
	}
	printf("\n");
}

void updata_field(struct data *data, int id, int col)
{
	char (*f)[MAX_FIELD] = data->field;
	int x = col, y;
	for(y=0;y<data->field_rows-1;y++){
		if(f[y+1][x] != 0) break;
	}
	f[y][x] = id;
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
	updata_field(data, data->your_botid, col);
	updata_field(data, 2, cal_col(data));
	dump_field(data);
}

int main(int argc, char *argv[])
{
	struct command cmd;
	struct data data;
	memset(&data, 0, sizeof(struct data));
	while(1){
		get_lines(&cmd);
		if(cmd.type == TYPE_PLAY_TEST){
			test(&data, atoi(cmd.play_test.value));
		}else if(cmd.type == TYPE_ACTION){
			action(&data);
		}else{
			update_data(&data, &cmd);
		}
#if DEBUG==1
		dump_data(&data);
#endif
	}
}

int isInRange(struct data *data, int x, int y)
{
	if(x<0 || x>=data->field_columns)return 0;
	if(y<0 || y>=data->field_rows)return 0;
	return 1;
}
int isFinish(struct data *data, int last_col, int player_id)
{
	char (*f)[MAX_FIELD] = data->field;
	int x = last_col, y;
	int cnt, tx, ty;
	for(y=0;y<data->field_rows;y++){
		if(f[y][x] != 0) break;
	}
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
		cnt++;
	}
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
	while(isInRange(data, --tx,--ty) && f[ty][tx] == player_id){
		cnt++;
	}
	tx = x; ty = y;
	while(isInRange(data, ++tx,++ty) && f[ty][tx] == player_id){
		cnt++;
	}
	if(cnt >= 4) return 1;
	return 0;
}

int cal_col(struct data *data)
{
	static int i;
	char (*f)[MAX_FIELD] = data->field;
	return (++i-1)%data->field_columns;
}