#ifndef __FOUR_H__
#define __FOUR_H__

#include "io.h"

#define FOUR_ROW 6
#define FOUR_COL 7

#define ID_P1    1
#define ID_P2    2
#define four_op_id(id) (3-id)

struct four {
	int timebank;
	int time_per_move;
	int id;
	unsigned short field_h[3][FOUR_ROW];
	unsigned short field_v[3][FOUR_COL];
	unsigned short field_s[3][FOUR_ROW+FOUR_COL];
	unsigned short field_bs[3][FOUR_ROW+FOUR_COL];
	char finishMap[1<<16];
	char nextPosMap[1<<8];
	char fieldSxMap[FOUR_ROW][FOUR_COL];
	char fieldSyMap[FOUR_ROW][FOUR_COL];
	char fieldBsxMap[FOUR_ROW][FOUR_COL];
	char fieldBsyMap[FOUR_ROW][FOUR_COL];
};

struct four* four_create(void);
void four_destory(struct four *four);
void four_reset(struct four *four);
int four_add(struct four *four, int id, int col);
int four_remove(struct four *four, int id, int col);
int four_set_field(struct four *four, char field[FOUR_ROW][FOUR_COL]);
int four_get_field(struct four *four, char field[FOUR_ROW][FOUR_COL]);
int four_isFinish(struct four *four, int id, int last_col);

#endif /* __FOUR_H__ */
