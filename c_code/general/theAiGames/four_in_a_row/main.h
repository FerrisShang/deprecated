#ifndef __FOUR_TYPES_H__
#define __FOUR_TYPES_H__

#include <stdio.h>

#define STR_SETTINGS   "settings"
#define STR_UPDATE     "update"
#define STR_ACTION     "action"
#define STR_PLAY_TEST  "t"
#define MAX_STRING_LEN 1024
#define MAX_FIELD 10

#define DEBUG 1
enum{
	TYPE_SETTINGS,
	TYPE_UPDATE,
	TYPE_ACTION,
	TYPE_PLAY_TEST,
	TYPE_UNKNOWN,
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

void test(struct data *data, int col);
int cal_col(struct data *data);
void action_move(struct data* data);
void update_data(struct data* data, struct command* cmd);

#endif
