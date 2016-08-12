#ifndef __FOUR_TYPES_H__
#define __FOUR_TYPES_H__

#include <stdio.h>

#define STR_SETTINGS   "settings"
#define STR_UPDATE     "update"
#define STR_ACTION     "action"
#define STR_PLAY_TEST  "t"
#define MAX_STRING_LEN 1024
#define FIELD_ROW 6
#define FIELD_COL 7

#define VALUE_INF   100000000
#define GAME_MAX    1000000
#define STEP_MINIUS 10000

//#define DEBUG 1
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
	unsigned short field_h[3][FIELD_ROW];
	unsigned short field_v[3][FIELD_COL];
	unsigned short field_s[3][FIELD_ROW+FIELD_COL];
	unsigned short field_bs[3][FIELD_ROW+FIELD_COL];
	char finishMap[1<<16];
	char nextPosMap[1<<8];
	char fieldSxMap[FIELD_ROW][FIELD_COL];
	char fieldSyMap[FIELD_ROW][FIELD_COL];
	char fieldBsxMap[FIELD_ROW][FIELD_COL];
	char fieldBsyMap[FIELD_ROW][FIELD_COL];
};

#define op_id(id) (3-id)
void test(struct data *data, int col);
int cal_col(struct data *data);
void action_move(struct data* data);
void clear_field(struct data *data);
int add_field(struct data *data, int id, int col);
void remove_field(struct data *data, int id, int col);
void update_data(struct data* data, struct command* cmd);
int isColFull(struct data *data, int col);
int isFinish(struct data *data, int id, int last_col);
int isFull(struct data *data);
int judgement(struct data *data, int id, int col);
void get_field_str(char *str, struct data *data);

#endif
