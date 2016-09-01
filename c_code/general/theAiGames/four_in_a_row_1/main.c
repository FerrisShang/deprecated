#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ai.h"

#define STR_SETTINGS   "settings "
#define STR_UPDATE     "update "
#define STR_ACTION     "action "
#define STR_TEST       "t "
#define STR_YOUR_BOTID "your_botid"

struct ai_four *ai;
static void settings(char *type, char *value);
static void update(char *field);
static void action(char *time_limit);
static void test(char *args1, char *args2, char *args3);
static void dump_field(struct ai_four *ai);

int main(int argc, char *argv[])
{
#define MAX_LINE_LEN 9128
#define MAX_ARG_LEN 1024
	char line[MAX_LINE_LEN];
	char args[4][MAX_ARG_LEN];

	struct timeval t_time;
	gettimeofday(&t_time, NULL);
	srand(t_time.tv_usec);

	ai = ai_create();
	if(!ai){
		return -1;
	}
	ai_reset(ai, DEFAULT_TIMEBANK, DEFAULT_TIME_PER_MOVE);

	while(fgets(line, MAX_LINE_LEN, stdin) != NULL){
		if (!strncmp(line, STR_SETTINGS, strlen(STR_SETTINGS))){
			sscanf(line, "%s %s %s", args[0], args[1], args[2]);
			settings(args[1], args[2]);
		}else if (!strncmp(line, STR_UPDATE, strlen(STR_UPDATE))) {
            sscanf(line, "%s %s %s %s", args[0], args[1], args[2], args[3]);
            update(args[3]);
		}else if (!strncmp(line, STR_ACTION, strlen(STR_ACTION))) {
            sscanf(line, "%s %s", args[0], args[1]);
			action(args[1]);
        }else if (!strncmp(line, STR_TEST, strlen(STR_TEST))) {
            sscanf(line, "%s %s %s %s", args[0], args[1], args[2], args[3]);
			test(args[1], args[2], args[3]);
        }

	}
	return 0;
}
static void settings(char *type, char *time_limit)
{
	if(!strcmp(type, STR_YOUR_BOTID)){
		ai_set_id(ai, atoi(time_limit));
		ai_reset(ai, DEFAULT_TIMEBANK, DEFAULT_TIME_PER_MOVE);
	}
}
static void update(char *field)
{
	ai_update_field(ai, field);
}
static void action(char *time_limit)
{
	int col;
	col = ai_move(ai, atoi(time_limit));
	io_printf("place_disc %d\n", col);
}
static void test(char *args1, char *args2, char *args3)
{
	int col = atoi(args1);
	int id = atoi(args2);
	ai_set_id(ai, four_op_id(id));
	four_add(ai->four, id, col);
	col = ai_move(ai, 1000);
	four_add(ai->four, four_op_id(id), col);
	dump_field(ai);
}
static void dump_field(struct ai_four *ai)
{
	int i,j;
	for(i=FOUR_ROW-1;i>=0;i--){
		for(j=0;j<FOUR_COL;j++){
			/*
			if(((ai->four->field_h[1][i] >> j) & 1)){
				dbg_printf("1");
			}else if(((ai->four->field_h[2][i] >> j) & 1)){
				dbg_printf("2");
			}else{
				dbg_printf("0");
			}
			*/
		}
		dbg_printf("\n");
	}
	dbg_printf("\n");
}
