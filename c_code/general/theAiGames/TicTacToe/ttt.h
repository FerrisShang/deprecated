#ifndef __TTT_H__
#define __TTT_H__

#include "io.h"

#define TTT_BLK_NUM 9
#define TTT_COL 9

#define ID_P1    1
#define ID_P2    2
#define ttt_op_id(id) (3-id)

typedef unsigned char point_t;
#define GET_P_X(p) ((point_t)p >> 4)
#define GET_P_Y(p) ((point_t)p & 0x0F)
#define POINT(x,y) ((x<<4)|y)

struct ttt {
	int timebank;
	int time_per_move;
	int id;
	unsigned short field[3][TTT_BLK_NUM];
	unsigned short big_field[3];
	char finishMap[1<<16];
};


struct ttt* ttt_create(void);
void ttt_destory(struct ttt *ttt);
void ttt_reset(struct ttt *ttt);
int ttt_add(struct ttt *ttt, int id, point_t p);
int ttt_remove(struct ttt *ttt, int id, point_t p);
int ttt_set_field(struct ttt *ttt, char field[][9]);
int ttt_get_field(struct ttt *ttt, char field[][9]);
int ttt_isFinish(struct ttt *ttt, int id, point_t last_point);

#endif /* __TTT_H__ */
