#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "ai.h"

#define STR_SETTINGS   "settings "
#define STR_UPDATE     "update "
#define STR_FIELD      "field"
#define STR_ACTION     "action "
#define STR_TEST       "t "
#define STR_YOUR_BOTID "your_botid"

struct ai_ttt *ai;
static void settings(char *type, char *value);
static void update(char *type, char *value);
static void action(char *time_limit);
static void test(char *args1, char *args2, char *args3);

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
			update(args[2], args[3]);
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
static void settings(char *type, char *id)
{
	if(!strcmp(type, STR_YOUR_BOTID)){
		ai_set_id(ai, atoi(id));
		ai_reset(ai, DEFAULT_TIMEBANK, DEFAULT_TIME_PER_MOVE);
	}
}
static void update(char *type, char *value)
{
	if(!strcmp(type, STR_FIELD)){
		ai_update_field(ai, value);
	}
}
static void action(char *time_limit)
{
	point_t res;
	res = ai_move(ai, atoi(time_limit));
	io_printf("place_disc %d\n", GET_P_X(res), GET_P_Y(res));
}
static void test(char *args1, char *args2, char *args3)
{
}
