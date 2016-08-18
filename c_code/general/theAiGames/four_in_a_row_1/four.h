#ifndef __FOUR_H__
#define __FOUR_H__

#include "io.h"

#define FOUR_ROW 6
#define FOUR_COL 7

#define ID_P1    1
#define ID_P2    2
#define four_op_id(id) (3-id)

struct four;

struct four* four_create(void);
void four_destory(struct four *four);
void four_reset(struct four *four);
int four_add(struct four *four, int id, int col);
int four_remove(struct four *four, int id, int col);
int four_set_field(struct four *four, char field[FOUR_ROW][FOUR_COL]);
int four_get_field(struct four *four, char field[FOUR_ROW][FOUR_COL]);
int four_isFinish(struct four *four, int id, int last_col);

#endif /* __FOUR_H__ */
