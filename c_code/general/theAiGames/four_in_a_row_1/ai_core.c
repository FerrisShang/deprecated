#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/time.h>
#include "ai_core.h"

#define MIN_STEP        1
#define MAX_STEP        42
#define TOTAL_TIME_MS   2500
#define HASH_BIT_SIZE   26
#define MIN_HASH_DEEP   4
#define STATIC_HASH_MEM 1

enum {
	FULL = 0,
	TIED = 1,
	NORMAL_MIM = 16,
	NORMAL_MAX = NORMAL_MIM + 0x40-1,
#define rand_normal() (NORMAL_MIM+(rand()&(0x40-1)))
	WIN = 127,
};

struct position{
	int colume;
	int value;
	int next_col;
};

static struct position max_score(struct ai_four *ai, int id, int steps);
static int judgement(struct ai_four *ai, int id, int col);
static void dbg_steps(struct ai_four *ai, int step, struct position *last_pos);
static int get_hash(char *h, struct ai_four *ai, int step, int *score);
static int set_hash(char *h, struct ai_four *ai, int step, int *score);

static int timediff_ms(struct timeval *start, struct timeval *end);
#if STATIC_HASH_MEM == 1
char h[1ull<<HASH_BIT_SIZE];
#else
char *h;
#endif

int cal_col(struct ai_four *ai, int time_limit_ms)
{
	int step, used_time;
	struct timeval t_cal, t_end;
	struct position cur_pos = {0}, last_pos;
	for(step=MIN_STEP;step<MAX_STEP;step++){
		gettimeofday(&t_cal, NULL);
		last_pos = cur_pos;
#if STATIC_HASH_MEM == 1
		memset(h, 0, sizeof(h));
		cur_pos = max_score(ai, ai->id, step);
#else
		h = calloc(1, 1ull<<HASH_BIT_SIZE);
		if(!h)
			dbg_printf("calloc memory failed\n");
		cur_pos = max_score(ai, ai->id, step);
		free(h);
#endif
		gettimeofday(&t_end, NULL);
		used_time = timediff_ms(&t_cal, &t_end);
		if(cur_pos.value == WIN || cur_pos.value == -WIN ||
				cur_pos.value == TIED || cur_pos.value == -TIED ||
				used_time*8 > TOTAL_TIME_MS-used_time){
			break;
		}
	}
	//select step
	if(-cur_pos.value == -WIN && step != MIN_STEP){
		dbg_printf("step =%2d colume = %d value =%4d\n",step,last_pos.colume,-last_pos.value);
		dbg_steps(ai, step, &last_pos);
		return last_pos.colume;
	}else{
		dbg_printf("step =%2d colume = %d value =%4d\n",step,cur_pos.colume,-cur_pos.value);
		if(-cur_pos.value == WIN){
			dbg_steps(ai, step, &cur_pos);
		}
		return cur_pos.colume;
	}
}

static struct position max_score(struct ai_four *ai, int id, int steps)
{
	int i, j;
	struct position pos[FOUR_COL], tmp;
	for(i=0;i<FOUR_COL;i++){
		pos[i].colume = i;
		pos[i].next_col = -1;
		pos[i].value = FULL;
	}
	for(i=0;i<FOUR_COL;i++){
		if(four_add(ai->four, id, i) < 0){
			continue;
		}
		if(steps <= MIN_HASH_DEEP || get_hash(h, ai, steps, &pos[i].value) < 0){
			if(four_isFinish(ai->four, id, i)){
				pos[i].value = WIN;
			}else{
				if(steps == 1){
					pos[i].value = judgement(ai, id, i);
				}else{
					tmp = max_score(ai, four_op_id(id), steps-1);
					pos[i].value = tmp.value;
					pos[i].next_col = tmp.colume;
				}
			}
			if(steps > MIN_HASH_DEEP){
				set_hash(h, ai, steps, &pos[i].value);
			}
		}
		four_remove(ai->four, id, i);
		if(pos[i].value == WIN){
			break;
		}
	}
	//select best colume
	for(i=0;i<FOUR_COL-1;i++){
		for(j=FOUR_COL-1;j>i;j--){
			if(pos[j].value>pos[j-1].value ||
					(pos[j].value==pos[j-1].value && (rand()&1))
			  ){
				tmp = pos[j];
				pos[j] = pos[j-1];
				pos[j-1] = tmp;
			}
		}
	}
	tmp.value = -TIED;
	for(i=0;i<FOUR_COL;i++){
		if(pos[i].value != FULL){
			tmp = pos[i];
			tmp.value = -tmp.value;
			break;
		}
	}
	return tmp;
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

uint32_t get_hash_idx(struct ai_four *ai, int step)
{
	uint32_t h_idx, i;
	char buf[13];
	unsigned short *f1 = ai->four->field_h[1];
	unsigned short *f2 = ai->four->field_h[2];
	for(i=0;i<6;i++){
		buf[i] = (char)f1[i];
		buf[i+6] = (char)f2[i];
	}
	buf[12] = step;
	h_idx = hash(buf, 13);
	return h_idx & ((1ull<<HASH_BIT_SIZE)-1);
}
static int get_hash(char *h, struct ai_four *ai, int step, int *score)
{
	uint32_t h_idx;
	char value;
	h_idx = get_hash_idx(ai, step);
	value = h[h_idx];
	if(value == 0){
		return -1;
	}else{
		*score = value;
	}
	return 0;
}

static int set_hash(char *h, struct ai_four *ai, int step, int *score)
{
	uint32_t h_idx;
	h_idx = get_hash_idx(ai, step);
#if 1
	if(h[h_idx]!=0){
		dbg_printf("hash replaced.\n");
	}
#endif
	h[h_idx] = *score;
	return 0;
}

static int judgement(struct ai_four *ai, int id, int col)
{
	return rand_normal();
}

static void dbg_steps(struct ai_four *ai, int step, struct position *pos)
{
	int i;
	char col_list[MAX_STEP];
	col_list[0] = pos->colume;
	dbg_printf("%d ", col_list[0]);
	four_add(ai->four, ai->id, col_list[0]);
	if(four_isFinish(ai->four, ai->id, col_list[0])){ dbg_printf("\n");return; }
	col_list[1] = pos->next_col;
	dbg_printf("%d ", col_list[1]);
	for(i=1;i<step-1;i++){
		struct position tmp;
		int cur_id = ((i&1)==0)?ai->id:four_op_id(ai->id);
		four_add(ai->four, cur_id, col_list[i]);
		if(four_isFinish(ai->four, cur_id, i)){ break; }
		tmp = max_score(ai, four_op_id(cur_id), step-1-i);
		if(tmp.colume == -WIN){
			col_list[i+1] = tmp.next_col;
		}else{
			col_list[i+1] = tmp.colume;
		}
		dbg_printf("%d ", col_list[i+1]);
	}
	dbg_printf("\n");
}
