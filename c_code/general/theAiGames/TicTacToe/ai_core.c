#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "ai_core.h"

#define MIN_STEP        1
#define MAX_STEP        81
#define TOTAL_TIME_MS   2000
#define HASH_BIT_SIZE   26
#define MIN_HASH_DEEP   4

enum {
	FULL = 0,
	TIED = 1,
	NORMAL_MIM = 16,
	NORMAL_MAX = NORMAL_MIM + 0x40-1,
#define rand_normal() (NORMAL_MIM+(rand()&(0x40-1)))
	WIN = 127,
};

struct position{
	point_t point;
	point_t next_p;
	int value;
};

static struct position max_score(struct ai_ttt *ai, int id, int *mboard, int steps);
static void dbg_steps(struct ai_ttt *ai, int step, struct position *last_pos);
static int get_hash(char *h, struct ai_ttt *ai, int step, int *score);
static int set_hash(char *h, struct ai_ttt *ai, int step, int *score);
static int judgement(struct ai_ttt *ai, int id, point_t p);
static int timediff_ms(struct timeval *start, struct timeval *end);
char *h;

point_t cal_point(struct ai_ttt *ai, int time_limit_ms)
{
	int step, used_time;
	struct timeval t_cal, t_end;
	struct position cur_pos = {0}, last_pos;
	for(step=MIN_STEP;step<MAX_STEP;step++){
		gettimeofday(&t_cal, NULL);
		last_pos = cur_pos;
		h = calloc(1, 1ull<<HASH_BIT_SIZE);
		if(!h){
			dbg_printf("calloc memory failed\n");
			exit(0);
		}
		cur_pos = max_score(ai, ai->id, ai->ttt->macroboard, step);
		free(h);
		gettimeofday(&t_end, NULL);
		used_time = timediff_ms(&t_cal, &t_end);
		if(cur_pos.value == WIN || cur_pos.value == -WIN ||
				cur_pos.value == TIED || cur_pos.value == -TIED ||
				used_time*20 > TOTAL_TIME_MS-used_time){
			break;
		}
	}
	//select step
	if(-cur_pos.value == -WIN && step != MIN_STEP){
		dbg_printf("step =%2d point = (%d,%d) value =%4d\n",
			step, POINT_X(last_pos.point), POINT_Y(last_pos.point), -last_pos.value);
		dbg_steps(ai, step, &last_pos);
		return last_pos.point;
	}else{
		dbg_printf("step =%2d point = (%d,%d) value =%4d\n",
			step, POINT_X(cur_pos.point), POINT_Y(cur_pos.point), -cur_pos.value);
		if(-cur_pos.value == WIN){
			dbg_steps(ai, step, &cur_pos);
		}
		return cur_pos.point;
	}
}

/****************************************
 *
 * while block == -1, it means any block is allowed
 */
static struct position max_score(struct ai_ttt *ai, int id, int *mboard, int steps)
{
	int b,o, nextmBoard[TTT_BLK_NUM];
	struct position pos[9][9], tmp;
	memset(pos, 0, sizeof(pos));
	for(b=0;b<TTT_BLK_NUM;b++){
		if(mboard[b] == 0){
			continue;
		}
		for(o=0;o<TTT_BLK_NUM;o++){
			point_t p = BO2PO(ai->ttt, b, o);
			if(ttt_add(ai->ttt, id, p) < 0){
				pos[b][o].value = FULL;
				continue;
			}else{
				pos[b][o].point = p;
				if(steps == 1){
					pos[b][o].value = judgement(ai, id, p);
				}else if(ttt_isMFinish(ai->ttt)==id){
					pos[b][o].value = WIN;
				}else{
					// get macroboard
					if(GB2(ai->ttt->m_field, o) == NORMAL){
						memset(nextmBoard, 0, sizeof(nextmBoard));
						nextmBoard[o] = -1;
					}else{
						int i;
						for(i=0;i<TTT_BLK_NUM;i++){
							if(GB2(ai->ttt->m_field, i) == NORMAL){
								nextmBoard[i] = -1;
							}else{
								nextmBoard[i] = 0;
							}
						}
					}
					//let's play next step
					tmp = max_score(ai, ttt_op_id(id), nextmBoard, steps-1);
					pos[b][o].value = tmp.value;
					pos[b][o].next_p = tmp.point;
				}
			}
			ttt_remove(ai->ttt, p);
		}
	}
	//select best point
	{
		int max_value = -WIN;
		int max_v_cnt = 0, midx;
		struct{
			int b;
			int o;
		}max[9*9];
		for(b=0;b<TTT_BLK_NUM;b++){
			if(mboard[b] == 0){
				continue;
			}
			for(o=0;o<TTT_BLK_NUM;o++){
				if(pos[b][o].value>max_value && pos[b][o].value!=FULL){
					max_value = pos[b][o].value;
					max_v_cnt = 1;
					max[0].b = b;
					max[0].o = o;
				}else if(pos[b][o].value==max_value && pos[b][o].value!=FULL){
					max[max_v_cnt].b = b;
					max[max_v_cnt].o = o;
					max_v_cnt++;
				}
			}
		}
		if(max_v_cnt == 0){//must be draw
			tmp.value = TIED;
			tmp.point = POINT(-1,-1);
			return tmp;
		}
		midx = rand()%max_v_cnt;
		pos[max[midx].b][max[midx].o].value = -pos[max[midx].b][max[midx].o].value;
		return pos[max[midx].b][max[midx].o];
	}
}
static int timediff_ms(struct timeval *start, struct timeval *end)
{
	return (end->tv_sec-start->tv_sec)*1000+(end->tv_usec-start->tv_usec)/1000;
}

uint32_t hash(void *key, size_t len)
{
	uint32_t hash, i;
	for(hash = i = 0; i < len; ++i)
	{
		hash += ((char*)key)[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

uint32_t get_hash_idx(struct ai_ttt *ai, int step)
{
	return 0;
}
static int get_hash(char *h, struct ai_ttt *ai, int step, int *score)
{
	return 0;
}

static int set_hash(char *h, struct ai_ttt *ai, int step, int *score)
{
	return 0;
}

static void dbg_steps(struct ai_ttt *ai, int step, struct position *pos)
{
	return;
}
static int judgement(struct ai_ttt *ai, int id, point_t p)
{
	return rand_normal();
}
