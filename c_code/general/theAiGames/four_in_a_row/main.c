#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "main.h"

void get_lines(struct command* cmd)
{
	char cmd_type[16];
	int res;
	res = scanf("%s", cmd_type);
	if(!strcmp(cmd_type, STR_SETTINGS)){
		cmd->type = TYPE_SETTINGS;
		res = scanf("%s %s", cmd->settings.type, cmd->settings.value);
	}else if(!strcmp(cmd_type, STR_UPDATE)){
		cmd->type = TYPE_UPDATE;
		res = scanf("%s %s %s", cmd->update.player, cmd->update.type, cmd->update.value);
	}else if(!strcmp(cmd_type, STR_ACTION)){
		cmd->type = TYPE_ACTION;
		res = scanf("%s %s", cmd->action.type, cmd->action.time);
	}else if(!strcmp(cmd_type, STR_PLAY_TEST)){
		cmd->type = TYPE_PLAY_TEST;
		res = scanf("%s", cmd->play_test.value);
	}else{
		cmd->type == TYPE_UNKNOWN;
	}
}

int main(int argc, char *argv[])
{
	struct command cmd;
	struct data data;
	memset(&data, 0, sizeof(struct data));

	struct timeval t_time;
	gettimeofday(&t_time, NULL);
	srand(t_time.tv_usec);
	data.your_botid = 2;
	data.field_columns = 7;
	data.field_rows = 6;
	init_para(&data);

	while(1){
		get_lines(&cmd);
		if(cmd.type == TYPE_PLAY_TEST){
			test(&data, atoi(cmd.play_test.value));
		}else if(cmd.type == TYPE_ACTION){
			action_move(&data);
		}else if(cmd.type == TYPE_SETTINGS || cmd.type == TYPE_UPDATE){
			update_data(&data, &cmd);
		}else if(cmd.type == TYPE_UNKNOWN){
		}
#if DEBUG==1
		dump_data(&data);
#endif
	}
}

