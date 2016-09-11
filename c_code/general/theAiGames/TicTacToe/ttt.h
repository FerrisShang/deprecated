#ifndef __TTT_H__
#define __TTT_H__

#include "io.h"

#define TTT_BLK_NUM 9

#define EMPTY    0
#define NORMAL   0
#define ID_P1    1
#define ID_P2    2
#define DRAW     3

#define ttt_op_id(id) (3-id)

typedef unsigned char point_t;
#define POINT_X(p)   ((point_t)p >> 4)
#define POINT_Y(p)   ((point_t)p & 0x0F)
#define POINT(x,y)   ((x<<4)|y)
#define BO2PO(t,b,o) (t->bo2poMap[b][o])
#define PO2BO(t,p)   (t->blockMap[p])
#define PO2OF(t,p)   (t->posMap[p])

#define GB1(n,bit)   (((n)>>(bit))&3)
#define SB1(n,s,bit) (n) |= ((s)<<((bit)));
#define CB1(n,bit)   (n) &= ~((3)<<(bit));
#define GB2(n,bit)   (((n)>>(bit*2))&3)
#define SB2(n,s,bit) (n) |= ((s)<<((bit)*2));

struct ttt {
	int timebank;
	int time_per_move;
	int id;
	int s_field[TTT_BLK_NUM];
	int m_field;
	int macroboard[TTT_BLK_NUM];
	char finishMap  [1<<(TTT_BLK_NUM*2)];
	char blockMap   [1<<(sizeof(point_t)*8)];
	char blockOffMap[1<<(sizeof(point_t)*8)];
	char posMap     [1<<(sizeof(point_t)*8)];
	char posOffMap  [1<<(sizeof(point_t)*8)];
	char bo2poMap   [TTT_BLK_NUM][TTT_BLK_NUM];
};


struct ttt* ttt_create(void);
void ttt_destory(struct ttt *ttt);
int ttt_add(struct ttt *ttt, int id, point_t p);
int ttt_remove(struct ttt *ttt, point_t p);
int ttt_get_field(struct ttt *ttt, char field[][9]);
int ttt_set_field(struct ttt *ttt, char field[][9]);
int ttt_set_mboard(struct ttt *ttt, int *m_board);
int ttt_isSFinish(struct ttt *ttt, point_t last_point);
int ttt_isMFinish(struct ttt *ttt);

#endif /* __TTT_H__ */
